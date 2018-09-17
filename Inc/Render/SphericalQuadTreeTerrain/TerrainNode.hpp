#pragma once

#include "Body/IPlanet.hpp"
#include "ISphericalTerrain.hpp"
#include "Render/Drawable.hpp"
#include "Render/DirectX/ConstantBuffer.hpp"

#include <array>

namespace Galactic
{
    struct PlanetVertex
    {
        DirectX::SimpleMath::Vector3 position;
        DirectX::SimpleMath::Vector3 normal;
        DirectX::SimpleMath::Vector4 color;
    };

    struct MatrixBuffer
    {
        DirectX::SimpleMath::Matrix worldViewProj; // 64 bytes
        DirectX::SimpleMath::Matrix world; // 64 bytes
    };

    struct Square
    {
        float x, y;
        float size;
    };
    
    class TerrainNode : public IRenderable, public Drawable<PlanetVertex>, public std::enable_shared_from_this<TerrainNode>
    {
        public:
            enum EDir { North, East, South, West };
            enum EQuad { NE, NW, SE, SW };

            TerrainNode(std::shared_ptr<ISphericalTerrain> terrain, std::weak_ptr<TerrainNode> parent, std::weak_ptr<IPlanet> planet, Square bounds, int quad);

            void Render(DirectX::SimpleMath::Matrix view, DirectX::SimpleMath::Matrix proj);
            void Update(float dt);
            void Reset();

            PlanetVertex &GetVertex(int i) { return m_originalVertices[i]; }
            DirectX::SimpleMath::Matrix &GetMatrix() { return m_world; }

            void Generate();
            bool IsLeaf() { return m_children[0] == nullptr; }
            void Split();
            void Merge();
            void Release();

            bool IsRoot() const { return !m_parent.lock(); }
            void SetDebugName(std::string name) { m_dbgName = name; }
            void SetDebugColour(DirectX::SimpleMath::Color col) { m_dbgCol = col; }
            void FixEdges();

            int GetDepth() const { return m_depth; }
            std::vector<uint16_t> GetEdge(EDir edge) const { return m_edges[edge]; }

            std::shared_ptr<TerrainNode> GetChild(int dir) const { return m_children[dir]; }

        private:
            int m_depth;
            bool m_visible;
            float m_scale, m_diameter;

            int m_quad;
            Square m_bounds;

            std::weak_ptr<TerrainNode> m_parent;
            std::weak_ptr<IPlanet> m_planet;
            std::shared_ptr<ISphericalTerrain> m_terrain;
            std::unique_ptr<ConstantBuffer<MatrixBuffer>> m_buffer;

            // Debug
            std::string m_dbgName;
            DirectX::SimpleMath::Color m_dbgCol;
            std::unique_ptr<DirectX::CommonStates> m_dbgStates;
            std::unique_ptr<DirectX::BasicEffect> m_dbgEffect;
            std::unique_ptr<DirectX::PrimitiveBatch<DirectX::VertexPositionColor>> m_dbgBatch;
            Microsoft::WRL::ComPtr<ID3D11InputLayout> m_dbgInputLayout;

            std::vector<PlanetVertex> m_originalVertices;
            std::array<std::vector<uint16_t>, 4> m_edges;
            std::array<std::shared_ptr<TerrainNode>, 4> m_children;

            DirectX::SimpleMath::Matrix m_world;

            DirectX::SimpleMath::Vector3 CalculateNormal(float x, float y, float step);
            DirectX::SimpleMath::Vector3 PointToSphere(DirectX::SimpleMath::Vector3 point);

            void NotifyNeighbours();
            void FixEdge(EDir dir, std::shared_ptr<TerrainNode> neighbour, std::vector<uint16_t> nEdge, int depth);
            
            std::shared_ptr<TerrainNode> GetGreaterThanOrEqualNeighbour(int dir) const;
            std::vector<std::shared_ptr<TerrainNode>> GetSmallerNeighbours(std::shared_ptr<TerrainNode> neighbour, int dir) const;
    };
}