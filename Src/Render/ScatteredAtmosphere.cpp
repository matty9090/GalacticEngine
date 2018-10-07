#include "pch.h"
#include "Render/ScatteredAtmosphere.hpp"
#include "Physics/Constants.hpp"
#include "Utils/Shapes.hpp"

using namespace Galactic;
using namespace DirectX;
using namespace DirectX::SimpleMath;

ScatteredAtmosphere::ScatteredAtmosphere(ID3D11DeviceContext *context, IPlanet *planet)
    : Drawable(context, D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST),
      m_planet(planet)
{
    std::vector<VertexPositionTexture> vertices;

    Utils::CreateSphere(1.0f, 120U, vertices, m_indices);

    for (auto &v : vertices)
        m_vertices.push_back(ScatteredAtmosphereVertex{ v.position });

    ID3D11Device *device;
    context->GetDevice(&device);

    D3D11_INPUT_ELEMENT_DESC els[] = {
        // Semantic   Index  Format							 Slot   Offset	Slot Class					 Instance Step
        { "POSITION", 0,	 DXGI_FORMAT_R32G32B32_FLOAT,	 0,		0,		D3D11_INPUT_PER_VERTEX_DATA, 0 },
    };

    unsigned int num = sizeof(els) / sizeof(els[0]);

    m_effect = EffectManager::getInstance().GetEffect(device, L"Shaders/ScatteredAtmosphereVS.fx", L"Shaders/ScatteredAtmospherePS.fx", els, num, false);

    CD3D11_RASTERIZER_DESC rastDesc(D3D11_FILL_SOLID, D3D11_CULL_BACK, FALSE,
        D3D11_DEFAULT_DEPTH_BIAS, D3D11_DEFAULT_DEPTH_BIAS_CLAMP,
        D3D11_DEFAULT_SLOPE_SCALED_DEPTH_BIAS, TRUE, FALSE, TRUE, FALSE);

    DX::ThrowIfFailed(device->CreateRasterizerState(&rastDesc, m_raster.ReleaseAndGetAddressOf()));

    m_buffer  = std::make_unique<ConstantBuffer<ScatteredAtmosphereBuffer>>(device);
    m_buffer2 = std::make_unique<ConstantBuffer<ScatterBuffer>>(device);

    m_states  = std::make_unique<CommonStates>(device);

    Init();
}

void ScatteredAtmosphere::Render(DirectX::SimpleMath::Matrix view, DirectX::SimpleMath::Matrix proj)
{
    float factor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };

    auto sampler = m_states->LinearWrap();

    m_context->PSSetSamplers(0, 1, &sampler);
    m_context->RSSetState(m_raster.Get());
    m_context->OMSetDepthStencilState(m_states->DepthDefault(), 0);
    m_context->OMSetBlendState(m_states->NonPremultiplied(), factor, 0xFFFFFFFF);

    m_context->IASetInputLayout(m_effect->GetInputLayout());
    m_context->VSSetShader(m_effect->GetVertexShader(), nullptr, 0);
    m_context->PSSetShader(m_effect->GetPixelShader(), nullptr, 0);

	float radius = (float)(m_planet->GetRadius() / Constants::Scale);
	float atmheight = (float)(m_planet->GetAtmosphereHeight() / Constants::Scale);
	float atmradius = atmheight + radius;

    m_world = Matrix::CreateScale(atmradius) * Matrix::CreateTranslation(m_planet->GetPosition());

    Matrix worldViewProj =  m_world * view * proj;

	ScatteredAtmosphereBuffer buffer = {
		worldViewProj.Transpose()
    };

	ScatterBuffer buffer2 = GetScatterBuffer(m_planet);

    m_buffer->SetData(m_context, buffer);
    m_buffer2->SetData(m_context, buffer2);

    m_context->VSSetConstantBuffers(0, 1, m_buffer->GetBuffer());
    m_context->VSSetConstantBuffers(1, 1, m_buffer2->GetBuffer());
    m_context->PSSetConstantBuffers(0, 1, m_buffer->GetBuffer());
    m_context->PSSetConstantBuffers(1, 1, m_buffer2->GetBuffer());

    Draw();
}

void ScatteredAtmosphere::Update(float dt)
{
    dt;
}

void ScatteredAtmosphere::Reset()
{
    m_buffer.reset();
    m_states.reset();
    m_raster.Reset();
}
