#pragma once

#include <d3d11.h>

#include "Drawable.hpp"
#include "VertexTypes.h"
#include "IRenderable.hpp"

#include "DirectX/Effect.hpp"
#include "DirectX/ConstantBuffer.hpp"

namespace Galactic
{
    /**
     * @brief Matrix buffer for a cube object
     * 
     */
    struct CubeBuffer
    {
        DirectX::SimpleMath::Matrix worldViewProj; // 64 bytes
    };

    /**
     * @brief Cube renderer
     * 
     */
    class Cube : public IRenderable, public Galactic::Drawable<DirectX::VertexPositionColor>
    {
        public:
            /**
             * @brief Construct a new Cube object
             * 
             * @param context 
             */
            Cube(ID3D11DeviceContext *context);

            void Render(DirectX::SimpleMath::Matrix view, DirectX::SimpleMath::Matrix proj);
            void Update(float dt);
            void Reset();

            /**
             * @brief Set the position
             * 
             * @param position 
             */
            void SetPosition(DirectX::SimpleMath::Vector3 position) { m_position = position; }

            /**
             * @brief Get the world matrix
             * 
             * @return DirectX::SimpleMath::Matrix& 
             */
            DirectX::SimpleMath::Matrix &GetMatrix() { return m_world; }

        private:
            ID3D11DeviceContext *m_context;

            DirectX::SimpleMath::Vector3 m_position;
            DirectX::SimpleMath::Matrix m_world;

            std::unique_ptr<Effect> m_effect;
            std::unique_ptr<DirectX::CommonStates> m_states;
            
            std::unique_ptr<ConstantBuffer<CubeBuffer>> m_buffer;

            Microsoft::WRL::ComPtr<ID3D11RasterizerState> m_raster;
    };
}