#pragma once

#include <memory>

#include "Drawable.hpp"

#include "Body/IStar.hpp"
#include "Render/DirectX/Effect.hpp"
#include "Render/DirectX/ConstantBuffer.hpp"
#include "Render/DirectX/EffectManager.hpp"
#include "Render/DirectX/TextureManager.hpp"

#include "SimpleMath.h"
#include "VertexTypes.h"
#include "CommonStates.h"

namespace Galactic
{
    /**
     * @brief Matrix constant buffer to pass to shader
     * 
     */
    struct BillboardBuffer
    {
        DirectX::SimpleMath::Matrix worldViewProj; // 64 bytes
    };

    /**
     * @brief Billboard renderer
     * 
     */
    class Billboard : public Drawable<DirectX::VertexPositionTexture> // TODO: Inherit from IRenderable?
    {
        public:
            enum Blend { Alpha, Additive };

            /**
             * @brief Construct a new Billboard object
             * 
             * @param deviceContext 
             * @param parent attach billboard to an IBody
             * @param offset offset from parent's position
             * @param texture
             * @param blend
             */
            Billboard(ID3D11DeviceContext *deviceContext, IBody *parent, DirectX::SimpleMath::Vector3 offset, std::string texture, Blend blend = Alpha);

            /**
             * @brief Render the billboard
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
             * @brief Set the position
             * 
             * @param pos 
             */
            void SetPosition(DirectX::SimpleMath::Vector3 pos) { m_offset = pos; }

            /**
             * @brief Set the scale
             * 
             * @param scale 
             */
            void SetScale(float scale) { m_scale = scale; }

            /**
             * @brief Get the world matrix
             * 
             * @return DirectX::SimpleMath::Matrix 
             */
            DirectX::SimpleMath::Matrix GetMatrix() const { return m_world; }

        private:
            Microsoft::WRL::ComPtr<ID3D11RasterizerState> m_raster;
            Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_texture;

            DirectX::SimpleMath::Matrix m_world;

            float m_scale;

            DirectX::SimpleMath::Vector3 m_offset;

            Blend m_blend;
            IBody *m_parent;
            Effect *m_effect;

            std::unique_ptr<DirectX::CommonStates> m_states;
            std::unique_ptr<ConstantBuffer<BillboardBuffer>> m_buffer;
    };
}