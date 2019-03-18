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
     * @brief Scattered atmosphere vertex
     * 
     */
    struct ScatteredAtmosphereVertex
    {
        DirectX::SimpleMath::Vector3 position;
    };

    /**
     * @brief Scattered atmosphere constant buffer
     * 
     */
    struct ScatteredAtmosphereBuffer
    {
        DirectX::SimpleMath::Matrix worldViewProj;
    };

    /**
     * @brief Scattered atmosphere renderer
     * 
     */
    class ScatteredAtmosphere : public IAtmosphereRenderer, public Drawable<ScatteredAtmosphereVertex>
    {
        public:
            /**
             * @brief Construct a new Scattered Atmosphere object
             * 
             * @param context 
             * @param planet 
             */
            ScatteredAtmosphere(ID3D11DeviceContext *context, IPlanet *planet);

            /**
             * @brief Destroy the Scattered Atmosphere object
             * 
             */
            virtual ~ScatteredAtmosphere();

            void Render(DirectX::SimpleMath::Matrix view, DirectX::SimpleMath::Matrix proj);
            void Update(float dt);
            void Reset();

            DirectX::SimpleMath::Matrix GetMatrix() const { return m_world; }

        private:
            ID3D11Device *m_device;

            DirectX::SimpleMath::Matrix m_world;

            Microsoft::WRL::ComPtr<ID3D11RasterizerState> m_raster;

            IPlanet *m_planet;
            Effect *m_effect;

            std::unique_ptr<DirectX::CommonStates> m_states;
            std::unique_ptr<ConstantBuffer<ScatteredAtmosphereBuffer>> m_buffer;
            std::unique_ptr<ConstantBuffer<ScatterBuffer>> m_buffer2;

            void InitEffect();
    };
}