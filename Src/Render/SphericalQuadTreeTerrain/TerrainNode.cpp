#include "pch.h"
#include "Render/SphericalQuadTreeTerrain/TerrainNode.hpp"

using namespace Galactic;
using namespace DirectX;
using namespace DirectX::SimpleMath;

TerrainNode::TerrainNode(std::shared_ptr<ISphericalTerrain> terrain, std::weak_ptr<TerrainNode> parent, std::weak_ptr<IPlanet> planet, Square bounds, int quad)
    : Drawable<PlanetVertex>(terrain->GetContext().Get(), D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST),
      m_terrain(terrain),
      m_parent(parent),
      m_planet(planet),
      m_bounds(bounds),
      m_scale(bounds.size),
      m_visible(true),
      m_quad(quad)
{
    ID3D11Device *device;
    terrain->GetContext()->GetDevice(&device);

    m_buffer = std::make_unique<ConstantBuffer<MatrixBuffer>>(device);
    m_world = IsRoot() ? Matrix::Identity : m_parent.lock()->GetMatrix();
    m_depth = -(int)log2f(m_scale);
    m_diameter = m_scale * m_terrain->GetRadius() * 2;
}

void TerrainNode::Generate()
{
    uint16_t gridsize = (uint16_t)m_terrain->GetGridSize();

    float step = m_bounds.size / (gridsize - 1);
    bool hasParent = !IsRoot();

    int sx, sy;
    int gh = (gridsize - 1) / 2;

    switch (m_quad)
    {
        case NW: sx = 0, sy = 0; break;
        case NE: sx = gh, sy = 0; break;
        case SE: sx = gh, sy = gh; break;
        case SW: sx = 0, sy = gh; break;
    }

    auto parent = m_parent.lock();

    for (int y = 0; y < gridsize; ++y)
    {
        float yy = m_bounds.y + y * step;

        for (int x = 0; x < gridsize; ++x)
        {
            PlanetVertex v;

            if (hasParent && (x % 2 == 0) && (y % 2 == 0))
            {
                int xh = sx + x / 2;
                int yh = sy + y / 2;

                v = parent->GetVertex(xh + yh * gridsize);
                v.normal = Vector3::Zero;
            }
            else
            {
                float xx = m_bounds.x + x * step;

                Vector3 pos = Vector3(xx, 0.5f, yy);

                pos.Normalize();
                pos = Vector3::Transform(pos, m_world);

                v.color = Vector4(0.5f, 0.5f, 0.5f, 1.0f);
                v.position = pos + pos * m_terrain->GetHeight(pos);
            }

            m_vertices.push_back(v);
        }
    }

    parent.reset();

    for (uint16_t y = 0; y < gridsize - 1; ++y)
    {
        for (uint16_t x = 0; x < gridsize - 1; ++x)
        {
            m_indices.push_back(y * gridsize + x);
            m_indices.push_back(y * gridsize + x + 1);
            m_indices.push_back((y + 1) * gridsize + x);
            m_indices.push_back(y * gridsize + x + 1);
            m_indices.push_back((y + 1) * gridsize + x + 1);
            m_indices.push_back((y + 1) * gridsize + x);
        }
    }

    for (size_t i = 0; i < m_indices.size() - 3; i += 3)
    {
        Vector3 p1 = m_vertices[m_indices[i + 0]].position;
        Vector3 p2 = m_vertices[m_indices[i + 1]].position;
        Vector3 p3 = m_vertices[m_indices[i + 2]].position;

        Vector3 n = (p3 - p1).Cross(p2 - p1);

        m_vertices[m_indices[i + 0]].normal += n;
        m_vertices[m_indices[i + 1]].normal += n;
        m_vertices[m_indices[i + 2]].normal += n;
    }

    Init();
}

void TerrainNode::Render(DirectX::SimpleMath::Matrix view, DirectX::SimpleMath::Matrix proj)
{
    if (m_visible)
    {
        if (IsLeaf())
        {
            PreDraw();

            Matrix worldViewProj = m_terrain->GetMatrix() * view * proj;
            MatrixBuffer buffer = { worldViewProj.Transpose(), m_terrain->GetMatrix().Transpose() };

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
}

void TerrainNode::Update(float dt)
{
    int gridsize = m_terrain->GetGridSize();

    Vector3 cam = m_planet.lock()->GetCameraPos();
    Vector3 midpoint = m_vertices[(gridsize * gridsize) / 2].position;
    Vector3 mid = Vector3::Transform(midpoint, m_terrain->GetMatrix());

    float distance = Vector3::Distance(cam, mid);
    float height = (cam - m_planet.lock()->GetPosition()).Length() - m_terrain->GetRadius() * 0.96f;

    float horizon = sqrtf(height * (2 * m_terrain->GetRadius() + height));
    m_visible = distance - m_diameter < horizon;

    if (m_visible)
    {
        bool divide = distance < m_scale * 30.0f;

        if (!divide)
            Merge();

        if (IsLeaf() && divide)
        {
            Split();
        }
        else if (!IsLeaf())
        {
            for (auto &child : m_children)
                child->Update(dt);
        }
    }
}

void TerrainNode::Reset()
{
    m_terrain.reset();
    m_buffer.reset();

    Cleanup();
}

void TerrainNode::Split()
{
    if (m_depth >= 9)
        return;

    if (IsLeaf())
    {
        float x = m_bounds.x, y = m_bounds.y;
        float d = m_bounds.size / 2;

        m_children[0] = std::make_shared<TerrainNode>(m_terrain, weak_from_this(), m_planet, Square{ x    , y    , d }, NW);
        m_children[1] = std::make_shared<TerrainNode>(m_terrain, weak_from_this(), m_planet, Square{ x + d, y    , d }, NE);
        m_children[2] = std::make_shared<TerrainNode>(m_terrain, weak_from_this(), m_planet, Square{ x + d, y + d, d }, SE);
        m_children[3] = std::make_shared<TerrainNode>(m_terrain, weak_from_this(), m_planet, Square{ x    , y + d, d }, SW);

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
    if (IsLeaf())
        return;

    if (m_children[0]->IsLeaf())
    {
        for (auto &child : m_children) {
            child->Reset();
        }

        m_children[0].reset();
        m_children[1].reset();
        m_children[2].reset();
        m_children[3].reset();
    }
    else
    {
        for (auto &child : m_children)
            child->Merge();
    }
}

void TerrainNode::Release()
{
    if (!IsLeaf())
    {
        for (auto &child : m_children)
            child->Release();
    }

    Reset();
}

Vector3 TerrainNode::CalculateNormal(float x, float y, float step)
{
    std::array<float, 9> s;
    Vector3 n;
    uint32_t i = 0;

    for (int yy = -1; yy <= 1; yy++) {
        for (int xx = -1; xx <= 1; xx++) {
            Vector3 v = Vector3(x + (float)xx * step, 0.5f, y + (float)yy * step);
            v.Normalize();
            s[i++] = m_terrain->GetHeight(Vector3::Transform(v, m_world));
        }
    }

    float scale = 1.0f;

    n.x = scale * -(s[2] - s[0] + 2 * (s[5] - s[3]) + s[8] - s[6]);
    n.z = scale * -(s[6] - s[0] + 2 * (s[7] - s[1]) + s[8] - s[2]);
    n.y = 1.0;

    n.Normalize();

    return n;
}

Vector3 TerrainNode::PointToSphere(DirectX::SimpleMath::Vector3 p)
{
    float x2 = p.x * p.x, y2 = p.y * p.y, z2 = p.z * p.z;

    return Vector3(p.x * sqrtf(1.0f - y2 * 0.5f - z2 * 0.5f + (y2 * z2) * 0.33333333f),
                   p.y * sqrtf(1.0f - z2 * 0.5f - x2 * 0.5f + (z2 * x2) * 0.33333333f),
                   p.z * sqrtf(1.0f - x2 * 0.5f - y2 * 0.5f + (x2 * y2) * 0.33333333f));
}