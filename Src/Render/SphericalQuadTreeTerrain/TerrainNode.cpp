#include "pch.h"
#include "Render/SphericalQuadTreeTerrain/TerrainNode.hpp"
#include "Render/SphericalQuadTreeTerrain/SphericalQuadTreeTerrain.hpp"

#include <thread>
#include <mutex>

using namespace Galactic;
using namespace DirectX;
using namespace DirectX::SimpleMath;

float TerrainNode::SplitDistance = 34.0f;

TerrainNode::TerrainNode(ISphericalTerrain *terrain, TerrainNode *parent, IPlanet *planet, Square bounds, int quad, bool simple)
    : Drawable<PlanetVertex>(terrain->GetContext().Get(), D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST),
      m_terrain(terrain),
      m_parent(parent),
      m_planet(planet),
      m_bounds(bounds),
      m_scale(bounds.size),
      m_visible(true),
      m_quad(quad),
      m_simple(simple),
	  m_neighbours()
{
    ID3D11Device *device;
    m_context->GetDevice(&device);

    m_device = device;
    m_world = IsRoot() ? Matrix::Identity : m_parent->GetMatrix();
    m_depth = -(int)log2f(m_scale);
    m_diameter = m_scale * m_terrain->GetRadius() * 2;
    m_buffer = std::make_unique<ConstantBuffer<MatrixBuffer>>(device);

#ifdef _DEBUG
    m_dbgCol    = Color(0.0f, 0.0f, 1.0f, 0.15f);
    m_dbgStates = std::make_unique<CommonStates>(device);
    m_dbgEffect = std::make_unique<BasicEffect>(device);
    m_dbgEffect->SetVertexColorEnabled(true);

    void const* shaderByteCode;
    size_t byteCodeLength;

    m_dbgEffect->GetVertexShaderBytecode(&shaderByteCode, &byteCodeLength);

    DX::ThrowIfFailed(device->CreateInputLayout(VertexPositionColor::InputElements, VertexPositionColor::InputElementCount, shaderByteCode, byteCodeLength, m_dbgInputLayout.ReleaseAndGetAddressOf()));

    m_dbgBatch = std::make_unique<PrimitiveBatch<VertexPositionColor>>(terrain->GetContext().Get());
#endif
}

void TerrainNode::Generate()
{
    uint16_t gridsize = (uint16_t)m_terrain->GetGridSize();

    float step = m_bounds.size / (gridsize - 1);
    bool hasParent = !IsRoot();

    uint16_t k = 0;
    int sx = 0, sy = 0;
    int gh = (gridsize - 1) / 2;

    switch (m_quad)
    {
        case NW: sx = 0, sy = 0; break;
        case NE: sx = gh, sy = 0; break;
        case SE: sx = gh, sy = gh; break;
        case SW: sx = 0, sy = gh; break;
    }

    auto parent = m_parent;

    for (int y = 0; y < gridsize; ++y)
    {
        float yy = m_bounds.y + y * step;

        for (int x = 0; x < gridsize; ++x, ++k)
        {
            PlanetVertex v;

            if (hasParent && (x % 2 == 0) && (y % 2 == 0))
            {
                int xh = sx + x / 2;
                int yh = sy + y / 2;

                v = parent->GetVertex(xh + yh * gridsize);
				v.position1 = v.position2;
                v.normal1 = v.normal2;
            }
            else
            {
                float xx = m_bounds.x + x * step;

                Vector3 pos = Vector3(xx, 0.5f, yy);

                pos.Normalize();
                pos = Vector3::TransformNormal(pos, m_world);

                std::string tex;
                float height = 0.0f;
                Vector2 biome;
                float texIndex = 0.0f, normalIndex = 0.0f;
                
                if (!m_simple)
                {
                    m_terrain->GetHeight(pos, height, biome, tex);
                    texIndex = static_cast<float>(BiomeConfig::Biomes[tex].Tex);
                    normalIndex = static_cast<float>(BiomeConfig::Biomes[tex].NormalMap);
                }

                bool calcNormal = true;
                Vector3 avgPos  = pos + pos * height;
                Vector3 avgNorm = Vector3::Zero;

                if (hasParent)
                {
                    int xh1 = sx + x / 2, xh2 = sx + x / 2;
                    int yh1 = sy + y / 2, yh2 = sy + y / 2;

                    if (x % 2 != 0 && y % 2 == 0)
                        xh2++;
                    else if (x % 2 != 0 && y % 2 != 0)
                    {
                        xh2++;
                        yh2++;
                    }
                    else if(x % 2 == 0 && y % 2 != 0)
                        yh2++;

                    avgPos = (parent->GetVertex(xh1 + yh1 * gridsize).position2 + (parent->GetVertex(xh2 + yh2 * gridsize).position2)) / 2;
                    avgNorm = (parent->GetVertex(xh1 + yh1 * gridsize).normal2 + (parent->GetVertex(xh2 + yh2 * gridsize).normal2)) / 2;
                    calcNormal = false;
                }

                v.biome = biome;
                v.position1 = avgPos;
                v.position2 = pos + pos * height;
                v.normal1 = avgNorm;
                v.normal2 = Vector3::Zero;
                v.sphere = pos;
                v.uv = Vector3(xx * 10000.0f, yy * 10000.0f, texIndex);
                v.normalIndex = normalIndex;
                v.calcNormal = calcNormal;
            }

            if (x == 0)             m_edges[West].push_back(k);
            if (x >= gridsize - 1)  m_edges[East].push_back(k);
            if (y == 0)             m_edges[North].push_back(k);
            if (y >= gridsize - 1)  m_edges[South].push_back(k);

            m_vertices.push_back(v);
        }
    }

    m_indices = SphericalQuadTreeTerrain::Perms[SphericalQuadTreeTerrain::None];

    CalculateNormals();

    m_planet->IncrementVertices(gridsize * gridsize * 4);
}

void TerrainNode::Render(DirectX::SimpleMath::Matrix view, DirectX::SimpleMath::Matrix proj)
{
    if (m_visible)
    {
        if (IsLeaf())
        {
            auto gridsize = m_terrain->GetGridSize();

            Vector3 midpoint = m_vertices[(gridsize * gridsize) / 2].position2;
            Vector3 mid = Vector3::Transform(midpoint, m_terrain->GetMatrix());

            float lerp  = (Vector3::Distance(m_planet->GetCameraPos(), m_planet->GetPosition()) - (float)(m_planet->GetRadius() / Constants::Scale)) * 2.0f;
            float morph = m_scale * SplitDistance;

            if (lerp < 0.0f) lerp = 0.0f;
            if (lerp > 1.0f) lerp = 1.0f;

            Matrix worldViewProj = m_terrain->GetMatrix() * view * proj;
            MatrixBuffer buffer = { worldViewProj.Transpose(), m_terrain->GetMatrix().Transpose(), m_planet->GetCameraPos(), 1.0f - lerp, morph };

            m_buffer->SetData(m_context, buffer);

            m_context->VSSetConstantBuffers(0, 1, m_buffer->GetBuffer());
            m_context->PSSetConstantBuffers(0, 1, m_buffer->GetBuffer());

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
    size_t gridsize = m_terrain->GetGridSize();

    Vector3 cam = m_planet->GetCameraPos();
    Vector3 midpoint = m_vertices[(gridsize * gridsize) / 2].position2;
    Vector3 mid = Vector3::Transform(midpoint, m_terrain->GetMatrix());

    float distance = Vector3::Distance(cam, mid);
    float height = (cam - m_planet->GetPosition()).Length() - m_terrain->GetRadius() * 0.96f;

    float horizon = sqrtf(height * (2 * m_terrain->GetRadius() + height));
    m_visible = distance - m_diameter < horizon;

    if (m_visible)
    {
        float splitDist = m_simple ? SplitDistance / 2.0f : SplitDistance;
        bool divide = (!m_simple && m_depth < 2) || distance < m_scale * splitDist;

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
    m_buffer.reset();
    
#ifdef _DEBUG
    m_dbgStates.reset();
    m_dbgEffect.reset();
    m_dbgBatch.reset();
    m_dbgInputLayout.Reset();
#endif

    Cleanup();
}

void TerrainNode::Split()
{
    if (m_depth >= 12 || SphericalQuadTreeTerrain::FrameSplits >= SphericalQuadTreeTerrain::MaxSplitsPerFrame)
        return;

    if (IsLeaf())
    {
        SphericalQuadTreeTerrain::FrameSplits++;

        float x = m_bounds.x, y = m_bounds.y;
        float d = m_bounds.size / 2;

        m_children[NW] = std::make_unique<TerrainNode>(m_terrain, this, m_planet, Square{ x    , y    , d }, NW, m_simple);
        m_children[NE] = std::make_unique<TerrainNode>(m_terrain, this, m_planet, Square{ x + d, y    , d }, NE, m_simple);
        m_children[SE] = std::make_unique<TerrainNode>(m_terrain, this, m_planet, Square{ x + d, y + d, d }, SE, m_simple);
        m_children[SW] = std::make_unique<TerrainNode>(m_terrain, this, m_planet, Square{ x    , y + d, d }, SW, m_simple);

#ifdef _DEBUG
        m_children[NW]->SetDebugName(m_dbgName + "_" + std::to_string(NW));
        m_children[NE]->SetDebugName(m_dbgName + "_" + std::to_string(NE));
        m_children[SE]->SetDebugName(m_dbgName + "_" + std::to_string(SE));
        m_children[SW]->SetDebugName(m_dbgName + "_" + std::to_string(SW));
#endif

        std::vector<std::thread> threads;

        for (auto &child : m_children)
            threads.push_back(std::thread([&]() { child->Generate(); }));

        for (auto &t : threads)
            t.join();

        threads.clear();

        for (auto &child : m_children)
            threads.push_back(std::thread([&]() { child->FixEdges(); }));

        for (auto &t : threads)
            t.join();

        NotifyNeighbours();
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

        size_t gridsize = m_terrain->GetGridSize();
        m_planet->IncrementVertices(-static_cast<int>(gridsize * gridsize * 4));

        NotifyNeighbours();
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

void TerrainNode::FixEdges()
{
    m_neighbours[North] = GetGreaterThanOrEqualNeighbour(North);
    m_neighbours[East] = GetGreaterThanOrEqualNeighbour(East);
    m_neighbours[South] = GetGreaterThanOrEqualNeighbour(South);
    m_neighbours[West] = GetGreaterThanOrEqualNeighbour(West);

    auto depthNorth = m_neighbours[North] && (m_depth - m_neighbours[North]->GetDepth() >= 1);
    auto depthEast = m_neighbours[East]   && (m_depth - m_neighbours[East]->GetDepth()  >= 1);
    auto depthSouth = m_neighbours[South] && (m_depth - m_neighbours[South]->GetDepth() >= 1);
    auto depthWest = m_neighbours[West]   && (m_depth - m_neighbours[West]->GetDepth()  >= 1);

    if (!depthNorth && !depthEast && !depthSouth && !depthWest)
        m_indices = SphericalQuadTreeTerrain::Perms[SphericalQuadTreeTerrain::None];
    else if (depthNorth && !depthEast && !depthSouth && !depthWest)
        m_indices = SphericalQuadTreeTerrain::Perms[SphericalQuadTreeTerrain::Top];
    else if (!depthNorth && !depthEast && depthSouth && !depthWest)
        m_indices = SphericalQuadTreeTerrain::Perms[SphericalQuadTreeTerrain::Bottom];
    else if (!depthNorth && !depthEast && !depthSouth && depthWest)
        m_indices = SphericalQuadTreeTerrain::Perms[SphericalQuadTreeTerrain::Left];
    else if (!depthNorth && depthEast && !depthSouth && !depthWest)
        m_indices = SphericalQuadTreeTerrain::Perms[SphericalQuadTreeTerrain::Right];
    else if (depthNorth && depthEast && !depthSouth && !depthWest)
        m_indices = SphericalQuadTreeTerrain::Perms[SphericalQuadTreeTerrain::TopRight];
    else if (!depthNorth && depthEast && depthSouth && !depthWest)
        m_indices = SphericalQuadTreeTerrain::Perms[SphericalQuadTreeTerrain::RightBottom];
    else if (!depthNorth && !depthEast && depthSouth && depthWest)
        m_indices = SphericalQuadTreeTerrain::Perms[SphericalQuadTreeTerrain::BottomLeft];
    else if (depthNorth && !depthEast && !depthSouth && depthWest)
        m_indices = SphericalQuadTreeTerrain::Perms[SphericalQuadTreeTerrain::LeftTop];
    
    if (m_neighbours[North]) FixEdge(North, m_neighbours[North], m_neighbours[North]->GetEdge(South), m_neighbours[North]->GetDepth());
    if (m_neighbours[East])  FixEdge(East,  m_neighbours[East],  m_neighbours[East]->GetEdge(West),   m_neighbours[East]->GetDepth());
    if (m_neighbours[South]) FixEdge(South, m_neighbours[South], m_neighbours[South]->GetEdge(North), m_neighbours[South]->GetDepth());
    if (m_neighbours[West])  FixEdge(West,  m_neighbours[West],  m_neighbours[West]->GetEdge(East),   m_neighbours[West]->GetDepth());

    Init();
}

void TerrainNode::FixEdge(EDir dir, TerrainNode *neighbour, std::vector<uint16_t> nEdge, int depth)
{
    int diff = m_depth - depth;
    size_t grid = m_terrain->GetGridSize();

    if (diff == 0)
    {
        for (int i = 0; i < grid; ++i)
        {
            m_vertices[m_edges[dir][i]].normal2 = neighbour->GetVertex(nEdge[i]).normal2;
        }
    }
}

void TerrainNode::CalculateNormals()
{
    for (size_t i = 0; i < m_indices.size(); i += 3)
    {
        Vector3 p1 = m_vertices[m_indices[i + 0]].position2;
        Vector3 p2 = m_vertices[m_indices[i + 1]].position2;
        Vector3 p3 = m_vertices[m_indices[i + 2]].position2;

        Vector3 t1 = m_vertices[m_indices[i + 0]].uv;
        Vector3 t2 = m_vertices[m_indices[i + 1]].uv;
        Vector3 t3 = m_vertices[m_indices[i + 2]].uv;

        Vector3 vector1 = p2 - p1, vector2 = p3 - p1;
        Vector2 tuVector = t2 - t1, tvVector = t3 - t1;
        Vector3 n = vector2.Cross(vector1);
        Vector3 tangent;

        float den = 1.0f / (tuVector.x * tvVector.y - tuVector.y * tvVector.x);

        tangent.x = (tvVector.y * vector1.x - tvVector.x * vector2.x) * den;
        tangent.y = (tvVector.y * vector1.y - tvVector.x * vector2.y) * den;
        tangent.z = (tvVector.y * vector1.z - tvVector.x * vector2.z) * den;

        m_vertices[m_indices[i + 0]].normal2 += n;
        m_vertices[m_indices[i + 1]].normal2 += n;
        m_vertices[m_indices[i + 2]].normal2 += n;

        m_vertices[m_indices[i + 0]].tangent += tangent;
        m_vertices[m_indices[i + 1]].tangent += tangent;
        m_vertices[m_indices[i + 2]].tangent += tangent;
    }

    for (size_t i = 0; i < m_indices.size(); ++i)
    {
        m_vertices[m_indices[i]].normal2.Normalize();
        m_vertices[m_indices[i]].tangent.Normalize();

        if (m_vertices[m_indices[i]].calcNormal)
           m_vertices[m_indices[i]].normal1 = m_vertices[m_indices[i]].normal2;
    }
}

void TerrainNode::NotifyNeighbours()
{
    std::vector<TerrainNode*> neighbours;

    auto n1 = GetGreaterThanOrEqualNeighbour(North);
    auto n2 = GetGreaterThanOrEqualNeighbour(East);
    auto n3 = GetGreaterThanOrEqualNeighbour(South);
    auto n4 = GetGreaterThanOrEqualNeighbour(West);

    auto l1 = GetSmallerNeighbours(n1, North);
    auto l2 = GetSmallerNeighbours(n2, East);
    auto l3 = GetSmallerNeighbours(n3, South);
    auto l4 = GetSmallerNeighbours(n4, West);

    neighbours.insert(neighbours.end(), l1.begin(), l1.end());
    neighbours.insert(neighbours.end(), l2.begin(), l2.end());
    neighbours.insert(neighbours.end(), l3.begin(), l3.end());
    neighbours.insert(neighbours.end(), l4.begin(), l4.end());

    for (auto n : neighbours)
        n->FixEdges();
}

std::vector<TerrainNode*> TerrainNode::GetSmallerNeighbours(TerrainNode *neighbour, int dir) const {
    std::vector<TerrainNode*> neighbours;
    std::queue<TerrainNode*> nodes;

    if (neighbour)
        nodes.push(neighbour);

    switch (dir) {
        case North: {
            while (nodes.size() > 0) {
                if (nodes.front()->IsLeaf())
                    neighbours.push_back(nodes.front());
                else {
                    nodes.push(nodes.front()->GetChild(SW));
                    nodes.push(nodes.front()->GetChild(SE));
                }

                nodes.pop();
            }

            break;
        }

        case South: {
            while (nodes.size() > 0) {
                if (nodes.front()->IsLeaf())
                    neighbours.push_back(nodes.front());
                else {
                    nodes.push(nodes.front()->GetChild(NW));
                    nodes.push(nodes.front()->GetChild(NE));
                }

                nodes.pop();
            }

            break;
        }

        case East: {
            while (nodes.size() > 0) {
                if (nodes.front()->IsLeaf())
                    neighbours.push_back(nodes.front());
                else {
                    nodes.push(nodes.front()->GetChild(NW));
                    nodes.push(nodes.front()->GetChild(SW));
                }

                nodes.pop();
            }

            break;
        }

        case West: {
            while (nodes.size() > 0) {
                if (nodes.front()->IsLeaf())
                    neighbours.push_back(nodes.front());
                else {
                    nodes.push(nodes.front()->GetChild(NE));
                    nodes.push(nodes.front()->GetChild(SE));
                }

                nodes.pop();
            }

            break;
        }
    }

    return neighbours;
}

TerrainNode *TerrainNode::GetGreaterThanOrEqualNeighbour(int dir) const {
    auto parent = m_parent;
    auto self = this;
    
    switch (dir) {
        case North: {
            if (!parent) return nullptr;
            if (parent->GetChild(SW) == self) return parent->GetChild(NW);
            if (parent->GetChild(SE) == self) return parent->GetChild(NE);

           auto node = parent->GetGreaterThanOrEqualNeighbour(dir);

            if (!node || node->IsLeaf())
                return node;

            return (parent->GetChild(NW) == self) ? node->GetChild(SW) : node->GetChild(SE);

            break;
        }

        case South: {
            if (!parent) return nullptr;
            if (parent->GetChild(NW) == self) return parent->GetChild(SW);
            if (parent->GetChild(NE) == self) return parent->GetChild(SE);

            auto node = parent->GetGreaterThanOrEqualNeighbour(dir);

            if (!node || node->IsLeaf())
                return node;

            return (parent->GetChild(SW) == self) ? node->GetChild(NW) : node->GetChild(NE);

            break;
        }

        case East: {
            if (!parent) return nullptr;
            if (parent->GetChild(NW) == self) return parent->GetChild(NE);
            if (parent->GetChild(SW) == self) return parent->GetChild(SE);

            auto node = parent->GetGreaterThanOrEqualNeighbour(dir);

            if (!node || node->IsLeaf())
                return node;

            return (parent->GetChild(NE) == self) ? node->GetChild(NW) : node->GetChild(SW);

            break;
        }

        case West: {
            if (!parent) return nullptr;
            if (parent->GetChild(NE) == self) return parent->GetChild(NW);
            if (parent->GetChild(SE) == self) return parent->GetChild(SW);

            auto node = parent->GetGreaterThanOrEqualNeighbour(dir);

            if (!node || node->IsLeaf())
                return node;

            return (parent->GetChild(NW) == self) ? node->GetChild(NE) : node->GetChild(SE);

            break;
        }
    }

    return nullptr;
}
