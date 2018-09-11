#include "pch.h"
#include "Render/SphericalQuadTreeTerrain/SphericalQuadTreeTerrain.hpp"

using namespace Galactic;

SphericalQuadTreeTerrain::SphericalQuadTreeTerrain(std::weak_ptr<IPlanet> planet) : m_planet(planet)
{
    
}

void SphericalQuadTreeTerrain::Render(DirectX::SimpleMath::Matrix view, DirectX::SimpleMath::Matrix proj)
{
    view;
    proj;
}

void SphericalQuadTreeTerrain::Update(float dt)
{
    dt;
}

void SphericalQuadTreeTerrain::Reset()
{

}
