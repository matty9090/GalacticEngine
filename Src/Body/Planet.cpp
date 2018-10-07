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
      m_atmosphereHeight(200.0f),
      m_atmColour(Color(0.0f, 0.7f, 1.0f))
{
    m_params.resize(20);
    SetParam(EParams::Biomes, 0.0f);
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

void Planet::Reset()
{
    if (m_renderer)
        m_renderer->Reset();
    
    if(m_atmosphere)
        m_atmosphere->Reset();

    m_renderer.reset();
    m_atmosphere.reset();
}
