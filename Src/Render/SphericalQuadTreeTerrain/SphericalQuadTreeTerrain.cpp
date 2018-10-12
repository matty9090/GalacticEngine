#include "pch.h"
#include "Physics/Constants.hpp"
#include "Render/SphericalQuadTreeTerrain/SphericalQuadTreeTerrain.hpp"

using namespace Galactic;
using namespace DirectX;
using namespace DirectX::SimpleMath;

#ifdef _DEBUG
    size_t SphericalQuadTreeTerrain::GridSize = 9;
#else
    size_t SphericalQuadTreeTerrain::GridSize = 33;
#endif

bool   SphericalQuadTreeTerrain::CancelGeneration = false;
size_t SphericalQuadTreeTerrain::FrameSplits = 0;
size_t SphericalQuadTreeTerrain::MaxSplitsPerFrame = 2;

SphericalQuadTreeTerrain::SphericalQuadTreeTerrain(Microsoft::WRL::ComPtr<ID3D11DeviceContext> deviceContext, IPlanet *planet)
    : m_deviceContext(deviceContext),
      m_planet(planet),
      m_world(planet->GetMatrix()),
      m_radius((float)(planet->GetRadius() / Constants::Scale))
{
    m_deviceContext->GetDevice(&m_device);

    m_states = std::make_unique<DirectX::CommonStates>(m_device.Get());

    m_gradient.addColorStop(0.0f, Gradient::GradientColor(0.0f * 255.0f, 0.467f * 255.0f, 0.745f * 255.0f, 255.0f));
    m_gradient.addColorStop(0.06f, Gradient::GradientColor(0.93f * 255.0f, 0.79f * 255.0f, 0.69f * 255.0f, 255.0f));
    m_gradient.addColorStop(0.2f, Gradient::GradientColor(0.22f * 255.0f, 0.62f * 255.0f, 0.14f * 255.0f, 255.0f));
    m_gradient.addColorStop(1.0f, Gradient::GradientColor(0.22f * 255.0f, 0.62f * 255.0f, 0.14f * 255.0f, 255.0f));

    CreateEffect();
}

void SphericalQuadTreeTerrain::CreateEffect()
{
    D3D11_INPUT_ELEMENT_DESC els[] = {
        // Semantic   Index  Format                           Slot      Offset     Slot Class                   Instance Step
        { "POSITION", 0,     DXGI_FORMAT_R32G32B32_FLOAT,     0,        0,         D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "NORMAL",   0,     DXGI_FORMAT_R32G32B32_FLOAT,     0,        12,        D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "TANGENT",  0,     DXGI_FORMAT_R32G32B32_FLOAT,     0,        24,        D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "COLOR",    0,     DXGI_FORMAT_R32G32B32A32_FLOAT,  0,        36,        D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "TEXCOORD", 0,     DXGI_FORMAT_R32G32_FLOAT,        0,        52,        D3D11_INPUT_PER_VERTEX_DATA, 0 }
    };

    unsigned int num = sizeof(els) / sizeof(els[0]);

    m_effect = EffectManager::getInstance().GetEffect(m_device.Get(), L"Shaders/PlanetVS.fx", L"Shaders/PlanetPS.fx", els, num, false);

    CD3D11_RASTERIZER_DESC rastDesc(D3D11_FILL_SOLID, D3D11_CULL_NONE, FALSE,
        D3D11_DEFAULT_DEPTH_BIAS, D3D11_DEFAULT_DEPTH_BIAS_CLAMP,
        D3D11_DEFAULT_SLOPE_SCALED_DEPTH_BIAS, TRUE, FALSE, TRUE, FALSE);

    CD3D11_RASTERIZER_DESC rastDescWire(D3D11_FILL_WIREFRAME, D3D11_CULL_NONE, FALSE,
        D3D11_DEFAULT_DEPTH_BIAS, D3D11_DEFAULT_DEPTH_BIAS_CLAMP,
        D3D11_DEFAULT_SLOPE_SCALED_DEPTH_BIAS, TRUE, FALSE, TRUE, FALSE);

    DX::ThrowIfFailed(m_device.Get()->CreateRasterizerState(&rastDesc, m_raster.ReleaseAndGetAddressOf()));
    DX::ThrowIfFailed(m_device.Get()->CreateRasterizerState(&rastDescWire, m_rasterWire.ReleaseAndGetAddressOf()));

    // TODO: Cache resources as this is very slow
    D3DX11CreateShaderResourceViewFromFileA(m_device.Get(), "Resources/biomes.png", NULL, NULL, &m_texBiomes, NULL);
    //D3DX11CreateShaderResourceViewFromFileA(m_device.Get(), "Resources/rock.jpg", NULL, NULL, m_surface.ReleaseAndGetAddressOf(), NULL);

    D3D11_RECT rect;
    

    m_buffer = std::make_unique<ConstantBuffer<ScatterBuffer>>(m_device.Get());
}

void SphericalQuadTreeTerrain::Generate()
{
    CancelGeneration = false;

    m_noise.SetInterp(FastNoise::Quintic);
    m_noise.SetNoiseType(FastNoise::SimplexFractal);
    m_noise.SetFractalOctaves((int)m_planet->GetParam(EParams::Octaves));
    m_noise.SetFractalGain(m_planet->GetParam(EParams::Gain));
    m_noise.SetFractalLacunarity(m_planet->GetParam(EParams::Lacunarity));
    m_noise.SetFrequency(m_planet->GetParam(EParams::Frequency));
    m_noise.SetSeed(m_planet->GetSeed());

    m_bnoise = m_noise;
    m_bnoise.SetSeed(m_planet->GetSeed() + 1);
    
    m_biomes[EBiomes::Grass] = std::make_unique<Biome>(m_noise, Biomes[EBiomes::Grass]);
    m_biomes[EBiomes::Mountains] = std::make_unique<Biome>(m_noise, Biomes[EBiomes::Mountains]);
    m_biomes[EBiomes::Desert] = std::make_unique<Biome>(m_noise, Biomes[EBiomes::Desert]);
    m_biomes[EBiomes::Ocean] = std::make_unique<OceanBiome>(m_noise, Biomes[EBiomes::Ocean]);

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
        m_faces[i] = std::make_unique<TerrainNode>(this, nullptr, m_planet, Square{ -0.5f, -0.5f, 1.0f }, 0);
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

    m_deviceContext->PSSetShaderResources(0, 1, &m_texBiomes);
    //m_deviceContext->PSSetShaderResources(1, 1, m_surface.GetAddressOf());

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
    FrameSplits = 0;
    m_world = m_planet->GetMatrix();

    if (!CancelGeneration)
    {
        for (auto &face : m_faces)
            face->Update(dt);
    }
}

void SphericalQuadTreeTerrain::Reset()
{
    m_texBiomes->Release();
    m_states.reset();
    m_effect->Reset();

    for (auto &face : m_faces)
        face->Release();
}

void SphericalQuadTreeTerrain::GetHeight(DirectX::SimpleMath::Vector3 p, float &height, Vector2 &biomeLookup)
{
   // bool biomes_enabled = (bool)m_planet->GetParam(EParams::Biomes);
    float scale = m_planet->GetParam(EParams::NoiseScale);
    float minvalue = m_planet->GetParam(EParams::MinValue);

    float x = p.x * 40.0f * scale;
    float y = p.y * 40.0f * scale;
    float z = p.z * 40.0f * scale;

    float e = m_noise.GetNoise(x, y, z);
    float m = (m_bnoise.GetNoise(x * 2.0f, y * 2.0f, z * 2.0f) + 1.0f) / 2.0f;
    float h = e * m_planet->GetParam(EParams::Height);

    EBiomes biome = EBiomes::Grass;

    if (e < 0.2f) biome = EBiomes::Ocean;
    else if (e < 0.22f) biome = EBiomes::Beach;
    
    if (e > 0.8f)
    {
        if (m < 0.1f) biome = EBiomes::Desert;
    }

    height = h;
    biomeLookup = Vector2(e, 1.0f - m);
}
