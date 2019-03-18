#pragma once

#include "pch.h"
#include "Render/IRenderable.hpp"
#include "Render/PlanetRendererHigh.hpp"

using namespace Galactic;
using namespace DirectX;
using namespace DirectX::SimpleMath;

HighDetailPlanetRenderer::HighDetailPlanetRenderer(Microsoft::WRL::ComPtr<ID3D11DeviceContext> deviceContext, IPlanet *planet)
    : m_deviceContext(deviceContext),
      m_planet(planet)
{
    m_world = Matrix::Identity;
    m_terrain = std::make_shared<SphericalQuadTreeTerrain>(deviceContext, planet);
    m_terrain->Generate();
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
    m_terrain->Generate();
}

Vector3 Galactic::HighDetailPlanetRenderer::GetPoint(Vector3 normal)
{
    normal.Normalize();

    Vector2 lookup;
    float height;
    std::string tex;

    m_terrain->GetHeight(normal, height, lookup, tex);

    return Vector3::Transform(normal + normal * height, m_terrain->GetMatrix());
}
