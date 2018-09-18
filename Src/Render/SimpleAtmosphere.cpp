#include "pch.h"
#include "Render/SimpleAtmosphere.hpp"
#include "Physics/Constants.hpp"

using namespace Galactic;
using namespace DirectX;
using namespace DirectX::SimpleMath;

SimpleAtmosphere::SimpleAtmosphere(ID3D11DeviceContext *context, IPlanet *planet)
    : Drawable(context, D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST),
      m_context(context),
      m_planet(planet)
{
    ID3D11Device *device;
    context->GetDevice(&device);

    CreateSphere(1.0f, 40U);

    D3D11_INPUT_ELEMENT_DESC els[] = {
        // Semantic   Index  Format							 Slot   Offset	Slot Class					 Instance Step
        { "POSITION", 0,	 DXGI_FORMAT_R32G32B32_FLOAT,	 0,		0,		D3D11_INPUT_PER_VERTEX_DATA, 0 },
    };

    unsigned int num = sizeof(els) / sizeof(els[0]);

    m_effect = std::make_unique<Effect>(device, L"Shaders/AtmosphereVS.fx", L"Shaders/AtmospherePS.fx", els, num, false);

    CD3D11_RASTERIZER_DESC rastDesc(D3D11_FILL_SOLID, D3D11_CULL_BACK, FALSE,
        D3D11_DEFAULT_DEPTH_BIAS, D3D11_DEFAULT_DEPTH_BIAS_CLAMP,
        D3D11_DEFAULT_SLOPE_SCALED_DEPTH_BIAS, TRUE, FALSE, TRUE, FALSE);

    DX::ThrowIfFailed(device->CreateRasterizerState(&rastDesc, m_raster.ReleaseAndGetAddressOf()));

    m_buffer = std::make_unique<ConstantBuffer<AtmosphereBuffer>>(device);
    m_states = std::make_unique<CommonStates>(device);

    Init();
}

void Galactic::SimpleAtmosphere::Render(DirectX::SimpleMath::Matrix view, DirectX::SimpleMath::Matrix proj)
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
    
    PreDraw();

    float radius = (float)(m_planet->GetRadius() / Constants::Scale);
    float atmheight = (float)(m_planet->GetAtmosphereHeight() / Constants::Scale);
    float atmradius = atmheight + radius;
    float camHeight = (m_planet->GetCameraPos() - m_planet->GetPosition()).Length();
    
    m_world = m_planet->GetMatrix();

    Matrix worldViewProj = m_world * view * proj;

    AtmosphereBuffer buffer = {
        worldViewProj.Transpose(),
        Vector3::Forward,
        radius,
        m_planet->GetCameraPos() - m_planet->GetPosition(),
        atmradius,
        camHeight
    };

    m_buffer->SetData(m_context, buffer);
    m_context->VSSetConstantBuffers(0, 1, m_buffer->GetBuffer());

    Draw();
}

void Galactic::SimpleAtmosphere::Update(float dt)
{
    dt;
}

void Galactic::SimpleAtmosphere::Reset()
{
    m_buffer.reset();
    m_effect.reset();
    m_states.reset();
    m_raster.Reset();
}

void SimpleAtmosphere::CreateSphere(float radius, size_t tessellation)
{
    m_vertices.clear();
    m_indices.clear();

    if (tessellation < 3)
        throw std::out_of_range("tesselation parameter out of range");

    size_t verticalSegments = tessellation;
    size_t horizontalSegments = tessellation * 2;

    // Create rings of vertices at progressively higher latitudes.
    for (size_t i = 0; i <= verticalSegments; i++)
    {
        float v = 1 - float(i) / verticalSegments;

        float latitude = (i * XM_PI / verticalSegments) - XM_PIDIV2;
        float dy, dxz;

        XMScalarSinCos(&dy, &dxz, latitude);

        // Create a single ring of vertices at this latitude.
        for (size_t j = 0; j <= horizontalSegments; j++)
        {
            float u = float(j) / horizontalSegments;

            float longitude = j * XM_2PI / horizontalSegments;
            float dx, dz;

            XMScalarSinCos(&dx, &dz, longitude);

            dx *= dxz;
            dz *= dxz;

            XMVECTOR normal = XMVectorSet(dx, dy, dz, 0);
            XMVECTOR textureCoordinate = XMVectorSet(u, v, 0, 0);

            m_vertices.push_back(AtmosphereVertex{ XMVectorScale(normal, radius) });
        }
    }

    // Fill the index buffer with triangles joining each pair of latitude rings.
    size_t stride = horizontalSegments + 1;

    for (size_t i = 0; i < verticalSegments; i++)
    {
        for (size_t j = 0; j <= horizontalSegments; j++)
        {
            size_t nextI = i + 1;
            size_t nextJ = (j + 1) % stride;

            m_indices.push_back((uint16_t)(i * stride + nextJ));
            m_indices.push_back((uint16_t)(nextI * stride + j));
            m_indices.push_back((uint16_t)(i * stride + j));

            m_indices.push_back((uint16_t)(nextI * stride + nextJ));
            m_indices.push_back((uint16_t)(nextI * stride + j));
            m_indices.push_back((uint16_t)(i * stride + nextJ));
        }
    }
}
