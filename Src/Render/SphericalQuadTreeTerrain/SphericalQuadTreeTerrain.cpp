#include "pch.h"
#include "Physics/Constants.hpp"
#include "Render/SphericalQuadTreeTerrain/SphericalQuadTreeTerrain.hpp"
#include "WICTextureLoader.h"

#include <wincodecsdk.h>

using namespace Galactic;
using namespace DirectX;
using namespace DirectX::SimpleMath;

#ifdef _DEBUG
    size_t SphericalQuadTreeTerrain::GridSize = 9;
#else
    size_t SphericalQuadTreeTerrain::GridSize = 47;
#endif

bool   SphericalQuadTreeTerrain::CancelGeneration = false;
size_t SphericalQuadTreeTerrain::FrameSplits = 0;
size_t SphericalQuadTreeTerrain::MaxSplitsPerFrame = 6;

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

    GetBiomeTexture();

    CreateEffect();
}

void SphericalQuadTreeTerrain::CreateEffect()
{
    D3D11_INPUT_ELEMENT_DESC els[] = {
        // Semantic   Index  Format                           Slot      Offset     Slot Class                   Instance Step
        { "POSITION",    0,     DXGI_FORMAT_R32G32B32_FLOAT,     0,        0,         D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "NORMAL",      0,     DXGI_FORMAT_R32G32B32_FLOAT,     0,        12,        D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "TANGENT",     0,     DXGI_FORMAT_R32G32B32_FLOAT,     0,        24,        D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "NORMAL",      1,     DXGI_FORMAT_R32G32B32_FLOAT,     0,        36,        D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "TEXCOORD",    0,     DXGI_FORMAT_R32G32_FLOAT,        0,        48,        D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "TEXCOORD",    1,     DXGI_FORMAT_R32G32_FLOAT,        0,        56,        D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "BLENDWEIGHT", 0,     DXGI_FORMAT_R32_FLOAT,           0,        64,        D3D11_INPUT_PER_VERTEX_DATA, 0 },
    };

    unsigned int num = sizeof(els) / sizeof(els[0]);

    m_effect = EffectManager::getInstance().GetEffect(m_device.Get(), L"Shaders/PlanetVS.fx", L"Shaders/PlanetPS.fx", els, num, false);

    CD3D11_RASTERIZER_DESC rastDesc(D3D11_FILL_SOLID, D3D11_CULL_NONE, FALSE,
        D3D11_DEFAULT_DEPTH_BIAS, D3D11_DEFAULT_DEPTH_BIAS_CLAMP,
        D3D11_DEFAULT_SLOPE_SCALED_DEPTH_BIAS, TRUE, FALSE, TRUE, FALSE);

    CD3D11_RASTERIZER_DESC rastDescWire(D3D11_FILL_WIREFRAME, D3D11_CULL_NONE, FALSE,
        D3D11_DEFAULT_DEPTH_BIAS, D3D11_DEFAULT_DEPTH_BIAS_CLAMP,
        D3D11_DEFAULT_SLOPE_SCALED_DEPTH_BIAS, TRUE, FALSE, TRUE, FALSE);

    DX::ThrowIfFailed(m_device.Get()->CreateRasterizerState(&rastDesc, m_raster.ReleaseAndGetAddressOf()), "Creating raster");
    DX::ThrowIfFailed(m_device.Get()->CreateRasterizerState(&rastDescWire, m_rasterWire.ReleaseAndGetAddressOf()), "Creating raster");

    m_textures.resize(2);

    // TODO: Cache resources as this is very slow
    //D3DX11CreateShaderResourceViewFromFileA(m_device.Get(), "Resources/biomes.png", NULL, NULL, &m_texBiomes, NULL);
    //D3DX11CreateShaderResourceViewFromFileA(m_device.Get(), "Resources/grass01d.jpg", NULL, NULL, &m_textures[0], NULL);
    D3DX11CreateShaderResourceViewFromFileA(m_device.Get(), "Resources/grass01n.png", NULL, NULL, &m_textures[0], NULL);
    //D3DX11CreateShaderResourceViewFromFileA(m_device.Get(), "Resources/snow01d.png", NULL, NULL, &m_textures[2], NULL);
    D3DX11CreateShaderResourceViewFromFileA(m_device.Get(), "Resources/snow01n.png", NULL, NULL, &m_textures[1], NULL);

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

    m_snoise = m_noise;
    m_snoise.SetFrequency(m_planet->GetParam(EParams::Frequency) * m_planet->GetParam(EParams::DetailFrequency));

    m_bnoise = m_noise;
    m_bnoise.SetSeed(m_planet->GetSeed() + 1);
    m_bnoise.SetFrequency(m_planet->GetParam(EParams::BiomeFrequency));
    m_bnoise.SetFractalGain(m_planet->GetParam(EParams::BiomeGain));
    m_bnoise.SetFractalLacunarity(m_planet->GetParam(EParams::BiomeLacunarity));

    auto row1 = BiomeConfig::Row().AddBiome(0.2f, { 0.0f, 1.0f, 1.0f })
                                  .AddBiome(1.0f, { 1.0f, 0.0f, 0.0f });

    auto row2 = BiomeConfig::Row().AddBiome(0.6f, { 0.0f, 1.0f, 0.0f })
                                  .AddBiome(1.0f, { 0.0f, 0.0f, 1.0f });

    m_biomeConf.AddBiomeRow(row1, 0.5f);
    m_biomeConf.AddBiomeRow(row2, 1.0f);

    ID3D11Texture2D *tex = NULL;

    m_biomeConf.Generate(m_device.Get(), &tex, &m_texBiomes, 200, 200);

    DX::ThrowIfFailed(DirectX::SaveWICTextureToFile(m_deviceContext.Get(), (ID3D11Resource*)tex, GUID_ContainerFormatPng, L"Resources/b.png"));

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
        
//#ifdef _DEBUG
        m_faces[i]->SetDebugName(std::to_string(i));
//#endif
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
    m_deviceContext->PSSetShaderResources(1, m_textures.size(), &m_textures[0]);

    ScatterBuffer buffer = GetScatterBuffer(m_planet);
    m_buffer->SetData(m_deviceContext.Get(), buffer);

    m_deviceContext->VSSetConstantBuffers(1, 1, m_buffer->GetBuffer());
    m_deviceContext->PSSetConstantBuffers(1, 1, m_buffer->GetBuffer());
}

void SphericalQuadTreeTerrain::GetBiomeTexture()
{
    
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

    for (auto &tex : m_textures)
        tex->Release();
}

void SphericalQuadTreeTerrain::GetHeight(DirectX::SimpleMath::Vector3 p, float &height, Vector2 &biomeLookup, int &texIndex)
{
    float scale = m_planet->GetParam(EParams::NoiseScale);
    float minvalue = m_planet->GetParam(EParams::MinValue);
    float bscale = m_planet->GetParam(EParams::BiomeScale);

    float x = p.x * 40.0f;
    float y = p.y * 40.0f;
    float z = p.z * 40.0f;

    float e = (m_noise.GetNoise(x * scale, y * scale, z * scale) + 1.0f) / 2.0f;
    float s = (m_snoise.GetNoise(x * scale, y * scale, z * scale) + 1.0f) / 2.0f;
    float m = (m_bnoise.GetNoise(x * bscale, y * bscale, z * bscale) + 1.0f) / 2.0f;
    float h = (e + s * m_planet->GetParam(EParams::DetailHeightMod)) * m_planet->GetParam(EParams::Height);

    if (e < 0.53f) texIndex = 0;
    else if (e < 0.51f) texIndex = 1;
    else texIndex = 1;

    /* TODO: Automatic biome resolution */

    /*if (e > 0.8f)
    {
        
    }

    if (e > 0.8f)
    {
        if (m < 0.1f) texIndex = (int)EBiomes::Desert;
        else if (m < 0.2f) texIndex = (int)EBiomes::Mountains;
        else if (m < 0.5f) texIndex = (int)EBiomes::Mountains;
    }*/

    height = h;
    biomeLookup = Vector2(m, 1.0f - e);
}