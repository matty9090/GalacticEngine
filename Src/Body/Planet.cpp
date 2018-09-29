#include "pch.h"
#include "Body/Planet.hpp"
#include "Render/PlanetRendererFactory.hpp"
#include "Physics/Constants.hpp"

using namespace Galactic;
using namespace DirectX;
using namespace DirectX::SimpleMath;

Planet::Planet(Microsoft::WRL::ComPtr<ID3D11DeviceContext> deviceContext, std::string name)
    : m_name(name),
      m_deviceContext(deviceContext),
      m_renderer(nullptr),
      m_atmosphere(nullptr),
      m_isGenerated(false),
      m_freq(0.03f),
      m_gain(0.5f),
      m_octaves(13),
      m_lacunarity(1.9f),
      m_height(0.04f),
      m_noiseScale(1.0f),
      m_minValue(0.0f),
      m_atmosphereHeight(50.0f),
      m_atmColour(Color(0.0f, 0.7f, 1.0f))
{
    m_gradient.addColorStop(0.0f, Gradient::GradientColor(0.0f, 0.467f, 0.745f, 1.0f));
    m_gradient.addColorStop(0.06f, Gradient::GradientColor(0.93f, 0.79f, 0.69f, 1.0f));
    m_gradient.addColorStop(0.2f, Gradient::GradientColor(0.22f, 0.62f, 0.14f, 1.0f));
    m_gradient.addColorStop(1.0f, Gradient::GradientColor(0.22f, 0.62f, 0.14f, 1.0f));
}

Planet::~Planet()
{
    
}

void Planet::Generate(EDetail detail)
{
    Matrix matrix;

    if (m_isGenerated)
    {
        matrix = m_renderer->GetMatrix();

        m_renderer->Reset();
        m_renderer.reset();

        m_atmosphere->Reset();
        m_atmosphere.reset();
    }

    m_renderer = CreatePlanetRenderer(m_deviceContext, this, detail);
    m_atmosphere = CreateAtmosphereRenderer(m_deviceContext, this, detail);

    if (m_isGenerated)
        m_renderer->GetMatrix() = matrix;

    m_isGenerated = true;
}

void Galactic::Planet::SetParameters(float freq, float lacunarity, float gain, float height, int octaves, float noiseScale)
{
    m_freq = freq;
    m_gain = gain;
    m_height = height;
    m_octaves = octaves;
    m_lacunarity = lacunarity;
    m_noiseScale = noiseScale;
}

void Planet::Render(DirectX::SimpleMath::Matrix view, DirectX::SimpleMath::Matrix proj)
{
    if (m_renderer)
        m_renderer->Render(view, proj);

    if (m_atmosphere)
        m_atmosphere->Render(view, proj);
}

void Planet::Update(float dt)
{
    m_position += m_velocity * dt;

    Matrix rotX = Matrix::CreateRotationX(m_rotation.x);
    Matrix rotY = Matrix::CreateRotationY(m_rotation.y);
    Matrix rotZ = Matrix::CreateRotationZ(m_rotation.z);

    Matrix scale = Matrix::CreateScale((float)(m_radius / Constants::Scale));
    Matrix translation = Matrix::CreateTranslation(m_position);

    m_world = scale * rotX * rotY * rotZ * translation;

    if (m_renderer)
        m_renderer->Update(dt);

    if (m_atmosphere)
        m_atmosphere->Update(dt);
}

void Galactic::Planet::Reset()
{
    if (m_renderer)
        m_renderer->Reset();

    m_renderer.reset();
}
