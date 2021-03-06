#pragma once

#include "Drawable.hpp"
#include "IStarRenderer.hpp"

#include "Body/IStar.hpp"

#include "Render/Billboard.hpp"
#include "Render/DirectX/Effect.hpp"
#include "Render/DirectX/ConstantBuffer.hpp"

namespace Galactic
{
    /**
     * @brief Star vertex
     * 
     */
    struct StarVertex
    {
        DirectX::SimpleMath::Vector3 position;
        DirectX::SimpleMath::Color color;
    };

    /**
     * @brief Star matrix constant buffer
     * 
     */
    struct StarBuffer
    {
        DirectX::SimpleMath::Matrix worldViewProj; // 64 bytes
    };

    /**
     * @brief Simple star renderer
     * 
     */
    class SimpleStarRenderer : public IStarRenderer, public Drawable<StarVertex>
    {
        public:
            /**
             * @brief Construct a new Simple Star Renderer object
             * 
             * @param deviceContext 
             * @param star 
             */
            SimpleStarRenderer(ID3D11DeviceContext *deviceContext, IStar *star);

            void Render(DirectX::SimpleMath::Matrix view, DirectX::SimpleMath::Matrix proj);
            void Update(float dt);
            void Reset();

            DirectX::SimpleMath::Matrix GetMatrix() const { return m_world; }

        private:
            DirectX::SimpleMath::Matrix m_world;

            Microsoft::WRL::ComPtr<ID3D11RasterizerState> m_raster;

            IStar *m_star;

            std::unique_ptr<Effect> m_effect;
            std::unique_ptr<DirectX::CommonStates> m_states;
            std::unique_ptr<ConstantBuffer<StarBuffer>> m_buffer;
    };
}