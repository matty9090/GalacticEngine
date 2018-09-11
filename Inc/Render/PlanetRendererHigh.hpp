#pragma once

#include "Render/IPlanetRenderer.hpp"

namespace Galactic {
    class HighDetailPlanetRenderer : public IPlanetRenderer
    {
        public:
            HighDetailPlanetRenderer(Microsoft::WRL::ComPtr<ID3D11DeviceContext> device);

            void Render(DirectX::SimpleMath::Matrix view, DirectX::SimpleMath::Matrix proj);
            void Update(float dt);
            void Reset();

            DirectX::SimpleMath::Matrix &GetMatrix() { return m_world; }

        private:
            DirectX::SimpleMath::Matrix m_world;

            Microsoft::WRL::ComPtr<ID3D11DeviceContext> m_deviceContext;
            std::unique_ptr<DirectX::GeometricPrimitive> m_geometry;
    };
}