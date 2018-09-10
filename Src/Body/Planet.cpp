#include "pch.h"
#include "Body/Planet.hpp"
#include "Render/PlanetRendererFactory.hpp"

using namespace Galactic;

Planet::Planet(Microsoft::WRL::ComPtr<ID3D11DeviceContext> deviceContext, std::string name) : m_name(name)
{
	m_renderer = CreatePlanetRenderer(deviceContext, EPlanetDetail::Low);
}

Planet::~Planet()
{

}

void Planet::Generate()
{
	
}

void Planet::Render(DirectX::SimpleMath::Matrix view, DirectX::SimpleMath::Matrix proj)
{
	m_renderer->Render(view, proj);
}

void Planet::Update(float dt)
{
	m_renderer->Update(dt);
}

void Galactic::Planet::Reset()
{
	m_renderer->Reset();
}
