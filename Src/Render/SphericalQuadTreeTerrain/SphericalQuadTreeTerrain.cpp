#include "pch.h"
#include "Physics/Constants.hpp"
#include "Render/SphericalQuadTreeTerrain/SphericalQuadTreeTerrain.hpp"
#include "WICTextureLoader.h"

using namespace Galactic;
using namespace DirectX;
using namespace DirectX::SimpleMath;

#ifdef _DEBUG
    size_t SphericalQuadTreeTerrain::GridSize = 9;
#else
    size_t SphericalQuadTreeTerrain::GridSize = 5;
#endif

bool   SphericalQuadTreeTerrain::CancelGeneration = false;
size_t SphericalQuadTreeTerrain::FrameSplits = 0;
size_t SphericalQuadTreeTerrain::MaxSplitsPerFrame = 999;

std::map<std::string, size_t> counts;
float hmin = 999.0f, hmax = -999.0f;

SphericalQuadTreeTerrain::SphericalQuadTreeTerrain(Microsoft::WRL::ComPtr<ID3D11DeviceContext> deviceContext, IPlanet *planet)
    : m_context(deviceContext),
      m_planet(planet),
      m_world(planet->GetMatrix()),
      m_radius((float)(planet->GetSettings().Radius / Constants::Scale))
{
    m_context->GetDevice(&m_device);

    m_states = std::make_unique<DirectX::CommonStates>(m_device.Get());

    m_gradient.addColorStop(0.0f, Gradient::GradientColor(0.0f * 255.0f, 0.467f * 255.0f, 0.745f * 255.0f, 255.0f));
    m_gradient.addColorStop(0.06f, Gradient::GradientColor(0.93f * 255.0f, 0.79f * 255.0f, 0.69f * 255.0f, 255.0f));
    m_gradient.addColorStop(0.2f, Gradient::GradientColor(0.22f * 255.0f, 0.62f * 255.0f, 0.14f * 255.0f, 255.0f));
    m_gradient.addColorStop(1.0f, Gradient::GradientColor(0.22f * 255.0f, 0.62f * 255.0f, 0.14f * 255.0f, 255.0f));

    CreateEffect();
}

void SphericalQuadTreeTerrain::CreateEffect()
{
    InitEffect();

    CD3D11_RASTERIZER_DESC rastDesc(D3D11_FILL_SOLID, D3D11_CULL_NONE, FALSE,
        D3D11_DEFAULT_DEPTH_BIAS, D3D11_DEFAULT_DEPTH_BIAS_CLAMP,
        D3D11_DEFAULT_SLOPE_SCALED_DEPTH_BIAS, TRUE, FALSE, TRUE, FALSE);

    CD3D11_RASTERIZER_DESC rastDescWire(D3D11_FILL_WIREFRAME, D3D11_CULL_NONE, FALSE,
        D3D11_DEFAULT_DEPTH_BIAS, D3D11_DEFAULT_DEPTH_BIAS_CLAMP,
        D3D11_DEFAULT_SLOPE_SCALED_DEPTH_BIAS, TRUE, FALSE, TRUE, FALSE);

    DX::ThrowIfFailed(m_device.Get()->CreateRasterizerState(&rastDesc, m_raster.ReleaseAndGetAddressOf()), "Creating raster");
    DX::ThrowIfFailed(m_device.Get()->CreateRasterizerState(&rastDescWire, m_rasterWire.ReleaseAndGetAddressOf()), "Creating raster");

    m_buffer = std::make_unique<ConstantBuffer<ScatterBuffer>>(m_device.Get());
}

void SphericalQuadTreeTerrain::Generate()
{
#ifdef _DEBUG
    InitEffect();
#endif
    
    CancelGeneration = false;
    auto &settings = m_planet->GetSettings();

    for (const auto &map : settings.NoiseMaps)
    {
        FastNoise noise;
        noise.SetInterp(FastNoise::Quintic);
        noise.SetNoiseType(FastNoise::SimplexFractal);
        noise.SetFractalOctaves(map.Octaves);
        noise.SetFractalGain(map.Gain);
        noise.SetFractalLacunarity(map.Lacunarity);
        noise.SetFrequency(map.Frequency);
        noise.SetSeed(settings.Seed);

        m_noiseMaps.push_back(noise);
    }

    auto &biomeSettings = settings.BiomeMap;

    m_biomeMap.SetInterp(FastNoise::Quintic);
    m_biomeMap.SetNoiseType(FastNoise::SimplexFractal);
    m_biomeMap.SetFractalOctaves(biomeSettings.Octaves);
    m_biomeMap.SetFractalGain(biomeSettings.Gain);
    m_biomeMap.SetFractalLacunarity(biomeSettings.Lacunarity);
    m_biomeMap.SetFrequency(biomeSettings.Frequency);
    m_biomeMap.SetSeed(settings.Seed + 1);

    auto row1 = BiomeConfig::Row().AddBiome(0.1f, "Scorched")
                                  .AddBiome(0.2f, "Bare")
                                  .AddBiome(0.5f, "Tundra")
                                  .AddBiome(1.0f, "Snowy");

    auto row2 = BiomeConfig::Row().AddBiome(0.3f, "Desert")
                                  .AddBiome(0.6f, "ShrubLand")
                                  .AddBiome(1.0f, "Taiga");

    auto row3 = BiomeConfig::Row().AddBiome(0.2f, "Desert")
                                  .AddBiome(0.5f, "GrassLand")
                                  .AddBiome(0.8f, "Forest")
                                  .AddBiome(1.0f, "TempRainForest");

    auto row4 = BiomeConfig::Row().AddBiome(0.2f, "DryDesert")
                                  .AddBiome(0.3f, "GrassLand")
                                  .AddBiome(0.7f, "TropForest")
                                  .AddBiome(1.0f, "TropRainForest");

    auto row5 = BiomeConfig::Row().AddBiome(1.0f, "Beach");
    auto row6 = BiomeConfig::Row().AddBiome(1.0f, "Ocean");

    settings.Biomes.ClearBiomes();
    settings.Biomes.AddBiomeRow(row1, 0.20f);
    settings.Biomes.AddBiomeRow(row2, 0.40f);
    settings.Biomes.AddBiomeRow(row3, 0.66f);
    settings.Biomes.AddBiomeRow(row4, 0.86f);
    settings.Biomes.AddBiomeRow(row5, 1.00f);

    settings.Biomes.Generate(m_device.Get(), &m_texBiomes, 100, 100);
    //D3DX11CreateShaderResourceViewFromFileA(m_device.Get(), "Resources/biomes.png", NULL, NULL, &m_texBiomes, NULL);

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

#ifdef _DEBUG
    for (auto c : counts)
        std::cout << c.first << ": " << c.second << "\n";

    std::cout << std::endl;
    std::cout << "Min h: " << hmin << "\n";
    std::cout << "Max h: " << hmax << "\n\n";
#endif
}

void SphericalQuadTreeTerrain::SetRenderContext()
{
    auto sampler = m_states->AnisotropicWrap();
    float factor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };

    m_context->PSSetSamplers(0, 1, &sampler);
    m_context->RSSetState(IBody::Wireframe ? m_rasterWire.Get() : m_raster.Get());
    m_context->OMSetBlendState(m_states->Opaque(), factor, 0xFFFFFFFF);
    m_context->OMSetDepthStencilState(m_states->DepthDefault(), 1);

    m_context->IASetInputLayout(m_effect->GetInputLayout());
    m_context->VSSetShader(m_effect->GetVertexShader(), nullptr, 0);
    m_context->PSSetShader(m_effect->GetPixelShader(), nullptr, 0);

    m_context->PSSetShaderResources(0, 1, &m_texBiomes);

    ScatterBuffer buffer = GetScatterBuffer(m_planet);
    m_buffer->SetData(m_context.Get(), buffer);

    m_context->VSSetConstantBuffers(1, 1, m_buffer->GetBuffer());
    m_context->PSSetConstantBuffers(1, 1, m_buffer->GetBuffer());
}

void SphericalQuadTreeTerrain::InitEffect()
{
    D3D11_INPUT_ELEMENT_DESC els[] = {
        // Semantic      Index  Format                           Slot      Offset     Slot Class                   Instance Step
        { "POSITION",    0,     DXGI_FORMAT_R32G32B32_FLOAT,     0,        0,         D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "NORMAL",      0,     DXGI_FORMAT_R32G32B32_FLOAT,     0,        12,        D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "TANGENT",     0,     DXGI_FORMAT_R32G32B32_FLOAT,     0,        24,        D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "NORMAL",      1,     DXGI_FORMAT_R32G32B32_FLOAT,     0,        36,        D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "TEXCOORD",    0,     DXGI_FORMAT_R32G32_FLOAT,        0,        48,        D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "TEXCOORD",    1,     DXGI_FORMAT_R32G32_FLOAT,        0,        56,        D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "TEXCOORD",    2,     DXGI_FORMAT_R32_UINT,            0,        64,        D3D11_INPUT_PER_VERTEX_DATA, 0 },
    };

    unsigned int num = sizeof(els) / sizeof(els[0]);

    std::wstring vs = L"Shaders/PlanetVS.fx";
    std::wstring ps = (m_planet->IsAtmosphereEnabled()) ? L"Shaders/PlanetPS.fx" : L"Shaders/PlanetNoAtmPS.fx";

#ifdef _DEBUG
    m_effect = new Effect(m_device.Get(), vs, ps, els, num, false);
#else
    m_effect = EffectManager::getInstance().GetEffect(m_device.Get(), vs, ps, els, num, false);
#endif
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

std::string SphericalQuadTreeTerrain::GetBiome(const DirectX::SimpleMath::Vector2 &lookup)
{
    return m_planet->GetSettings().Biomes.Sample(lookup.x, lookup.y);
}

void SphericalQuadTreeTerrain::GetHeight(const DirectX::SimpleMath::Vector3 &p, float &height, Vector2 &biomeLookup, std::string &texIndex)
{
    auto &settings = m_planet->GetSettings();
    
    float m = (m_biomeMap.GetNoise(p.x, p.y, p.z) + 1.0f) / 2.0f;
    float mod = 1.0f;

    float e = m_noiseMaps[0].GetNoise(p.x, p.y, p.z);
    float h = e * settings.NoiseMaps[0].Height;

    for (int i = 1; i < settings.NoiseMaps.size(); ++i)
    {
        mod *= settings.NoiseMaps[i].Mod;
        h += m_noiseMaps[i].GetNoise(p.x, p.y, p.z) * settings.NoiseMaps[i].Height * mod;
    }

    e = 1.0f - ((e + 1.0f) / 2.0f);

    if (e < hmin) hmin = e;
    if (e > hmax) hmax = e;

    texIndex    = settings.Biomes.Sample(m, e);
    biomeLookup = Vector2(m, e);
    height      = h;

#ifdef _DEBUG
    counts[texIndex]++;
#endif
}