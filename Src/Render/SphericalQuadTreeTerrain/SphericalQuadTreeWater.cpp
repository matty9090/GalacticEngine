#include "pch.h"
#include "Physics/Constants.hpp"
#include "Render/SphericalQuadTreeTerrain/SphericalQuadTreeWater.hpp"
#include "WICTextureLoader.h"

using namespace Galactic;
using namespace DirectX;
using namespace DirectX::SimpleMath;

#ifdef _DEBUG
    size_t SphericalQuadTreeWater::GridSize = 5;
#else
    size_t SphericalQuadTreeWater::GridSize = 5;
#endif

bool   SphericalQuadTreeWater::CancelGeneration = false;
size_t SphericalQuadTreeWater::FrameSplits = 0;
size_t SphericalQuadTreeWater::MaxSplitsPerFrame = 999;

SphericalQuadTreeWater::SphericalQuadTreeWater(Microsoft::WRL::ComPtr<ID3D11DeviceContext> deviceContext, IPlanet *planet)
    : m_context(deviceContext),
      m_planet(planet),
      m_world(planet->GetMatrix()),
      m_radius((float)(planet->GetSettings().Radius / Constants::Scale)),
      m_scroll(0.0f),
      m_waterLevel(1.0f)
{
    m_context->GetDevice(&m_device);

    m_states = std::make_unique<DirectX::CommonStates>(m_device.Get());

    CreateEffect();

    D3DX11CreateShaderResourceViewFromFileA(m_device.Get(), "Resources/Biomes/water.png", NULL, NULL, &m_texNormalMap, NULL);

    Generate();
}

void SphericalQuadTreeWater::CreateEffect()
{
    InitEffect();

    CD3D11_RASTERIZER_DESC rastDesc(D3D11_FILL_SOLID, D3D11_CULL_BACK, FALSE,
        D3D11_DEFAULT_DEPTH_BIAS, D3D11_DEFAULT_DEPTH_BIAS_CLAMP,
        D3D11_DEFAULT_SLOPE_SCALED_DEPTH_BIAS, TRUE, FALSE, TRUE, FALSE);

    CD3D11_RASTERIZER_DESC rastDescWire(D3D11_FILL_WIREFRAME, D3D11_CULL_BACK, FALSE,
        D3D11_DEFAULT_DEPTH_BIAS, D3D11_DEFAULT_DEPTH_BIAS_CLAMP,
        D3D11_DEFAULT_SLOPE_SCALED_DEPTH_BIAS, TRUE, FALSE, TRUE, FALSE);

    DX::ThrowIfFailed(m_device.Get()->CreateRasterizerState(&rastDesc, m_raster.ReleaseAndGetAddressOf()), "Creating raster");
    DX::ThrowIfFailed(m_device.Get()->CreateRasterizerState(&rastDescWire, m_rasterWire.ReleaseAndGetAddressOf()), "Creating raster");

    m_buffer = std::make_unique<ConstantBuffer<ScatterBuffer>>(m_device.Get());
    m_waterBuffer = std::make_unique<ConstantBuffer<WaterBuffer>>(m_device.Get());
}

void SphericalQuadTreeWater::Generate()
{
#ifdef _DEBUG
    InitEffect();
#endif
    
    CancelGeneration = false;
    //auto &settings = m_planet->GetSettings();

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
        m_faces[i] = std::make_unique<TerrainNode>(this, nullptr, m_planet, Square{ -0.5f, -0.5f, 1.0f }, 0, true);
        m_faces[i]->SetMatrix(orientations[i]);
    }
    
    for (int i = 0; i < 6; ++i) {
        m_faces[i]->Generate();
    }

    for (int i = 0; i < 6; ++i) {
        m_faces[i]->Init();
    }
}

void SphericalQuadTreeWater::SetRenderContext()
{
    auto sampler = m_states->AnisotropicWrap();
    float factor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };

    m_context->PSSetSamplers(0, 1, &sampler);
    m_context->RSSetState(IBody::Wireframe ? m_rasterWire.Get() : m_raster.Get());
    m_context->OMSetBlendState(m_states->AlphaBlend(), factor, 0xFFFFFFFF);
    m_context->OMSetDepthStencilState(m_states->DepthDefault(), 1);

    m_context->IASetInputLayout(m_effect->GetInputLayout());
    m_context->VSSetShader(m_effect->GetVertexShader(), nullptr, 0);
    m_context->PSSetShader(m_effect->GetPixelShader(), nullptr, 0);

    ScatterBuffer buffer = GetScatterBuffer(m_planet);
    WaterBuffer water = { m_scroll };

    m_buffer->SetData(m_context.Get(), buffer);
    m_waterBuffer->SetData(m_context.Get(), water);

    m_context->VSSetConstantBuffers(1, 1, m_buffer->GetBuffer());
    m_context->PSSetConstantBuffers(2, 1, m_waterBuffer->GetBuffer());

    m_context->PSSetShaderResources(0, 1, &m_texNormalMap);
}

void SphericalQuadTreeWater::InitEffect()
{
    D3D11_INPUT_ELEMENT_DESC els[] = {
        // Semantic      Index  Format                           Slot      Offset     Slot Class                   Instance Step
        { "POSITION",    0,     DXGI_FORMAT_R32G32B32_FLOAT,     0,        0,         D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "NORMAL",      0,     DXGI_FORMAT_R32G32B32_FLOAT,     0,        12,        D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "TANGENT",     0,     DXGI_FORMAT_R32G32B32_FLOAT,     0,        24,        D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "NORMAL",      1,     DXGI_FORMAT_R32G32B32_FLOAT,     0,        36,        D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "TEXCOORD",    0,     DXGI_FORMAT_R32G32_FLOAT,        0,        48,        D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "TEXCOORD",    1,     DXGI_FORMAT_R32G32B32_FLOAT,     0,        56,        D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "TEXCOORD",    2,     DXGI_FORMAT_R32_FLOAT,           0,        68,        D3D11_INPUT_PER_VERTEX_DATA, 0 },
    };

    unsigned int num = sizeof(els) / sizeof(els[0]);

    std::wstring vs = L"Shaders/PlanetVS.fx";
    std::wstring ps = (m_planet->IsAtmosphereEnabled()) ? L"Shaders/WaterPS.fx" : L"Shaders/WaterNoAtmPS.fx";

#ifdef _DEBUG
    m_effect = new Effect(m_device.Get(), vs, ps, els, num, false);
#else
    m_effect = EffectManager::getInstance().GetEffect(m_device.Get(), vs, ps, els, num, false);
#endif
}

void SphericalQuadTreeWater::Render(DirectX::SimpleMath::Matrix view, DirectX::SimpleMath::Matrix proj)
{
    SetRenderContext();

    for (auto &face : m_faces)
        face->Render(view, proj);
}

void SphericalQuadTreeWater::Update(float dt)
{
    FrameSplits = 0;
    m_world = m_planet->GetMatrix();
    m_scroll += dt * 10.0f;

    auto scale = DirectX::SimpleMath::Matrix::CreateScale(m_waterLevel);
    m_world *= scale;

    if (!CancelGeneration)
    {
        for (auto &face : m_faces)
            face->Update(dt);
    }
}

void SphericalQuadTreeWater::Reset()
{
    m_texNormalMap->Release();
    m_states.reset();
    m_effect->Reset();

    for (auto &face : m_faces)
        face->Release();
}

std::string Galactic::SphericalQuadTreeWater::GetBiome(const DirectX::SimpleMath::Vector2 &lookup)
{
    return "Ocean";
}

void SphericalQuadTreeWater::GetHeight(const DirectX::SimpleMath::Vector3 &p, float &height, Vector2 &biomeLookup, std::string &texIndex)
{
    texIndex    = "Ocean";
    biomeLookup = Vector2(0.0f, 0.0f);
    height      = 0.0f;
}