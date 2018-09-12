#include "pch.h"
#include "Render/SphericalQuadTreeTerrain/TerrainNode.hpp"

using namespace Galactic;
using namespace DirectX;
using namespace DirectX::SimpleMath;

TerrainNode::TerrainNode(std::shared_ptr<ISphericalTerrain> terrain, std::weak_ptr<TerrainNode> parent, std::weak_ptr<IPlanet> planet, Square bounds)
    : Drawable<PlanetVertex>(terrain->GetContext().Get(), D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST),
      m_terrain(terrain),
      m_parent(parent),
      m_planet(planet),
      m_bounds(bounds)
{
    ID3D11Device *device;
    terrain->GetContext()->GetDevice(&device);

    m_buffer = std::make_unique<ConstantBuffer<MatrixBuffer>>(device);
    m_world = Matrix::Identity;
}

void TerrainNode::Generate()
{
    uint16_t gridsize = (uint16_t)m_terrain->GetGridSize();

    float step = m_bounds.size / (gridsize - 1);

    for (int y = 0; y < gridsize; ++y) {
        for (int x = 0; x < gridsize; ++x) {
            float xx = m_bounds.start + x * step;
            float yy = m_bounds.start + y * step;

            Vector3 pos = PointToSphere(Vector3(xx, 0.5f, yy));
            pos.Normalize();

            PlanetVertex v;
            v.position = pos;
            v.color = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
            v.normal = Vector3::Up;
            //v.tangent = v.normal;

            m_vertices.push_back(v);
        }
    }

    for (uint16_t y = 0; y < gridsize - 1; ++y) {
        for (uint16_t x = 0; x < gridsize - 1; ++x) {
            m_indices.push_back(y * gridsize + x);
            m_indices.push_back(y * gridsize + x + 1);
            m_indices.push_back((y + 1) * gridsize + x);
            m_indices.push_back((y + 1) * gridsize + x);
            m_indices.push_back((y + 1) * gridsize + x + 1);
            m_indices.push_back(y * gridsize + x + 1);
        }
    }

    Init();
}

void TerrainNode::Render(DirectX::SimpleMath::Matrix view, DirectX::SimpleMath::Matrix proj)
{
    if (IsLeaf())
    {
        PreDraw();

        Matrix worldViewProj = m_world * m_terrain->GetMatrix() * view * proj;
        MatrixBuffer buffer = { worldViewProj.Transpose() };
        
        m_buffer->SetData(m_terrain->GetContext().Get(), buffer);

        m_terrain->GetContext()->VSSetConstantBuffers(0, 1, m_buffer->GetBuffer());

        Draw();
    }
    else
    {
        for (auto &child : m_children)
            child->Render(view, proj);
    }
}

void TerrainNode::Update(float dt)
{
    if (IsLeaf())
    {

    }
    else
    {
        for (auto &child : m_children)
            child->Update(dt);
    }
}

void TerrainNode::Reset()
{
    m_inputLayout.Reset();
    Cleanup();
}

void TerrainNode::Split()
{

}

void TerrainNode::Merge()
{

}

void Galactic::TerrainNode::Release()
{
    if (!IsLeaf())
    {
        for (auto &child : m_children)
            child->Release();
    }

    Reset();
}

DirectX::SimpleMath::Vector3 TerrainNode::PointToSphere(DirectX::SimpleMath::Vector3 p)
{
    float x2 = p.x * p.x, y2 = p.y * p.y, z2 = p.z * p.z;

    return Vector3(p.x * sqrtf(1.0f - y2 * 0.5f - z2 * 0.5f + (y2 * z2) * 0.33333333f),
                   p.y * sqrtf(1.0f - z2 * 0.5f - x2 * 0.5f + (z2 * x2) * 0.33333333f),
                   p.z * sqrtf(1.0f - x2 * 0.5f - y2 * 0.5f + (x2 * y2) * 0.33333333f));
}