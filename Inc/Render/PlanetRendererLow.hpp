#pragma once

#include "Render/IRenderable.hpp"

namespace Galactic {
    class LowDetailPlanetRenderer : public IRenderable
    {
    public:
        LowDetailPlanetRenderer(Microsoft::WRL::ComPtr<ID3D11DeviceContext> device);

        void Render(DirectX::SimpleMath::Matrix view, DirectX::SimpleMath::Matrix proj);
        void Update(float dt);
        void Reset();

    private:
        DirectX::SimpleMath::Matrix m_world;

        Microsoft::WRL::ComPtr<ID3D11DeviceContext> m_deviceContext;
        std::unique_ptr<DirectX::GeometricPrimitive> m_geometry;
    };
}