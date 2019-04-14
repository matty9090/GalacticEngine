#include "pch.h"
#include "Scene/Model.hpp"

using namespace Galactic;

Model::Model(ID3D11DeviceContext *context, std::string file) : Drawable(context, D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST),
	m_context(context)
{
	Init();
}

void Galactic::Model::Render(DirectX::SimpleMath::Matrix view, DirectX::SimpleMath::Matrix proj)
{

}

void Galactic::Model::Update(float dt)
{

}

void Galactic::Model::Reset()
{
	Cleanup();
}
