#include "pch.h"

#include "Body/Star.hpp"
#include "Physics/Constants.hpp"
#include "Render/StarRendererFactory.hpp"

using namespace Galactic;
using namespace DirectX;
using namespace DirectX::SimpleMath;

Star::Star(std::string name, ID3D11DeviceContext *context) : m_name(name), m_influence(nullptr)
{
    m_world = Matrix::Identity;
    m_renderer = CreateStarRenderer(context, this);
    m_billboard = std::make_unique<Billboard>(context, this, Vector3::Zero, "Resources/StarGlow.png", Billboard::Additive);
}

void Star::Render(DirectX::SimpleMath::Matrix view, DirectX::SimpleMath::Matrix proj)
{
    //m_renderer->Render(view, proj);
    //m_billboard->Render(view, proj);
}

void Star::Update(float dt)
{
    m_position += m_velocity * dt;

    m_world = Matrix::CreateScale((float)(m_radius / Constants::Scale)) * Matrix::CreateTranslation(m_position);

    float size = CalculateGlowSize(Vector3::Distance(m_camPos, m_position)) * 10.0f;

    m_billboard->SetScale(size);
    m_renderer->Update(dt);
    m_billboard->Update(dt);
}

void Star::Reset()
{
    m_renderer->Reset();
    m_renderer.reset();

    m_billboard->Reset();
    m_billboard.reset();
}

void Star::Generate(EDetail detail)
{
    detail;

    m_isGenerated = true;
}

float Galactic::Star::CalculateGlowSize(float distance)
{
    static const double DSUN = 1392684.0;
    static const double TSUN = 5778.0;

    double d = distance / Constants::Scale;
    double D = m_radius * 2 * DSUN;
    double L = (D * D) * pow(m_temperature / TSUN, 4.0);
    return (float)(0.016 * pow(L, 0.25) / pow(d, 0.5));
}
