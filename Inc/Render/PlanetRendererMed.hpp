#pragma once

#include "Body/IPlanet.hpp"
#include "Render/IPlanetRenderer.hpp"

namespace Galactic {
    class MedDetailPlanetRenderer : public IPlanetRenderer
    {
        public:
            MedDetailPlanetRenderer(Microsoft::WRL::ComPtr<ID3D11DeviceContext> device, IPlanet *planet);

            void Render(DirectX::SimpleMath::Matrix view, DirectX::SimpleMath::Matrix proj);
            void Update(float dt);
            void Reset();

            DirectX::SimpleMath::Matrix GetMatrix() const { return m_world; }

        private:
            DirectX::SimpleMath::Matrix m_world;

            IPlanet *m_planet;
            Microsoft::WRL::ComPtr<ID3D11DeviceContext> m_deviceContext;
            std::unique_ptr<DirectX::GeometricPrimitive> m_geometry;
    };
}