#include "pch.h"
#include "Render/GrassRenderer.hpp"

using namespace Galactic;
using namespace DirectX;
using namespace DirectX::SimpleMath;

GrassRenderer::GrassRenderer(ID3D11DeviceContext *context, IPlanet *planet)
    : m_planet(planet),
      m_distributor()
{
    m_grass = std::make_unique<Billboard>(context, planet, Vector3::Zero, "Resources/grass.png", Billboard::Alpha);
    m_grass->SetScale(0.1f);
}

GrassRenderer::~GrassRenderer()
{

}

void GrassRenderer::Render(Matrix view, Matrix proj)
{
    for (const auto &patch : m_distributor.GetPatches())
    {
        m_grass->SetPosition(patch.second.pos);
        m_grass->Render(view, proj);
    }
}

void GrassRenderer::Update(float dt)
{
    dt;
}

void GrassRenderer::Reset()
{
    
}
