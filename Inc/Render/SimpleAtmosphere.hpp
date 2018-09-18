#pragma once

#include "Body/IPlanet.hpp"
#include "Render/Drawable.hpp"
#include "Render/IAtmosphereRenderer.hpp"

#include "Render/DirectX/Effect.hpp"
#include "Render/DirectX/ConstantBuffer.hpp"

namespace Galactic
{
    struct AtmosphereVertex
    {
        DirectX::SimpleMath::Vector3 position;
    };

    struct AtmosphereBuffer
    {
        DirectX::SimpleMath::Matrix worldViewProj; // 64 bytes
        DirectX::SimpleMath::Vector3 lightDir;     // 12 bytes
        float planetRadius;                        // 4 bytes
        DirectX::SimpleMath::Vector3 camPos;       // 12 bytes
        float atmosphereRadius;                    // 4 bytes
        float camHeight;                           // 4 bytes
        DirectX::SimpleMath::Vector3 _unused;      // 12 bytes
    };

    class SimpleAtmosphere : public IAtmosphereRenderer, public Drawable<AtmosphereVertex>
    {
        public:
            SimpleAtmosphere(ID3D11DeviceContext *context, IPlanet *planet);

            void Render(DirectX::SimpleMath::Matrix view, DirectX::SimpleMath::Matrix proj);
            void Update(float dt);
            void Reset();

            DirectX::SimpleMath::Matrix GetMatrix() const { return m_world; }

        private:
            DirectX::SimpleMath::Matrix m_world;

            ID3D11DeviceContext *m_context;
            Microsoft::WRL::ComPtr<ID3D11RasterizerState> m_raster;

            IPlanet *m_planet;
            std::unique_ptr<Effect> m_effect;
            std::unique_ptr<DirectX::CommonStates> m_states;
            std::unique_ptr<ConstantBuffer<AtmosphereBuffer>> m_buffer;

            void CreateSphere(float radius, size_t tessellation);
    };
}