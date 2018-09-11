#pragma once

#include "pch.h"
#include "Render/IRenderable.hpp"
#include "Render/PlanetRendererMed.hpp"

using namespace Galactic;
using namespace DirectX;
using namespace DirectX::SimpleMath;

MedDetailPlanetRenderer::MedDetailPlanetRenderer(Microsoft::WRL::ComPtr<ID3D11DeviceContext> deviceContext, std::shared_ptr<IPlanet> planet) : m_deviceContext(deviceContext), m_planet(planet)
{
    m_geometry = GeometricPrimitive::CreateSphere(deviceContext.Get(), 1.0f, 16U);
    m_world = Matrix::Identity;
}

void MedDetailPlanetRenderer::Render(DirectX::SimpleMath::Matrix view, DirectX::SimpleMath::Matrix proj)
{
    m_geometry->Draw(m_world, view, proj);
}

void MedDetailPlanetRenderer::Update(float dt)
{
    dt;
}

void Galactic::MedDetailPlanetRenderer::Reset()
{
    m_geometry.reset();
}
