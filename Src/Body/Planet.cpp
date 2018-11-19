#include "pch.h"
#include "Body/Planet.hpp"
#include "Render/PlanetRendererFactory.hpp"
#include "Physics/Constants.hpp"

#include <fstream>

using namespace Galactic;
using namespace DirectX;
using namespace DirectX::SimpleMath;

Planet::Planet(Microsoft::WRL::ComPtr<ID3D11DeviceContext> deviceContext, std::string name)
    : m_name(name),
      m_deviceContext(deviceContext),
      m_renderer(nullptr),
      m_atmosphere(nullptr),
      m_isGenerated(false),
      m_atmosphereHeight(200.0f),
      m_atmColour(Color(0.0f, 0.7f, 1.0f)),
      m_vertexCount(0)
{
    m_params.resize(20);
    SetParam(EParams::Biomes, 0.0f);
}

Planet::~Planet()
{
    
}

void Planet::Generate(EDetail detail)
{
    m_vertexCount = 0;

    Matrix matrix;

    if (m_isGenerated)
    {
        matrix = m_renderer->GetMatrix();

        m_renderer->Reset();
        m_renderer.reset();
        
        m_atmosphere->Reset();
        m_atmosphere.reset();

        m_clouds->Reset();
        m_clouds.reset();

        m_grass->Reset();
        m_grass.reset();
    }

    m_grass = std::make_unique<GrassRenderer>(m_deviceContext.Get(), this);
    m_renderer = CreatePlanetRenderer(m_deviceContext, this, detail);
    m_atmosphere = CreateAtmosphereRenderer(m_deviceContext, this, detail);
    m_clouds = std::make_unique<NoiseCloudRenderer>(m_deviceContext.Get(), this);

    if (m_isGenerated)
        m_renderer->GetMatrix() = matrix;

    m_isGenerated = true;
}

void Planet::Render(DirectX::SimpleMath::Matrix view, DirectX::SimpleMath::Matrix proj)
{
    if (m_renderer)
        m_renderer->Render(view, proj);

    if (m_atmosphere)
        m_atmosphere->Render(view, proj);

    if (m_clouds)
        m_clouds->Render(view, proj);

    if (m_grass)
        m_grass->Render(view, proj);
}

void Planet::Update(float dt)
{
    //m_position += m_velocity * dt;

    Matrix rotX = Matrix::CreateRotationX(m_rotation.x);
    Matrix rotY = Matrix::CreateRotationY(m_rotation.y);
    Matrix rotZ = Matrix::CreateRotationZ(m_rotation.z);

    Matrix scale = Matrix::CreateScale((float)(m_radius / Constants::Scale));
    Matrix translation = Matrix::CreateTranslation(m_position);

    m_world = scale * rotX * rotY * rotZ * translation;

    if (m_renderer) m_renderer->Update(dt);
    if (m_atmosphere) m_atmosphere->Update(dt);
    if (m_clouds) m_clouds->Update(dt);
    if (m_grass) m_grass->Update(dt);
}

void Planet::Reset()
{
    if (m_renderer) m_renderer->Reset();
    if (m_atmosphere) m_atmosphere->Reset();
    if (m_clouds) m_clouds->Reset();
    if (m_grass) m_grass->Reset();

    m_renderer.reset();
    m_atmosphere.reset();
    m_clouds.reset();
    m_grass.reset();
}

void Galactic::Planet::ReadSettings(std::string file)
{
    std::ifstream f(file.c_str());

    while (!f.eof())
    {
        int param;
        float value;

        f >> param >> value;

        SetParam((EParams)param, value);
    }

    f.close();
}

Vector3 Galactic::Planet::GetPoint(Vector3 normal) {
    return m_renderer->GetPoint(normal);
}
