#include "pch.h"
#include "Render/SphericalQuadTreeTerrain/TerrainNode.hpp"

using namespace Galactic;
using namespace DirectX;
using namespace DirectX::SimpleMath;

TerrainNode::TerrainNode(std::shared_ptr<ISphericalTerrain> terrain, std::weak_ptr<TerrainNode> parent, std::weak_ptr<IPlanet> planet)
    : Drawable<PlanetVertex>(terrain->GetContext().Get(), D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST),
      m_terrain(terrain),
      m_parent(parent),
      m_planet(planet)
{
    ID3D11Device *device;
    terrain->GetContext()->GetDevice(&device);

    m_buffer = std::make_unique<ConstantBuffer<MatrixBuffer>>(device);
}

void TerrainNode::Generate()
{
    uint16_t gridsize = (uint16_t)m_terrain->GetGridSize();

    float step = 5.0f / (gridsize - 1);

    for (int y = 0; y < gridsize; ++y) {
        for (int x = 0; x < gridsize; ++x) {
            float xx = x * step - 0.5f;
            float yy = y * step - 0.5f;

            PlanetVertex v;
            v.position = Vector3(xx, -1.0f, yy);
            //v.normal = -Vector3::Forward;
            //v.tangent = v.normal;
            v.color = Vector4(1.0f, 1.0f, 1.0f, 1.0f);

            m_vertices.push_back(v);
        }
    }

    for (uint16_t y = 0; y < gridsize - 1; ++y) {
        for (uint16_t x = 0; x < gridsize - 1; ++x) {
            m_indices.push_back(y * gridsize + x);
            m_indices.push_back(y * gridsize + x + 1);
            m_indices.push_back((y + 1) * gridsize + x);
            m_indices.push_back((y + 1) * gridsize + x);
            m_indices.push_back(y * gridsize + x + 1);
            m_indices.push_back((y + 1) * gridsize + x + 1);
        }
    }

    Init();
}

void TerrainNode::Render(DirectX::SimpleMath::Matrix view, DirectX::SimpleMath::Matrix proj)
{
    if (IsLeaf())
    {
        PreDraw();

        Matrix worldViewProj = m_world * view * proj;
        MatrixBuffer buffer = { worldViewProj };
        
        m_buffer->SetData(m_terrain->GetContext().Get(), buffer);
        auto c_buffer = m_buffer->GetBuffer();

        m_terrain->GetContext()->VSSetConstantBuffers(0, 1, &c_buffer);

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
