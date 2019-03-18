#include "pch.h"
#include "Physics/Constants.hpp"
#include "Render/SphericalQuadTreeTerrain/SphericalQuadTreeTerrain.hpp"
#include "WICTextureLoader.h"

using namespace Galactic;
using namespace DirectX;
using namespace DirectX::SimpleMath;

#ifdef _DEBUG
    size_t SphericalQuadTreeTerrain::GridSize = 5;
#else
    size_t SphericalQuadTreeTerrain::GridSize = 41;
#endif

bool   SphericalQuadTreeTerrain::CancelGeneration = false;
size_t SphericalQuadTreeTerrain::FrameSplits = 0;
size_t SphericalQuadTreeTerrain::MaxSplitsPerFrame = 999;

std::map<SphericalQuadTreeTerrain::EPermutations, std::vector<uint16_t>> SphericalQuadTreeTerrain::Perms;
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
    LoadTextures();
    GeneratePermutations();
}

Galactic::SphericalQuadTreeTerrain::~SphericalQuadTreeTerrain()
{
    m_textures->Release();
    m_normalMaps->Release();
    m_texBiomes->Release();
    m_states.reset();
    m_effect->Reset();
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
    
    const float border[4] = { 0.f, 0.f, 0.f, 0.f };
    unsigned int maxAnisotropy = (m_device->GetFeatureLevel() > D3D_FEATURE_LEVEL_9_1) ? 16U : 2U;

    CD3D11_SAMPLER_DESC samplerDesc(D3D11_FILTER_ANISOTROPIC,
        D3D11_TEXTURE_ADDRESS_WRAP, D3D11_TEXTURE_ADDRESS_WRAP, D3D11_TEXTURE_ADDRESS_CLAMP,
        0.f, maxAnisotropy, D3D11_COMPARISON_NEVER, border, 0.f, FLT_MAX);

    DX::ThrowIfFailed(m_device.Get()->CreateRasterizerState(&rastDesc, m_raster.ReleaseAndGetAddressOf()), "Creating raster");
    DX::ThrowIfFailed(m_device.Get()->CreateRasterizerState(&rastDescWire, m_rasterWire.ReleaseAndGetAddressOf()), "Creating raster");
    DX::ThrowIfFailed(m_device.Get()->CreateSamplerState(&samplerDesc, m_sampler.ReleaseAndGetAddressOf()), "Creating raster");

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
    //auto sampler = m_states->AnisotropicWrap();
    float factor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };

    m_context->PSSetSamplers(0, 1, m_sampler.GetAddressOf());
    m_context->RSSetState(IBody::Wireframe ? m_rasterWire.Get() : m_raster.Get());
    m_context->OMSetBlendState(m_states->Opaque(), factor, 0xFFFFFFFF);
    m_context->OMSetDepthStencilState(m_states->DepthDefault(), 1);

    m_context->IASetInputLayout(m_effect->GetInputLayout());
    m_context->VSSetShader(m_effect->GetVertexShader(), nullptr, 0);
    m_context->PSSetShader(m_effect->GetPixelShader(), nullptr, 0);

    m_context->PSSetShaderResources(0, 1, &m_texBiomes);
    m_context->PSSetShaderResources(1, 1, &m_textures);
    m_context->PSSetShaderResources(2, 1, &m_normalMaps);

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
        { "POSITION",    1,     DXGI_FORMAT_R32G32B32_FLOAT,     0,        12,        D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "NORMAL",      0,     DXGI_FORMAT_R32G32B32_FLOAT,     0,        24,        D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "NORMAL",      1,     DXGI_FORMAT_R32G32B32_FLOAT,     0,        36,        D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "TANGENT",     0,     DXGI_FORMAT_R32G32B32_FLOAT,     0,        48,        D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "NORMAL",      2,     DXGI_FORMAT_R32G32B32_FLOAT,     0,        60,        D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "TEXCOORD",    0,     DXGI_FORMAT_R32G32_FLOAT,        0,        72,        D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "TEXCOORD",    1,     DXGI_FORMAT_R32G32B32_FLOAT,     0,        80,        D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "TEXCOORD",    2,     DXGI_FORMAT_R32_FLOAT,           0,        92,        D3D11_INPUT_PER_VERTEX_DATA, 0 }
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

void SphericalQuadTreeTerrain::LoadTextures()
{
    std::cout << "Reading biomes\n";

    ID3D11Resource *m_tex, *m_ntex;

    DX::ThrowIfFailed(DirectX::CreateDDSTextureFromFileEx(m_device.Get(), m_context.Get(), L"Resources/Biomes.dds", 0, D3D11_USAGE_DEFAULT, D3D11_BIND_SHADER_RESOURCE, 0, D3D11_RESOURCE_MISC_GENERATE_MIPS, 0, &m_tex, &m_textures));
    DX::ThrowIfFailed(DirectX::CreateDDSTextureFromFileEx(m_device.Get(), m_context.Get(), L"Resources/NormalMaps.dds", 0, D3D11_USAGE_DEFAULT, D3D11_BIND_SHADER_RESOURCE, 0, D3D11_RESOURCE_MISC_GENERATE_MIPS, 0, &m_ntex, &m_normalMaps));

    D3D11_TEXTURE2D_DESC desc;
    ((ID3D11Texture2D*)m_tex)->GetDesc(&desc);

    std::cout << desc.Width << "x" << desc.Height << "\n";
    std::cout << desc.ArraySize << " textures, " << desc.MipLevels << " mips\n";

    m_tex->Release();
    m_ntex->Release();
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
    m = (m - 0.2f) / 0.64f;

    float mod = 1.0f;

    float e = m_noiseMaps[0].GetNoise(p.x, p.y, p.z);
    float h = e * settings.NoiseMaps[0].Height;

    for (int i = 1; i < settings.NoiseMaps.size(); ++i)
    {
        mod *= settings.NoiseMaps[i].Mod;
        h += m_noiseMaps[i].GetNoise(p.x, p.y, p.z) * settings.NoiseMaps[i].Height * mod;
    }

    e = 1.0f - ((e + 1.0f) / 2.0f);
    e = (e - 0.2f) / 0.5f;

    texIndex    = settings.Biomes.Sample(m, e);
    biomeLookup = Vector2(m, e);
    height      = h;

#ifdef _DEBUG
    if (e < hmin) hmin = e;
    if (e > hmax) hmax = e;

    counts[texIndex]++;
#endif
}

void SphericalQuadTreeTerrain::GeneratePermutations()
{
    int gridsize = static_cast<int>(GetGridSize());

    typedef std::array<uint16_t, 5> Tri;

    std::vector<Tri> ind;

    auto add_index = [gridsize](std::vector<Tri> &ind, uint16_t x, uint16_t y, int i)
    {
        Tri t1, t2;

        if (i % 2 == 0)
        {
            t1[0] = y * gridsize + x;
            t1[1] = y * gridsize + x + 1;
            t1[2] = (y + 1) * gridsize + x + 1;

            t2[0] = (y + 1) * gridsize + x + 1;
            t2[1] = (y + 1) * gridsize + x;
            t2[2] = y * gridsize + x;
        }
        else
        {
            t1[0] = y * gridsize + x;
            t1[1] = y * gridsize + x + 1;
            t1[2] = (y + 1) * gridsize + x;

            t2[0] = y * gridsize + x + 1;
            t2[1] = (y + 1) * gridsize + x + 1;
            t2[2] = (y + 1) * gridsize + x;
        }

        t1[3] = x;
        t1[4] = y;
        t2[3] = x;
        t2[4] = y;

        ind.push_back(t1);
        ind.push_back(t2);
    };

    auto index_loop = [=](std::vector<Tri> &ind)
    {
        int i = 0;

        for (uint16_t y = 0; y < gridsize - 1; ++y)
        {
            for (uint16_t x = 0; x < gridsize - 1; ++x)
            {
                add_index(ind, x, y, i);
                ++i;
            }

            ++i;
        }
    };

    auto convert = [](std::vector<Tri> ind)
    {
        std::vector<uint16_t> r;

        std::for_each(ind.begin(), ind.end(), [&r](Tri t) {
            r.push_back(t[0]);
            r.push_back(t[1]);
            r.push_back(t[2]);
        });

        return r;
    };

    /*
        None
    */

    ind.clear();
    index_loop(ind);

    Perms[None] = convert(ind);

    /*
        Top
    */

    ind.clear();
    index_loop(ind);

    auto apply_top = [gridsize](std::vector<Tri> &ind) {
        int i = -1;

        ind.erase(std::remove_if(ind.begin(), ind.end(), [&i](const Tri &t) {
            ++i;
            return i % 2 == 0 && t[4] == 0;
        }), ind.end());

        for (uint16_t x = 0; x < gridsize - 2; x += 2)
        {
            Tri t = { x, x + 2, x + gridsize + 1 };
            ind.push_back(t);
        }
    };

    apply_top(ind);
    Perms[Top] = convert(ind);

    /*
        Bottom
    */

    ind.clear();
    index_loop(ind);    

    auto apply_bottom = [gridsize](std::vector<Tri> &ind) {
        int i = -1;

        ind.erase(std::remove_if(ind.begin(), ind.end(), [&i, gridsize](const Tri &t) {
            ++i;
            return i % 2 != 0 && t[4] == gridsize - 2;
        }), ind.end());

        int y0 = (gridsize - 2) * gridsize;
        int y1 = (gridsize - 1) * gridsize;

        for (uint16_t x = 0; x < gridsize - 2; x += 2)
        {
            Tri t = { x + y1, x + y1 + 2, x + y0 + 1 };
            ind.push_back(t);
        }
    };

    apply_bottom(ind);
    Perms[Bottom] = convert(ind);

    /*
        Right
    */

    ind.clear();
    index_loop(ind);
    
    auto apply_right = [gridsize](std::vector<Tri> &ind) {
        std::set<int> list;
        int i = -1;

        for (int z = 1; z < gridsize * 2 - 1; z += 4)
        {
            list.insert(z);
            list.insert(z + 1);
        }

        ind.erase(std::remove_if(ind.begin(), ind.end(), [&i, gridsize, list](const Tri &t) {
            if (t[3] == gridsize - 2)
            {
                ++i;
                return list.find(i) != list.end();
            }

            return false;
        }), ind.end());

        int x = gridsize - 1;

        for (uint16_t y = 0; y < gridsize - 2; y += 2)
        {
            Tri t = { x + y * gridsize, x + (y + 2) * gridsize, (x - 1) + (y + 1) * gridsize };
            ind.push_back(t);
        }
    };

    apply_right(ind);
    Perms[Right] = convert(ind);

    /*
        Left
    */

    ind.clear();
    index_loop(ind);

    auto apply_left = [gridsize](std::vector<Tri> &ind) {
        std::set<int> list;
        int i = -1;

        for (int z = 1; z < gridsize * 2 - 1; z += 4)
        {
            list.insert(z);
            list.insert(z + 1);
        }

        ind.erase(std::remove_if(ind.begin(), ind.end(), [&i, gridsize, list](const Tri &t) {
            if (t[3] == 0)
            {
                ++i;
                return list.find(i) != list.end();
            }

            return false;
        }), ind.end());

        int x = 0;

        for (uint16_t y = 0; y < gridsize - 2; y += 2)
        {
            Tri t = { x + y * gridsize, x + (y + 2) * gridsize, (x + 1) + (y + 1) * gridsize };
            ind.push_back(t);
        }
    };

    apply_left(ind);
    Perms[Left] = convert(ind);

    /*
        Top + Right
    */

    ind.clear();
    index_loop(ind);

    auto apply_topright = [gridsize](std::vector<Tri> &ind) {
        int i = -1, j = -1;
        std::set<int> list;

        for (int z = 1; z < gridsize * 2 - 1; z += 4)
        {
            list.insert(z);
            list.insert(z + 1);
        }

        ind.erase(std::remove_if(ind.begin(), ind.end(), [&i, &j, list, gridsize](const Tri &t) {
            ++j;
            
            if (t[3] == gridsize - 2)
            {
                ++i;

                if (list.find(i) != list.end())
                    return true;
            }
            
            return j % 2 == 0 && t[4] == 0;
        }), ind.end());

        for (uint16_t x = 0; x < gridsize - 2; x += 2)
        {
            Tri t = { x, x + 2, x + gridsize + 1 };
            ind.push_back(t);
        }

        int x = gridsize - 1;

        for (uint16_t y = 0; y < gridsize - 2; y += 2)
        {
            Tri t = { x + y * gridsize, x + (y + 2) * gridsize, (x - 1) + (y + 1) * gridsize };
            ind.push_back(t);
        }
    };

    apply_topright(ind);

    Perms[TopRight] = convert(ind);

    /*
        Right + Bottom
    */

    ind.clear();
    index_loop(ind);

    auto apply_rightbottom = [gridsize](std::vector<Tri> &ind) {
        int i = -1, j = -1;
        std::set<int> list;

        for (int z = 1; z < gridsize * 2 - 1; z += 4)
        {
            list.insert(z);
            list.insert(z + 1);
        }

        ind.erase(std::remove_if(ind.begin(), ind.end(), [&i, &j, list, gridsize](const Tri &t) {
            ++j;

            if (t[3] == gridsize - 2)
            {
                ++i;

                if (list.find(i) != list.end())
                    return true;
            }

            return j % 2 != 0 && t[4] == gridsize - 2;
        }), ind.end());

        int y0 = (gridsize - 2) * gridsize;
        int y1 = (gridsize - 1) * gridsize;

        for (uint16_t x = 0; x < gridsize - 2; x += 2)
        {
            Tri t = { x + y1, x + y1 + 2, x + y0 + 1 };
            ind.push_back(t);
        }

        int x = gridsize - 1;

        for (uint16_t y = 0; y < gridsize - 2; y += 2)
        {
            Tri t = { x + y * gridsize, x + (y + 2) * gridsize, (x - 1) + (y + 1) * gridsize };
            ind.push_back(t);
        }
    };

    apply_rightbottom(ind);

    Perms[RightBottom] = convert(ind);

    /*
        Bottom + Left
    */

    ind.clear();
    index_loop(ind);

    auto apply_bottomleft = [gridsize](std::vector<Tri> &ind) {
        int i = -1, j = -1;
        std::set<int> list;

        for (int z = 1; z < gridsize * 2 - 1; z += 4)
        {
            list.insert(z);
            list.insert(z + 1);
        }

        ind.erase(std::remove_if(ind.begin(), ind.end(), [&i, &j, list, gridsize](const Tri &t) {
            ++j;

            if (t[3] == 0)
            {
                ++i;

                if (list.find(i) != list.end())
                    return true;
            }

            return j % 2 != 0 && t[4] == gridsize - 2;
        }), ind.end());

        int y0 = (gridsize - 2) * gridsize;
        int y1 = (gridsize - 1) * gridsize;

        for (uint16_t x = 0; x < gridsize - 2; x += 2)
        {
            Tri t = { x + y1, x + y1 + 2, x + y0 + 1 };
            ind.push_back(t);
        }

        int x = 0;

        for (uint16_t y = 0; y < gridsize - 2; y += 2)
        {
            Tri t = { x + y * gridsize, x + (y + 2) * gridsize, (x + 1) + (y + 1) * gridsize };
            ind.push_back(t);
        }
    };

    apply_bottomleft(ind);

    Perms[BottomLeft] = convert(ind);

    /*
        Left + Top
    */

    ind.clear();
    index_loop(ind);

    auto apply_lefttop = [gridsize](std::vector<Tri> &ind) {
        int i = -1, j = -1;
        std::set<int> list;

        for (int z = 1; z < gridsize * 2 - 1; z += 4)
        {
            list.insert(z);
            list.insert(z + 1);
        }

        ind.erase(std::remove_if(ind.begin(), ind.end(), [&i, &j, list, gridsize](const Tri &t) {
            ++j;

            if (t[3] == 0)
            {
                ++i;

                if (list.find(i) != list.end())
                    return true;
            }

            return j % 2 == 0 && t[4] == 0;
        }), ind.end());

        for (uint16_t x = 0; x < gridsize - 2; x += 2)
        {
            Tri t = { x, x + 2, x + gridsize + 1 };
            ind.push_back(t);
        }

        int x = 0;

        for (uint16_t y = 0; y < gridsize - 2; y += 2)
        {
            Tri t = { x + y * gridsize, x + (y + 2) * gridsize, (x + 1) + (y + 1) * gridsize };
            ind.push_back(t);
        }
    };

    apply_lefttop(ind);

    Perms[LeftTop] = convert(ind);

    std::cout << "Generated permutations\n";
}