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
        DirectX::SimpleMath::Vector4 color;
    };

    struct MatrixBuffer
    {
        DirectX::SimpleMath::Matrix worldViewProj; // 64 bytes
    };

    class TerrainNode : public IRenderable, public Drawable<PlanetVertex>
    {
        public:
            TerrainNode(std::shared_ptr<ISphericalTerrain> terrain, std::weak_ptr<TerrainNode> parent, std::weak_ptr<IPlanet> planet);

            void Render(DirectX::SimpleMath::Matrix view, DirectX::SimpleMath::Matrix proj);
            void Update(float dt);
            void Reset();

            DirectX::SimpleMath::Matrix &GetMatrix() { return m_world; }

            void Generate();
            bool IsLeaf() { return m_children[0] == nullptr; }
            void Split();
            void Merge();
            void Release();

        private:
            std::weak_ptr<TerrainNode> m_parent;
            std::weak_ptr<IPlanet> m_planet;
            std::shared_ptr<ISphericalTerrain> m_terrain;
            std::unique_ptr<ConstantBuffer<MatrixBuffer>> m_buffer;

            std::array<std::shared_ptr<TerrainNode>, 4> m_children;

            DirectX::SimpleMath::Matrix m_world;

            Microsoft::WRL::ComPtr<ID3D11InputLayout> m_inputLayout;
    };
}