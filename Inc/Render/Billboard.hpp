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
    struct BillboardBuffer
    {
        DirectX::SimpleMath::Matrix worldViewProj; // 64 bytes
    };

    class Billboard : public Drawable<DirectX::VertexPositionTexture>
    {
        public:
            enum Blend { Alpha, Additive };

            Billboard(ID3D11DeviceContext *deviceContext, IBody *parent, DirectX::SimpleMath::Vector3 offset, std::string texture, Blend blend = Alpha);

            void Render(DirectX::SimpleMath::Matrix view, DirectX::SimpleMath::Matrix proj);
            void Update(float dt);
            void Reset();

            void SetPosition(DirectX::SimpleMath::Vector3 pos) { m_offset = pos; }
            void SetScale(float scale) { m_scale = scale; }

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