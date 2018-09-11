#include "pch.h"
#include "Physics/Constants.hpp"
#include "Render/SphericalQuadTreeTerrain/SphericalQuadTreeTerrain.hpp"

using namespace Galactic;
using namespace DirectX;
using namespace DirectX::SimpleMath;

SphericalQuadTreeTerrain::SphericalQuadTreeTerrain(Microsoft::WRL::ComPtr<ID3D11DeviceContext> deviceContext, std::weak_ptr<IPlanet> planet)
    : m_deviceContext(deviceContext),
      m_planet(planet),
      m_world(planet.lock()->GetMatrix()),
      m_radius((float)(planet.lock()->GetRadius() / Constants::Scale))
{
    
}

void SphericalQuadTreeTerrain::Generate()
{
    m_root = std::make_shared<TerrainNode>(weak_from_this(), std::weak_ptr<TerrainNode>(), m_planet);
    m_root->Generate();
}

void SphericalQuadTreeTerrain::Render(DirectX::SimpleMath::Matrix view, DirectX::SimpleMath::Matrix proj)
{
    view;
    proj;
}

void SphericalQuadTreeTerrain::Update(float dt)
{
    m_root->Update(dt);
}

void SphericalQuadTreeTerrain::Reset()
{

}
