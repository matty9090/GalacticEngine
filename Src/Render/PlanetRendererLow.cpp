#pragma once

#include "pch.h"
#include "Render/IRenderable.hpp"
#include "Render/PlanetRendererLow.hpp"
#include "Physics/Constants.hpp"

using namespace Galactic;
using namespace DirectX;
using namespace DirectX::SimpleMath;

LowDetailPlanetRenderer::LowDetailPlanetRenderer(Microsoft::WRL::ComPtr<ID3D11DeviceContext> deviceContext, IPlanet *planet)
    : m_deviceContext(deviceContext),
      m_planet(planet)
{
    m_geometry = GeometricPrimitive::CreateSphere(deviceContext.Get(), 2.0f, 5U);
}

void LowDetailPlanetRenderer::Render(DirectX::SimpleMath::Matrix view, DirectX::SimpleMath::Matrix proj)
{
    m_geometry->Draw(m_world, view, proj);
}

void LowDetailPlanetRenderer::Update(float dt)
{
    dt;

    m_world = Matrix::CreateScale((float)(m_planet->GetRadius() / Constants::Scale));
    m_world *= m_planet->GetMatrix();
}

void Galactic::LowDetailPlanetRenderer::Reset()
{
    m_geometry.reset();
}
