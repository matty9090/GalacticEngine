#pragma once

#include "Body/IPlanet.hpp"
#include "ISphericalTerrain.hpp"

#include "Render/Billboard.hpp"
#include "Render/Drawable.hpp"
#include "Render/DirectX/ConstantBuffer.hpp"

#include <array>

namespace Galactic
{
    /**
     * @brief Planet terrain vertex
     * 
     */
    struct PlanetVertex
    {
        DirectX::SimpleMath::Vector3 position1;
        DirectX::SimpleMath::Vector3 position2;
        DirectX::SimpleMath::Vector3 normal1;
        DirectX::SimpleMath::Vector3 normal2;
        DirectX::SimpleMath::Vector3 tangent;
        DirectX::SimpleMath::Vector3 sphere;
        DirectX::SimpleMath::Vector2 biome;
        DirectX::SimpleMath::Vector3 uv;
        float normalIndex;
        bool calcNormal;
    };

    /**
     * @brief Constant buffer to send matrices to shader
     * 
     */
    struct MatrixBuffer
    {
        DirectX::SimpleMath::Matrix worldViewProj; // 64 bytes
        DirectX::SimpleMath::Matrix world;         // 64 bytes
        DirectX::SimpleMath::Vector3 cam;          // 12 bytes
        float lerp;                                // 4  bytes
        float morph;                               // 4  bytes
        float p1, p2, p3;                          // 12 bytes
    };

    /**
     * @brief Square
     * 
     */
    struct Square
    {
        float x, y;
        float size;
    };
    
    /**
     * @brief Quadtree node
     * 
     */
    class TerrainNode : public IRenderable, public Drawable<PlanetVertex>
    {
        public:
            enum EDir { North, East, South, West };
            enum EQuad { NE, NW, SE, SW };

            TerrainNode(ISphericalTerrain *terrain, TerrainNode *parent, IPlanet *planet, Square bounds, int quad, bool simple = false);

            /**
             * @brief Set the world matrix
             * 
             * @param m 
             */
            void SetMatrix(DirectX::SimpleMath::Matrix m) { m_world = m; }
            void Render(DirectX::SimpleMath::Matrix view, DirectX::SimpleMath::Matrix proj);
            void Update(float dt);
            void Reset();

            DirectX::SimpleMath::Matrix GetMatrix() const { return m_world; }

            /**
             * @brief Generate the geometry
             * 
             */
            void Generate();

            /**
             * @brief Is this node a leaf node (aka has no children)
             * 
             * @return is leaf
             */
            bool IsLeaf() { return m_children[0] == nullptr; }

            /**
             * @brief Split the node into 4 nodes
             * 
             */
            void Split();

            /**
             * @brief Merge the child nodes back into 1 node
             * 
             */
            void Merge();

            /**
             * @brief Clean up resources
             * 
             */
            void Release();

            /**
             * @brief Is this node the root node (aka has no parent)
             * 
             * @return true 
             * @return false 
             */
            bool IsRoot() const { return m_parent == nullptr; }

            /**
             * @brief Set the debug identifier
             * 
             * @param name 
             */
            void SetDebugName(std::string name) { m_dbgName = name; }

            /**
             * @brief Set the debug colour
             * 
             * @param col 
             */
            void SetDebugColour(DirectX::SimpleMath::Color col) { m_dbgCol = col; }

            /**
             * @brief Find neighbour nodes and make sure the edges line up without cracks due to LOD difference
             * 
             */
            void FixEdges();

            /**
             * @brief Get the depth of the node
             * 
             * @return int 
             */
            int GetDepth() const { return m_depth; }

            /**
             * @brief Get a vertex
             * 
             * @param i 
             * @return PlanetVertex& 
             */
            __forceinline PlanetVertex &GetVertex(int i) { return m_vertices[i]; }

            /**
             * @brief Get a specific edges indices
             * 
             * @param edge 
             * @return std::vector<uint16_t>
             */
            __forceinline std::vector<uint16_t> GetEdge(EDir edge) const { return m_edges[edge]; }

            /**
             * @brief Get a specific child node
             * 
             * @param dir 
             * @return TerrainNode*
             */
            __forceinline TerrainNode *GetChild(int dir) const { return m_children[dir].get(); }

            /// Split distance from camera to terrain node
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

            // TODO: Remove
            DirectX::SimpleMath::Vector3 CalculateNormal(float x, float y, float step);

            // TODO: Remove
            DirectX::SimpleMath::Vector3 PointToSphere(DirectX::SimpleMath::Vector3 point);

            /**
             * @brief Calculate normals
             * 
             */
            void CalculateNormals();

            /**
             * @brief Tell neighbours that this node's LOD has changed
             * 
             */
            void NotifyNeighbours();
            
            // TODO: Remove
            void FixEdge(EDir dir, TerrainNode *neighbour, std::vector<uint16_t> nEdge, int depth);

            /**
             * @brief Get the all Greater than or qqual depth Neighbours
             * 
             * @param dir 
             * @return TerrainNode* 
             */
            TerrainNode *GetGreaterThanOrEqualNeighbour(int dir) const;

            /**
             * @brief Get the smaller depth neighbours
             * 
             * @param neighbour 
             * @param dir 
             * @return std::vector<TerrainNode*> 
             */
            std::vector<TerrainNode*> GetSmallerNeighbours(TerrainNode *neighbour, int dir) const;
    };
}