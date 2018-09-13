#include "pch.h"
#include "Physics/Constants.hpp"
#include "Render/SphericalQuadTreeTerrain/SphericalQuadTreeTerrain.hpp"

using namespace Galactic;
using namespace DirectX;
using namespace DirectX::SimpleMath;

SphericalQuadTreeTerrain::SphericalQuadTreeTerrain(Microsoft::WRL::ComPtr<ID3D11DeviceContext> deviceContext, std::weak_ptr<IPlanet> planet)
    : m_deviceContext(deviceContext),
      m_planet(planet),
      m_world(planet.lock()->GetMatrix()),
      m_radius((float)(planet.lock()->GetRadius() / Constants::Scale))
{
    m_deviceContext->GetDevice(&m_device);

    m_states = std::make_unique<DirectX::CommonStates>(m_device.Get());

    CreateEffect();
}

void SphericalQuadTreeTerrain::CreateEffect()
{
    D3D11_INPUT_ELEMENT_DESC els[] = {
        // Semantic   Index  Format							 Slot   Offset	Slot Class					 Instance Step
        { "POSITION", 0,	 DXGI_FORMAT_R32G32B32_FLOAT,	 0,		0,		D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "NORMAL",   0,	 DXGI_FORMAT_R32G32B32_FLOAT,	 0,		12,		D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "COLOR",	  0,	 DXGI_FORMAT_R32G32B32A32_FLOAT, 0,		24,		D3D11_INPUT_PER_VERTEX_DATA, 0 }
        //{ "TANGENT",  0,	 DXGI_FORMAT_R32G32B32_FLOAT,    0,		24,		D3D11_INPUT_PER_VERTEX_DATA, 0 }
    };

    unsigned int num = sizeof(els) / sizeof(els[0]);

    m_effect = std::make_shared<Effect>(m_device.Get(), L"Shaders/BasicVS.fx", L"Shaders/BasicPS.fx", els, num, false);

    CD3D11_RASTERIZER_DESC rastDesc(D3D11_FILL_WIREFRAME, D3D11_CULL_NONE, FALSE,
        D3D11_DEFAULT_DEPTH_BIAS, D3D11_DEFAULT_DEPTH_BIAS_CLAMP,
        D3D11_DEFAULT_SLOPE_SCALED_DEPTH_BIAS, TRUE, FALSE, TRUE, FALSE);

    DX::ThrowIfFailed(m_device.Get()->CreateRasterizerState(&rastDesc, m_raster.ReleaseAndGetAddressOf()));
}

void SphericalQuadTreeTerrain::Generate()
{
    std::array<Matrix, 6> orientations = 
    {
        Matrix::Identity,
        Matrix::CreateRotationX(XM_PI),
        Matrix::CreateRotationX(XM_PI / 2),
        Matrix::CreateRotationX(-XM_PI / 2),
        Matrix::CreateRotationZ(XM_PI / 2),
        Matrix::CreateRotationZ(-XM_PI / 2)
    };

    for (int i = 0; i < 6; ++i)
    {
        m_faces[i] = std::make_shared<TerrainNode>(shared_from_this(), std::weak_ptr<TerrainNode>(), m_planet, Square{ -0.5f, -0.5f, 1.0f });
        m_faces[i]->GetMatrix() = orientations[i];
    }
    
    for (int i = 0; i < 6; ++i) {
        m_faces[i]->Generate();
    }
}

void SphericalQuadTreeTerrain::Render(DirectX::SimpleMath::Matrix view, DirectX::SimpleMath::Matrix proj)
{
    m_world *= Matrix::CreateScale(m_radius);

    auto sampler = m_states->LinearWrap();
    float factor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };

    m_deviceContext->PSSetSamplers(0, 1, &sampler);
    m_deviceContext->RSSetState(m_raster.Get());
    m_deviceContext->OMSetBlendState(m_states->AlphaBlend(), factor, 0xFFFFFFFF);
    m_deviceContext->OMSetDepthStencilState(m_states->DepthDefault(), 1);

    m_deviceContext->IASetInputLayout(m_effect->GetInputLayout());
    m_deviceContext->VSSetShader(m_effect->GetVertexShader(), nullptr, 0);
    m_deviceContext->PSSetShader(m_effect->GetPixelShader(), nullptr, 0);

    for(auto &face : m_faces)
        face->Render(view, proj);
}

void SphericalQuadTreeTerrain::Update(float dt)
{
    for (auto &face : m_faces)
        face->Update(dt);
}

void SphericalQuadTreeTerrain::Reset()
{
    m_states.reset();
    m_effect->Reset();

    for (auto &face : m_faces)
        face->Release();
}
