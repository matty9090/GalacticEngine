#include "pch.h"
#include "Body/Planet.hpp"
#include "Render/PlanetRendererFactory.hpp"

using namespace Galactic;
using namespace DirectX;
using namespace DirectX::SimpleMath;

Planet::Planet(Microsoft::WRL::ComPtr<ID3D11DeviceContext> deviceContext, std::string name)
    : m_name(name),
      m_deviceContext(deviceContext),
      m_renderer(nullptr),
      m_isGenerated(false)
{
    
}

Planet::~Planet()
{

}

void Planet::Generate(EDetail detail)
{
    Matrix matrix;

    if (m_isGenerated)
        matrix = m_renderer->GetMatrix();

    m_renderer = CreatePlanetRenderer(m_deviceContext, shared_from_this(), detail);

    if (m_isGenerated)
        m_renderer->GetMatrix() = matrix;

    m_isGenerated = true;
}

void Planet::Render(DirectX::SimpleMath::Matrix view, DirectX::SimpleMath::Matrix proj)
{
    if(m_renderer)
        m_renderer->Render(view, proj);
}

void Planet::Update(float dt)
{
    if (m_renderer) {
        m_renderer->Update(dt);
        
        Matrix &m = m_renderer->GetMatrix();
        m *= Matrix::CreateRotationY(cosf(dt) * 0.006f);
    }
}

void Galactic::Planet::Reset()
{
    if (m_renderer)
        m_renderer->Reset();
}
