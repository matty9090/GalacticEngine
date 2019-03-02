#pragma once

#include "Body/IPlanet.hpp"
#include "ISphericalTerrain.hpp"

#include "Render/Billboard.hpp"
#include "Render/Drawable.hpp"
#include "Render/DirectX/ConstantBuffer.hpp"

#include <array>

namespace Galactic
{
    struct PlanetVertex
    {
        DirectX::SimpleMath::Vector3 position;
        DirectX::SimpleMath::Vector3 normal;
        DirectX::SimpleMath::Vector3 tangent;
        DirectX::SimpleMath::Vector2 biome;
        DirectX::SimpleMath::Vector3 uv;
        float normalIndex;
    };

    struct MatrixBuffer
    {
        DirectX::SimpleMath::Matrix worldViewProj; // 64 bytes
        DirectX::SimpleMath::Matrix world;         // 64 bytes
        float lerp;                                // 4  bytes
        float p0, p1, p2;                          // 16 bytes
    };

    struct Square
    {
        float x, y;
        float size;
    };
    
    class TerrainNode : public IRenderable, public Drawable<PlanetVertex>
    {
        public:
            enum EDir { North, East, South, West };
            enum EQuad { NE, NW, SE, SW };

            TerrainNode(ISphericalTerrain *terrain, TerrainNode *parent, IPlanet *planet, Square bounds, int quad, bool simple = false);

            void SetMatrix(DirectX::SimpleMath::Matrix m) { m_world = m; }
            void Render(DirectX::SimpleMath::Matrix view, DirectX::SimpleMath::Matrix proj);
            void Update(float dt);
            void Reset();

            DirectX::SimpleMath::Matrix GetMatrix() const { return m_world; }

            void Generate();
            bool IsLeaf() { return m_children[0] == nullptr; }
            void Split();
            void Merge();
            void Release();

            bool IsRoot() const { return m_parent == nullptr; }
            void SetDebugName(std::string name) { m_dbgName = name; }
            void SetDebugColour(DirectX::SimpleMath::Color col) { m_dbgCol = col; }
            void FixEdges();

            int GetDepth() const { return m_depth; }

            __forceinline PlanetVertex &GetVertex(int i) { return m_vertices[i]; }
            __forceinline std::vector<uint16_t> GetEdge(EDir edge) const { return m_edges[edge]; }
            __forceinline TerrainNode *GetChild(int dir) const { return m_children[dir].get(); }

            static float SplitDistance;

        private:
            int m_depth;
            bool m_visible, m_simple;
            float m_scale, m_diameter;

            int m_quad;
            Square m_bounds;

            IPlanet *m_planet;
            TerrainNode *m_parent;
            ISphericalTerrain *m_terrain;

            ID3D11Device *m_device;

            std::mutex mutex;
            std::unique_ptr<ConstantBuffer<MatrixBuffer>> m_buffer;

            // Debug
            std::string m_dbgName;
            DirectX::SimpleMath::Color m_dbgCol;
            std::unique_ptr<DirectX::CommonStates> m_dbgStates;
            std::unique_ptr<DirectX::BasicEffect> m_dbgEffect;
            std::unique_ptr<DirectX::PrimitiveBatch<DirectX::VertexPositionColor>> m_dbgBatch;
            Microsoft::WRL::ComPtr<ID3D11InputLayout> m_dbgInputLayout;
            
            std::vector<int> m_texIndex;
            std::array<TerrainNode*, 4> m_neighbours;
            std::array<std::vector<uint16_t>, 4> m_edges;
            std::array<std::unique_ptr<TerrainNode>, 4> m_children;

            DirectX::SimpleMath::Matrix m_world;

            DirectX::SimpleMath::Vector3 CalculateNormal(float x, float y, float step);
            DirectX::SimpleMath::Vector3 PointToSphere(DirectX::SimpleMath::Vector3 point);

            void CalculateNormals();
            void NotifyNeighbours();
            
            void FixEdge(EDir dir, TerrainNode *neighbour, std::vector<uint16_t> nEdge, int depth);
            TerrainNode *GetGreaterThanOrEqualNeighbour(int dir) const;
            std::vector<TerrainNode*> GetSmallerNeighbours(TerrainNode *neighbour, int dir) const;
    };
}