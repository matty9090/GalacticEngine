#pragma once

#include "Body/IPlanet.hpp"
#include "Render/IPlanetRenderer.hpp"
#include "Render/SphericalQuadTreeTerrain/SphericalQuadTreeTerrain.hpp"

namespace Galactic {
    class HighDetailPlanetRenderer : public IPlanetRenderer
    {
        public:
            HighDetailPlanetRenderer(Microsoft::WRL::ComPtr<ID3D11DeviceContext> device, IPlanet *planet);

            void Render(DirectX::SimpleMath::Matrix view, DirectX::SimpleMath::Matrix proj);
            void Update(float dt);
            void Reset();

            DirectX::SimpleMath::Matrix GetMatrix() const { return m_world; }
            DirectX::SimpleMath::Vector3 GetPoint(DirectX::SimpleMath::Vector3 normal);

        private:
            DirectX::SimpleMath::Matrix m_world;

            Microsoft::WRL::ComPtr<ID3D11DeviceContext> m_deviceContext;
            
            std::shared_ptr<SphericalQuadTreeTerrain> m_terrain;
            IPlanet *m_planet;
    };
}