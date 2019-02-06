#pragma once

#include "Body/IPlanet.hpp"
#include "Render/Scatter.hpp"
#include "Render/Drawable.hpp"
#include "Render/IAtmosphereRenderer.hpp"

#include "Render/DirectX/EffectManager.hpp"
#include "Render/DirectX/ConstantBuffer.hpp"

namespace Galactic
{
    struct CloudVertex
    {
        DirectX::SimpleMath::Vector3 position;
    };

    struct CloudBuffer
    {
        DirectX::SimpleMath::Matrix viewProj;      // 64 bytes
        DirectX::SimpleMath::Matrix world;         // 64 bytes
        DirectX::SimpleMath::Vector3 lightDir;     // 12 bytes
        float time;                                // 4  bytes
    };

    class NoiseCloudRenderer : public Drawable<CloudVertex>
    {
        public:
            NoiseCloudRenderer(ID3D11DeviceContext *context, IPlanet *planet);

            void Render(DirectX::SimpleMath::Matrix view, DirectX::SimpleMath::Matrix proj);
            void Update(float dt);
            void Reset();

            DirectX::SimpleMath::Matrix GetMatrix() const { return m_world; }

        private:
            DirectX::SimpleMath::Matrix m_world;

            Microsoft::WRL::ComPtr<ID3D11RasterizerState> m_raster;

            float m_time;

            Effect *m_effect;
            IPlanet *m_planet;

            std::unique_ptr<DirectX::CommonStates> m_states;
            std::unique_ptr<ConstantBuffer<CloudBuffer>> m_buffer;
            std::unique_ptr<ConstantBuffer<ScatterBuffer>> m_buffer2;
    };
}