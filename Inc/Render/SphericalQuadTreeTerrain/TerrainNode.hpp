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
            TerrainNode(std::shared_ptr<ISphericalTerrain> terrain, std::weak_ptr<TerrainNode> parent, std::weak_ptr<IPlanet> planet, Square bounds);

            void Render(DirectX::SimpleMath::Matrix view, DirectX::SimpleMath::Matrix proj);
            void Update(float dt);
            void Reset();

            PlanetVertex &GetVertex(int i) { return m_vertices[i]; }
            DirectX::SimpleMath::Matrix &GetMatrix() { return m_world; }

            void Generate();
            bool IsLeaf() { return m_children[0] == nullptr; }
            void Split();
            void Merge();
            void Release();

            bool IsRoot() const { return !m_parent.lock(); }

        private:
            int m_depth;
            bool m_visible;
            float m_scale, m_diameter;
            Square m_bounds;

            std::weak_ptr<TerrainNode> m_parent;
            std::weak_ptr<IPlanet> m_planet;
            std::shared_ptr<ISphericalTerrain> m_terrain;
            std::unique_ptr<ConstantBuffer<MatrixBuffer>> m_buffer;

            std::array<std::shared_ptr<TerrainNode>, 4> m_children;

            DirectX::SimpleMath::Matrix m_world;

            DirectX::SimpleMath::Vector3 CalculateNormal(float x, float y, float step);
            DirectX::SimpleMath::Vector3 PointToSphere(DirectX::SimpleMath::Vector3 point);
    };
}