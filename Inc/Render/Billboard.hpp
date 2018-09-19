#pragma once

#include <memory>

#include "Drawable.hpp"

#include "Body/IBody.hpp"
#include "Render/DirectX/Effect.hpp"
#include "Render/DirectX/ConstantBuffer.hpp"

#include "SimpleMath.h"
#include "VertexTypes.h"
#include "CommonStates.h"

namespace Galactic
{
    struct BillboardBuffer
    {
        DirectX::SimpleMath::Matrix worldViewProj; // 64 bytes
    };

    class Billboard : public Drawable<DirectX::VertexPositionTexture>
    {
        public:
            Billboard(ID3D11DeviceContext *deviceContext, IBody *parent, std::string texture);

            void Render(DirectX::SimpleMath::Matrix view, DirectX::SimpleMath::Matrix proj);
            void Update(float dt);
            void Reset();

            DirectX::SimpleMath::Matrix GetMatrix() const { return m_world; }

        private:
            Microsoft::WRL::ComPtr<ID3D11RasterizerState> m_raster;
            Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_texture;

            DirectX::SimpleMath::Matrix m_world;

            IBody *m_parent;
            std::unique_ptr<Effect> m_effect;
            std::unique_ptr<DirectX::CommonStates> m_states;
            std::unique_ptr<ConstantBuffer<BillboardBuffer>> m_buffer;
    };
}