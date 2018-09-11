#pragma once

#include "Body/IPlanet.hpp"
#include "TerrainNode.hpp"
#include "ISphericalTerrain.hpp"

namespace Galactic
{
    class SphericalQuadTreeTerrain : public ISphericalTerrain, public std::enable_shared_from_this<SphericalQuadTreeTerrain>
    {
        public:
            SphericalQuadTreeTerrain(Microsoft::WRL::ComPtr<ID3D11DeviceContext> deviceContext, std::weak_ptr<IPlanet> planet);

            void Generate();
            void Render(DirectX::SimpleMath::Matrix view, DirectX::SimpleMath::Matrix proj);
            void Update(float dt);
            void Reset();

            int GetGridSize() const { return 16; }
            float GetRadius() const { return m_radius; }
            DirectX::SimpleMath::Matrix &GetMatrix() { return m_world; }

        private:
            DirectX::SimpleMath::Matrix &m_world;

            std::weak_ptr<IPlanet> m_planet;
            std::shared_ptr<TerrainNode> m_root;

            Microsoft::WRL::ComPtr<ID3D11DeviceContext> m_deviceContext;

            float m_radius;
    };
}