#include "pch.h"
#include "Render/SimpleStarRenderer.hpp"
#include "Physics/Constants.hpp"
#include "Utils/Shapes.hpp"

using namespace Galactic;
using namespace DirectX;
using namespace DirectX::SimpleMath;

SimpleStarRenderer::SimpleStarRenderer(ID3D11DeviceContext *context, IStar *planet)
    : Drawable(context, D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST),
      m_star(planet)
{
    std::vector<VertexPositionTexture> vertices;

    Utils::CreateSphere(1.0f, 40U, vertices, m_indices);

    for (auto &v : vertices)
        m_vertices.push_back(StarVertex { v.position, Colors::LightYellow });

    ID3D11Device *device;
    context->GetDevice(&device);

    D3D11_INPUT_ELEMENT_DESC els[] = {
        // Semantic   Index  Format							 Slot   Offset	Slot Class					 Instance Step
        { "POSITION", 0,	 DXGI_FORMAT_R32G32B32_FLOAT,	 0,		0,		D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "COLOR",    0,	 DXGI_FORMAT_R32G32B32A32_FLOAT, 0,		12,		D3D11_INPUT_PER_VERTEX_DATA, 0 }
    };
    
    unsigned int num = sizeof(els) / sizeof(els[0]);

    m_effect = std::make_unique<Effect>(device, L"Shaders/SimpleVS.fx", L"Shaders/SimplePS.fx", els, num, false);

    CD3D11_RASTERIZER_DESC rastDesc(D3D11_FILL_SOLID, D3D11_CULL_BACK, FALSE,
        D3D11_DEFAULT_DEPTH_BIAS, D3D11_DEFAULT_DEPTH_BIAS_CLAMP,
        D3D11_DEFAULT_SLOPE_SCALED_DEPTH_BIAS, TRUE, FALSE, TRUE, FALSE);

    DX::ThrowIfFailed(device->CreateRasterizerState(&rastDesc, m_raster.ReleaseAndGetAddressOf()));

    m_buffer = std::make_unique<ConstantBuffer<StarBuffer>>(device);
    m_states = std::make_unique<CommonStates>(device);

    Init();
}

void SimpleStarRenderer::Render(DirectX::SimpleMath::Matrix view, DirectX::SimpleMath::Matrix proj)
{
    float factor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };

    auto sampler = m_states->LinearWrap();

    m_context->PSSetSamplers(0, 1, &sampler);
    m_context->RSSetState(m_raster.Get());
    m_context->OMSetDepthStencilState(m_states->DepthDefault(), 0);
    m_context->OMSetBlendState(m_states->Opaque(), factor, 0xFFFFFFFF);

    m_context->IASetInputLayout(m_effect->GetInputLayout());
    m_context->VSSetShader(m_effect->GetVertexShader(), nullptr, 0);
    m_context->PSSetShader(m_effect->GetPixelShader(), nullptr, 0);
    
    PreDraw();

    Matrix worldViewProj = m_star->GetMatrix() * view * proj;
    StarBuffer buffer = { worldViewProj.Transpose() };

    m_buffer->SetData(m_context, buffer);
    m_context->VSSetConstantBuffers(0, 1, m_buffer->GetBuffer());

    Draw();
}

void SimpleStarRenderer::Update(float dt)
{
    dt;
}

void SimpleStarRenderer::Reset()
{
    m_buffer.reset();
    m_effect.reset();
    m_states.reset();
    m_raster.Reset();
}
