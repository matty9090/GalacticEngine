#include "pch.h"
#include "Scene/SimpleStarSystem.hpp"

using namespace Galactic;
using namespace DirectX;
using namespace DirectX::SimpleMath;

SimpleStarSystem::SimpleStarSystem(std::string name) : m_name(name)
{
    m_world = Matrix::Identity;
}

IBody *SimpleStarSystem::FindBody(std::string name) const
{
    for (auto const &body : m_bodies)
    {
        if (body->GetName() == name)
            return body.get();
    }

    return nullptr;
}

void SimpleStarSystem::Update(float dt)
{
    for (auto const &body : m_bodies) {
        body->SetCameraPos(m_cameraPos);
        body->Update(dt);
    }
}

void SimpleStarSystem::Render(DirectX::SimpleMath::Matrix view, DirectX::SimpleMath::Matrix proj)
{
    for (auto const &body : m_bodies)
        body->Render(view, proj);
}

void SimpleStarSystem::Reset()
{
    for (auto &body : m_bodies)
    {
        body->Reset();
        body.reset();
    }
}

std::vector<ILightSource*> SimpleStarSystem::GetLightSources() const
{
    std::vector<ILightSource*> lights;
    
    for (auto const &light : m_lights)
        lights.push_back(light.get());

    return lights;
}
