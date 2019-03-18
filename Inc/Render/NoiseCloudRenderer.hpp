#pragma once

#include "Body/IPlanet.hpp"
#include "Render/Scatter.hpp"
#include "Render/Drawable.hpp"
#include "Render/IAtmosphereRenderer.hpp"

#include "Render/DirectX/EffectManager.hpp"
#include "Render/DirectX/ConstantBuffer.hpp"

namespace Galactic
{
    /**
     * @brief Cloud vertex
     * 
     */
    struct CloudVertex
    {
        DirectX::SimpleMath::Vector3 position;
    };

    /**
     * @brief Cloud constant buffer
     * 
     */
    struct CloudBuffer
    {
        DirectX::SimpleMath::Matrix viewProj;      // 64 bytes
        DirectX::SimpleMath::Matrix world;         // 64 bytes
        DirectX::SimpleMath::Vector3 lightDir;     // 12 bytes
        float time;                                // 4  bytes
    };

    /**
     * @brief Noise cloud renderer
     * 
     */
    class NoiseCloudRenderer : public Drawable<CloudVertex> // TODO: Inherit from IRenderable?
    {
        public:
            /**
             * @brief Construct a new Noise Cloud Renderer object
             * 
             * @param context 
             * @param planet 
             */
            NoiseCloudRenderer(ID3D11DeviceContext *context, IPlanet *planet);

            /**
             * @brief Render the clouds
             * 
             * @param view matrix
             * @param proj matrix
             */
            void Render(DirectX::SimpleMath::Matrix view, DirectX::SimpleMath::Matrix proj);

            /**
             * @brief Frame update
             * 
             * @param dt 
             */
            void Update(float dt);

            /**
             * @brief Clean up resources
             * 
             */
            void Reset();

            /**
             * @brief Get the world matrix
             * 
             * @return DirectX::SimpleMath::Matrix 
             */
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