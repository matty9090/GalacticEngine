#include "pch.h"
#include "Render/SphericalQuadTreeTerrain/SphericalQuadTreeTerrain.hpp"

using namespace Galactic;
using namespace DirectX;
using namespace DirectX::SimpleMath;

SphericalQuadTreeTerrain::SphericalQuadTreeTerrain(std::weak_ptr<IPlanet> planet)
    : m_planet(planet),
      m_world(planet.lock()->GetMatrix())
{
    
}

SphericalQuadTreeTerrain::~SphericalQuadTreeTerrain()
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
