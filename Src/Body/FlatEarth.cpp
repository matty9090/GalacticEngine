#include "pch.h"
#include "Body/FlatEarth.hpp"
#include "Physics/Constants.hpp"
#include "Render/DirectX/TextureManager.hpp"

using namespace Galactic;

using namespace DirectX;
using namespace DirectX::SimpleMath;

FlatEarth::FlatEarth(ID3D11DeviceContext *context, std::string name) : m_context(context), m_name(name)
{
    ID3D11Device *device;
    context->GetDevice(&device);

    m_texture =TextureManager::getInstance().GetTexture(device, "Resources/flatearth.jpg");

    m_rotation.x = -0.6f;
}

FlatEarth::~FlatEarth()
{

}

void FlatEarth::Render(DirectX::SimpleMath::Matrix view, DirectX::SimpleMath::Matrix proj)
{
    m_geometry->Draw(m_world, view, proj, Colors::White, m_texture);
}

void FlatEarth::Update(float dt)
{
    m_rotation.z += dt * -20.0f;
    m_position.x -= dt * 1000.0f;

    Matrix rotX = Matrix::CreateRotationX(m_rotation.x);
    Matrix rotY = Matrix::CreateRotationY(m_rotation.y);
    Matrix rotZ = Matrix::CreateRotationZ(m_rotation.z);

    Matrix scale = Matrix::CreateScale(1.0f);
    Matrix translation = Matrix::CreateTranslation(m_position);

    m_world = scale * rotZ * rotX * rotY * translation;
}

void FlatEarth::Reset()
{
    m_geometry.reset();
}

void FlatEarth::Generate(EDetail detail)
{
    detail;
    m_geometry = GeometricPrimitive::CreateCylinder(m_context, 1.4f, 40.0f);
}
