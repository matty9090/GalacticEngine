#pragma once

#include "pch.h"
#include "Render/IRenderable.hpp"
#include "Render/PlanetRendererHigh.hpp"

using namespace Galactic;
using namespace DirectX;
using namespace DirectX::SimpleMath;

HighDetailPlanetRenderer::HighDetailPlanetRenderer(Microsoft::WRL::ComPtr<ID3D11DeviceContext> deviceContext, std::shared_ptr<IPlanet> planet)
    : m_deviceContext(deviceContext),
      m_planet(planet),
      m_world(planet->GetMatrix())
{
    m_world = Matrix::Identity;
    m_terrain = std::make_shared<SphericalQuadTreeTerrain>(deviceContext, planet);
    m_terrain->Generate(m_planet->GetFrequency(), m_planet->GetLacunarity(), m_planet->GetGain(), m_planet->GetOctaves());
}

void HighDetailPlanetRenderer::Render(DirectX::SimpleMath::Matrix view, DirectX::SimpleMath::Matrix proj)
{
    m_terrain->Render(view, proj);
}

void HighDetailPlanetRenderer::Update(float dt)
{
    m_terrain->Update(dt);
}

void Galactic::HighDetailPlanetRenderer::Reset()
{
    m_terrain->Reset();
    m_terrain.reset();
}
