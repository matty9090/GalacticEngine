#include "pch.h"
#include "Render/Cube.hpp"

using namespace Galactic;
using namespace DirectX;
using namespace DirectX::SimpleMath;

Cube::Cube(ID3D11DeviceContext *context)
    : Drawable(context, D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP),
      m_context(context),
      m_position(Vector3::Zero),
      m_world(Matrix::Identity)
{
    ID3D11Device *device;
    context->GetDevice(&device);

    m_vertices.push_back(VertexPositionColor { Vector3(-1.0f, -1.0f, -1.0f), Color(1.0f, 1.0f, 1.0f, 1.0f) });
    m_vertices.push_back(VertexPositionColor { Vector3(-1.0f,  1.0f, -1.0f), Color(1.0f, 1.0f, 1.0f, 1.0f) });
    m_vertices.push_back(VertexPositionColor { Vector3( 1.0f, -1.0f, -1.0f), Color(1.0f, 1.0f, 1.0f, 1.0f) });
    m_vertices.push_back(VertexPositionColor { Vector3( 1.0f,  1.0f, -1.0f), Color(1.0f, 1.0f, 1.0f, 1.0f) });
    m_vertices.push_back(VertexPositionColor { Vector3( 1.0f, -1.0f,  1.0f), Color(1.0f, 1.0f, 1.0f, 1.0f) });
    m_vertices.push_back(VertexPositionColor { Vector3( 1.0f,  1.0f,  1.0f), Color(1.0f, 1.0f, 1.0f, 1.0f) });
    m_vertices.push_back(VertexPositionColor { Vector3(-1.0f, -1.0f,  1.0f), Color(1.0f, 1.0f, 1.0f, 1.0f) });
    m_vertices.push_back(VertexPositionColor { Vector3(-1.0f,  1.0f,  1.0f), Color(1.0f, 1.0f, 1.0f, 1.0f) });

    m_indices.push_back(1);
    m_indices.push_back(3);
    m_indices.push_back(0);
    m_indices.push_back(2);
    m_indices.push_back(4);
    m_indices.push_back(3);
    m_indices.push_back(5);
    m_indices.push_back(1);
    m_indices.push_back(7);
    m_indices.push_back(0);
    m_indices.push_back(6);
    m_indices.push_back(4);
    m_indices.push_back(7);
    m_indices.push_back(5);

    D3D11_INPUT_ELEMENT_DESC els[] = {
        // Semantic   Index  Format							 Slot   Offset	Slot Class					 Instance Step
        { "POSITION", 0,	 DXGI_FORMAT_R32G32B32_FLOAT,	 0,		0,		D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "COLOR",	  0,	 DXGI_FORMAT_R32G32B32A32_FLOAT, 0,		24,		D3D11_INPUT_PER_VERTEX_DATA, 0 }
    };

    unsigned int num = sizeof(els) / sizeof(els[0]);

    m_effect = std::make_unique<Effect>(device, L"Shaders/CubeVS.fx", L"Shaders/CubePS.fx", els, num, false);

    CD3D11_RASTERIZER_DESC rastDescWire(D3D11_FILL_SOLID, D3D11_CULL_NONE, FALSE,
        D3D11_DEFAULT_DEPTH_BIAS, D3D11_DEFAULT_DEPTH_BIAS_CLAMP,
        D3D11_DEFAULT_SLOPE_SCALED_DEPTH_BIAS, TRUE, FALSE, TRUE, FALSE);

    DX::ThrowIfFailed(device->CreateRasterizerState(&rastDescWire, m_raster.ReleaseAndGetAddressOf()));

    m_buffer = std::make_unique<ConstantBuffer<CubeBuffer>>(device);
    m_states = std::make_unique<DirectX::CommonStates>(device);

    Init();
}

void Cube::Render(DirectX::SimpleMath::Matrix view, DirectX::SimpleMath::Matrix proj)
{
    float factor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };

    //m_context->PSSetSamplers(0, 1, &sampler);
    //m_context->RSSetState(m_raster.Get());
    //m_context->OMSetBlendState(m_states->AlphaBlend(), factor, 0xFFFFFFFF);
    m_context->OMSetDepthStencilState(m_states->DepthDefault(), 1);

    m_context->IASetInputLayout(m_effect->GetInputLayout());
    m_context->VSSetShader(m_effect->GetVertexShader(), nullptr, 0);
    m_context->PSSetShader(m_effect->GetPixelShader(), nullptr, 0);

    m_world = Matrix::CreateScale(0.01f) * Matrix::CreateTranslation(m_position);

    Matrix worldViewProj = m_world * view * proj;
    CubeBuffer buffer = { worldViewProj.Transpose() };

    m_buffer->SetData(m_context, buffer);
    m_context->VSSetConstantBuffers(0, 1, m_buffer->GetBuffer());

    Draw();
}

void Cube::Update(float dt)
{
    dt;
}

void Cube::Reset()
{
    m_buffer.reset();
    m_effect.reset();
    m_states.reset();
    m_raster.Reset();
}