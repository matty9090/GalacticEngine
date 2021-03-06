#pragma once

#include "Body/IPlanet.hpp"
#include "Render/Drawable.hpp"
#include "Render/IAtmosphereRenderer.hpp"

#include "Render/DirectX/EffectManager.hpp"
#include "Render/DirectX/ConstantBuffer.hpp"

namespace Galactic
{
    /**
     * @brief Simple atmosphere vertex
     * 
     */
    struct AtmosphereVertex
    {
        DirectX::SimpleMath::Vector3 position;
    };

    /**
     * @brief Simple atmosphere constant buffer
     * 
     */
    struct AtmosphereBuffer
    {
        DirectX::SimpleMath::Matrix viewProj;      // 64 bytes
        DirectX::SimpleMath::Matrix world;         // 64 bytes
        DirectX::SimpleMath::Vector3 lightDir;     // 12 bytes
        float planetRadius;                        // 4 bytes
        DirectX::SimpleMath::Vector3 camPos;       // 12 bytes
        float atmosphereRadius;                    // 4 bytes
        DirectX::SimpleMath::Vector3 atmColour;    // 12 bytes
        float camHeight;                           // 4 bytes
    };

    /**
     * @brief Simple atmosphere renderer
     * 
     */
    class SimpleAtmosphere : public IAtmosphereRenderer, public Drawable<AtmosphereVertex>
    {
        public:
            /**
             * @brief Construct a new Simple Atmosphere object
             * 
             * @param context 
             * @param planet 
             */
            SimpleAtmosphere(ID3D11DeviceContext *context, IPlanet *planet);

            void Render(DirectX::SimpleMath::Matrix view, DirectX::SimpleMath::Matrix proj);
            void Update(float dt);
            void Reset();

            DirectX::SimpleMath::Matrix GetMatrix() const { return m_world; }

        private:
            DirectX::SimpleMath::Matrix m_world;

            Microsoft::WRL::ComPtr<ID3D11RasterizerState> m_raster;

            Effect *m_effect;
            IPlanet *m_planet;

            std::unique_ptr<DirectX::CommonStates> m_states;
            std::unique_ptr<ConstantBuffer<AtmosphereBuffer>> m_buffer;
    };
}