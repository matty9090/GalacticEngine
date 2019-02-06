#include "pch.h"
#include "Render/Billboard.hpp"
#include "Physics/Constants.hpp"

using namespace Galactic;
using namespace DirectX;
using namespace DirectX::SimpleMath;

Billboard::Billboard(ID3D11DeviceContext *context, IBody *parent, DirectX::SimpleMath::Vector3 offset, std::string texture, Blend blend)
    : Drawable(context, D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST),
      m_parent(parent),
      m_scale(1.0f),
      m_offset(offset),
      m_blend(blend)
{
    m_vertices.push_back(VertexPositionTexture { Vector3(0.5f, 0.5f, 0.0f), Vector2(0.0f, 0.0f) });
    m_vertices.push_back(VertexPositionTexture { Vector3(-0.5f, 0.5f, 0.0f), Vector2(1.0f, 0.0f) });
    m_vertices.push_back(VertexPositionTexture { Vector3(0.5f, -0.5f, 0.0f), Vector2(0.0f, 1.0f) });
    m_vertices.push_back(VertexPositionTexture { Vector3(-0.5f, -0.5f, 0.0f), Vector2(1.0f, 1.0f) });

    m_indices.push_back(0);
    m_indices.push_back(1);
    m_indices.push_back(2);
    m_indices.push_back(2);
    m_indices.push_back(1);
    m_indices.push_back(3);

    ID3D11Device *device;
    context->GetDevice(&device);

    D3D11_INPUT_ELEMENT_DESC els[] = {
        // Semantic   Index  Format                             Slot   Offset    Slot Class                     Instance Step
        { "POSITION", 0,     DXGI_FORMAT_R32G32B32_FLOAT,     0,        0,        D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "TEXCOORD", 0,     DXGI_FORMAT_R32G32_FLOAT,       0,        12,        D3D11_INPUT_PER_VERTEX_DATA, 0 }
    };

    unsigned int num = sizeof(els) / sizeof(els[0]);

    m_effect = EffectManager::getInstance().GetEffect(device, L"Shaders/BillboardVS.fx", L"Shaders/PositionTexPS.fx", els, num, false);

    CD3D11_RASTERIZER_DESC rastDesc(D3D11_FILL_SOLID, D3D11_CULL_NONE, FALSE,
        D3D11_DEFAULT_DEPTH_BIAS, D3D11_DEFAULT_DEPTH_BIAS_CLAMP,
        D3D11_DEFAULT_SLOPE_SCALED_DEPTH_BIAS, TRUE, FALSE, TRUE, FALSE);

    DX::ThrowIfFailed(device->CreateRasterizerState(&rastDesc, m_raster.ReleaseAndGetAddressOf()), "Creating raster");

    m_buffer = std::make_unique<ConstantBuffer<BillboardBuffer>>(device);
    m_states = std::make_unique<CommonStates>(device);
    
    m_texture = TextureManager::getInstance().GetTexture(device, texture);

    Init();
}

void Billboard::Render(DirectX::SimpleMath::Matrix view, DirectX::SimpleMath::Matrix proj)
{
    float factor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };

    auto sampler = m_states->AnisotropicWrap();
    auto blendmethod = m_states->NonPremultiplied();

    if (m_blend == Additive)
        blendmethod = m_states->Additive();

    m_context->PSSetSamplers(0, 1, &sampler);
    m_context->RSSetState(m_raster.Get());
    m_context->OMSetDepthStencilState(m_states->DepthNone(), 0);
    m_context->OMSetBlendState(blendmethod, factor, 0xFFFFFFFF);

    m_context->IASetInputLayout(m_effect->GetInputLayout());
    m_context->VSSetShader(m_effect->GetVertexShader(), nullptr, 0);
    m_context->PSSetShader(m_effect->GetPixelShader(), nullptr, 0);
    m_context->PSSetShaderResources(0, 1, m_texture.GetAddressOf());

    m_world = Matrix::CreateScale(m_scale);
    //m_world *= Matrix::CreateTranslation(m_parent->GetPosition() + m_offset);
    m_world *= Matrix::CreateBillboard(m_parent->GetPosition() + m_offset, m_parent->GetCameraPos(), Vector3::Up, &Vector3::Forward);

    Matrix worldViewProj = m_world * view * proj;
    BillboardBuffer buffer = { worldViewProj.Transpose() };

    m_buffer->SetData(m_context, buffer);
    m_context->VSSetConstantBuffers(0, 1, m_buffer->GetBuffer());

    Draw();
}

void Billboard::Update(float dt)
{
    dt;
}

void Billboard::Reset()
{
    m_buffer.reset();
    m_states.reset();
    m_raster.Reset();
}
