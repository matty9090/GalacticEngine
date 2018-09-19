#include "pch.h"

#include "Body/Star.hpp"
#include "Physics/Constants.hpp"
#include "Render/StarRendererFactory.hpp"

using namespace Galactic;
using namespace DirectX;
using namespace DirectX::SimpleMath;

Star::Star(std::string name, ID3D11DeviceContext *context) : m_name(name)
{
    m_world = Matrix::Identity;
    m_renderer = CreateStarRenderer(context, this);
}

void Star::Render(DirectX::SimpleMath::Matrix view, DirectX::SimpleMath::Matrix proj)
{
    m_renderer->Render(view, proj);
}

void Star::Update(float dt)
{
    m_world = Matrix::CreateTranslation(m_position) * Matrix::CreateScale((float)(m_radius / Constants::Scale));

    m_renderer->Update(dt);
}

void Star::Reset()
{
    m_renderer->Reset();
    m_renderer.reset();
}

void Star::Generate(EDetail detail)
{
    m_isGenerated = true;
}
