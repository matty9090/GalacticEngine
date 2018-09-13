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
      m_bounds(bounds),
      m_scale(bounds.size),
      m_transformed(Matrix::Identity)
{
    ID3D11Device *device;
    terrain->GetContext()->GetDevice(&device);

    m_buffer = std::make_unique<ConstantBuffer<MatrixBuffer>>(device);
    m_world = Matrix::Identity * (IsRoot() ? Matrix::Identity : m_parent.lock()->GetMatrix());
    m_depth = -(int)log2f(m_scale);
}

void TerrainNode::Generate()
{
    uint16_t gridsize = (uint16_t)m_terrain->GetGridSize();

    float step = m_bounds.size / (gridsize - 1);

    for (int y = 0; y < gridsize; ++y) {
        for (int x = 0; x < gridsize; ++x) {
            float xx = m_bounds.x + x * step;
            float yy = m_bounds.y + y * step;

            Vector3 pos = PointToSphere(Vector3(xx, 0.5f, yy));
            pos.Normalize();

            PlanetVertex v;
            v.position = pos;
            v.color = Vector4(0.0f, 0.2f, 1.0f, 1.0f);
            v.normal = pos;
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

        m_transformed = m_world * m_terrain->GetMatrix();

        Matrix worldViewProj = m_transformed * view * proj;
        MatrixBuffer buffer = { worldViewProj.Transpose(), m_transformed.Transpose() };
        
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
        int gridsize = m_terrain->GetGridSize();

        Vector3 cam = m_planet.lock()->GetCameraPos();
        Vector3 midpoint = m_vertices[(gridsize * gridsize) / 2].position;

        Vector3 mid = Vector3::Transform(midpoint, m_transformed);
        
        float distance = Vector3::Distance(cam, mid);

        if (distance < m_scale * 5.0f)
            Split();
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
    if (m_depth >= 8)
        return;

    if (IsLeaf())
    {
        float x = m_bounds.x, y = m_bounds.y;
        float d = m_bounds.size / 2;

        m_children[0] = std::make_shared<TerrainNode>(m_terrain, weak_from_this(), m_planet, Square{ x    , y    , d });
        m_children[1] = std::make_shared<TerrainNode>(m_terrain, weak_from_this(), m_planet, Square{ x + d, y    , d });
        m_children[2] = std::make_shared<TerrainNode>(m_terrain, weak_from_this(), m_planet, Square{ x + d, y + d, d });
        m_children[3] = std::make_shared<TerrainNode>(m_terrain, weak_from_this(), m_planet, Square{ x    , y + d, d });

        for (auto &child : m_children)
            child->Generate();
    }
    else
    {
        for (auto &child : m_children)
            child->Split();
    }
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