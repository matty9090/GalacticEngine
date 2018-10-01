#include "pch.h"
#include "Physics/Constants.hpp"
#include "Render/SphericalQuadTreeTerrain/SphericalQuadTreeTerrain.hpp"

using namespace Galactic;
using namespace DirectX;
using namespace DirectX::SimpleMath;

size_t SphericalQuadTreeTerrain::FrameSplits = 0;

SphericalQuadTreeTerrain::SphericalQuadTreeTerrain(Microsoft::WRL::ComPtr<ID3D11DeviceContext> deviceContext, IPlanet *planet)
    : m_deviceContext(deviceContext),
      m_planet(planet),
      m_world(planet->GetMatrix()),
      m_radius((float)(planet->GetRadius() / Constants::Scale))
{
    m_deviceContext->GetDevice(&m_device);

    m_states = std::make_unique<DirectX::CommonStates>(m_device.Get());
    m_noise.SetSeed(rand() % RAND_MAX);

    CreateEffect();
}

void SphericalQuadTreeTerrain::CreateEffect()
{
    D3D11_INPUT_ELEMENT_DESC els[] = {
        // Semantic   Index  Format							 Slot   Offset	Slot Class					 Instance Step
        { "POSITION", 0,	 DXGI_FORMAT_R32G32B32_FLOAT,	 0,		0,		D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "NORMAL",   0,	 DXGI_FORMAT_R32G32B32_FLOAT,	 0,		12,		D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "TANGENT",  0,	 DXGI_FORMAT_R32G32B32_FLOAT,	 0,		24,		D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "COLOR",	  0,	 DXGI_FORMAT_R32G32B32A32_FLOAT, 0,		36,		D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "TEXCOORD", 0,	 DXGI_FORMAT_R32G32_FLOAT,		 0,		52,		D3D11_INPUT_PER_VERTEX_DATA, 0 }
    };

    unsigned int num = sizeof(els) / sizeof(els[0]);

    m_effect = std::make_shared<Effect>(m_device.Get(), L"Shaders/PlanetVS.fx", L"Shaders/PlanetPS.fx", els, num, false);

    CD3D11_RASTERIZER_DESC rastDesc(D3D11_FILL_SOLID, D3D11_CULL_NONE, FALSE,
        D3D11_DEFAULT_DEPTH_BIAS, D3D11_DEFAULT_DEPTH_BIAS_CLAMP,
        D3D11_DEFAULT_SLOPE_SCALED_DEPTH_BIAS, TRUE, FALSE, TRUE, FALSE);

    CD3D11_RASTERIZER_DESC rastDescWire(D3D11_FILL_WIREFRAME, D3D11_CULL_NONE, FALSE,
        D3D11_DEFAULT_DEPTH_BIAS, D3D11_DEFAULT_DEPTH_BIAS_CLAMP,
        D3D11_DEFAULT_SLOPE_SCALED_DEPTH_BIAS, TRUE, FALSE, TRUE, FALSE);

    DX::ThrowIfFailed(m_device.Get()->CreateRasterizerState(&rastDesc, m_raster.ReleaseAndGetAddressOf()));
    DX::ThrowIfFailed(m_device.Get()->CreateRasterizerState(&rastDescWire, m_rasterWire.ReleaseAndGetAddressOf()));

	D3DX11CreateShaderResourceViewFromFileA(m_device.Get(), "Resources/planet_tex.jpg", NULL, NULL, m_texture.ReleaseAndGetAddressOf(), NULL);
	D3DX11CreateShaderResourceViewFromFileA(m_device.Get(), "Resources/rock.jpg", NULL, NULL, m_surface.ReleaseAndGetAddressOf(), NULL);

	m_buffer = std::make_unique<ConstantBuffer<ScatterBuffer>>(m_device.Get());
}

void SphericalQuadTreeTerrain::Generate(float freq, float lacunarity, float gain, int octaves)
{
    m_noise.SetInterp(FastNoise::Quintic);
    m_noise.SetNoiseType(FastNoise::SimplexFractal);
    m_noise.SetFractalOctaves(octaves);
    m_noise.SetFractalGain(gain);
    m_noise.SetFractalLacunarity(lacunarity);
    m_noise.SetFrequency(freq);

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
        m_faces[i] = std::make_shared<TerrainNode>(shared_from_this(), std::weak_ptr<TerrainNode>(), m_planet, Square{ -0.5f, -0.5f, 1.0f }, 0);
        m_faces[i]->SetMatrix(orientations[i]);
        
#ifdef _DEBUG
        m_faces[i]->SetDebugName(std::to_string(i));
#endif
    }
    
    for (int i = 0; i < 6; ++i) {
        m_faces[i]->Generate();
    }

    for (int i = 0; i < 6; ++i) {
        m_faces[i]->Init();
    }
}

void SphericalQuadTreeTerrain::SetRenderContext()
{
    auto sampler = m_states->AnisotropicWrap();
    float factor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };

    m_deviceContext->PSSetSamplers(0, 1, &sampler);
    m_deviceContext->RSSetState(IBody::Wireframe ? m_rasterWire.Get() : m_raster.Get());
    m_deviceContext->OMSetBlendState(m_states->Opaque(), factor, 0xFFFFFFFF);
    m_deviceContext->OMSetDepthStencilState(m_states->DepthDefault(), 1);

    m_deviceContext->IASetInputLayout(m_effect->GetInputLayout());
    m_deviceContext->VSSetShader(m_effect->GetVertexShader(), nullptr, 0);
    m_deviceContext->PSSetShader(m_effect->GetPixelShader(), nullptr, 0);

	m_deviceContext->PSSetShaderResources(0, 1, m_texture.GetAddressOf());
	m_deviceContext->PSSetShaderResources(1, 1, m_surface.GetAddressOf());

	ScatterBuffer buffer = GetScatterBuffer(m_planet);
	m_buffer->SetData(m_deviceContext.Get(), buffer);

	m_deviceContext->VSSetConstantBuffers(1, 1, m_buffer->GetBuffer());
	m_deviceContext->PSSetConstantBuffers(1, 1, m_buffer->GetBuffer());
}

void SphericalQuadTreeTerrain::Render(DirectX::SimpleMath::Matrix view, DirectX::SimpleMath::Matrix proj)
{
    SetRenderContext();

    for (auto &face : m_faces)
        face->Render(view, proj);
}

void SphericalQuadTreeTerrain::Update(float dt)
{
    m_world = m_planet->GetMatrix();

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

float Galactic::SphericalQuadTreeTerrain::GetHeight(DirectX::SimpleMath::Vector3 p)
{
    float scale = m_planet->GetNoiseScale();
    float minvalue = m_planet->GetMinValue();

    float v = m_planet->GetHeight() * m_noise.GetNoise(p.x * 40.0f * scale, p.y * 40.0f * scale, p.z * 40.0f * scale);
    
    return fmaxf(0.0f, v - minvalue);
}
