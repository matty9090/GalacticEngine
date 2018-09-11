#pragma once

#include "pch.h"
#include "Render/IRenderable.hpp"
#include "Render/PlanetRendererHigh.hpp"

using namespace Galactic;
using namespace DirectX;
using namespace DirectX::SimpleMath;

HighDetailPlanetRenderer::HighDetailPlanetRenderer(Microsoft::WRL::ComPtr<ID3D11DeviceContext> deviceContext, std::shared_ptr<IPlanet> planet) : m_deviceContext(deviceContext), m_planet(planet)
{
    m_geometry = GeometricPrimitive::CreateSphere(deviceContext.Get(), 1.0f, 64U);
    m_world = Matrix::Identity;
    m_terrain = std::make_unique<SphericalQuadTreeTerrain>(planet);
}

void HighDetailPlanetRenderer::Render(DirectX::SimpleMath::Matrix view, DirectX::SimpleMath::Matrix proj)
{
    m_geometry->Draw(m_world, view, proj);
}

void HighDetailPlanetRenderer::Update(float dt)
{
    dt;
}

void Galactic::HighDetailPlanetRenderer::Reset()
{
    m_geometry.reset();
    m_terrain.reset();
}
