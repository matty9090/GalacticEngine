#pragma once

#include "pch.h"
#include "Render/IRenderable.hpp"
#include "Render/PlanetRendererMed.hpp"
#include "Physics/Constants.hpp"

using namespace Galactic;
using namespace DirectX;
using namespace DirectX::SimpleMath;

MedDetailPlanetRenderer::MedDetailPlanetRenderer(Microsoft::WRL::ComPtr<ID3D11DeviceContext> deviceContext, std::weak_ptr<IPlanet> planet)
    : m_deviceContext(deviceContext),
      m_planet(planet),
      m_world(planet.lock()->GetMatrix())
{
    m_geometry = GeometricPrimitive::CreateSphere(deviceContext.Get(), 2.0f, 16U);
}

void MedDetailPlanetRenderer::Render(DirectX::SimpleMath::Matrix view, DirectX::SimpleMath::Matrix proj)
{
    m_geometry->Draw(m_world, view, proj);
}

void MedDetailPlanetRenderer::Update(float dt)
{
    dt;

    m_world *= Matrix::CreateScale(m_planet->GetRadius() / Constants::Scale);
}

void Galactic::MedDetailPlanetRenderer::Reset()
{
    m_geometry.reset();
}
