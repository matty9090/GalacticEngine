#include "pch.h"
#include "Scene/SimpleStarSystem.hpp"
#include "Physics/Gravity.hpp"

using namespace Galactic;
using namespace DirectX;
using namespace DirectX::SimpleMath;

SimpleStarSystem::SimpleStarSystem(std::string name) : m_name(name)
{
    m_world = Matrix::Identity;
}

void Galactic::SimpleStarSystem::RemoveBody(std::string name)
{
    for (auto it = m_bodies.begin(); it != m_bodies.end(); ++it)
    {
        if ((*it)->GetName() == name)
        {
            //(*it)->Reset();
            m_bodies.erase(it);
            return;
        }
    }
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

IBody *SimpleStarSystem::GetClosestBody(DirectX::SimpleMath::Vector3 pos) const
{
    IBody *body = nullptr;
    float d = std::numeric_limits<float>::max();

    for (auto &b : m_bodies)
    {
        float dist = Vector3::DistanceSquared(pos, b->GetPosition());

        if (dist < d)
            d = dist, body = b.get();
    }

    return body;
}

void SimpleStarSystem::Update(float dt)
{
    for (auto const &body : m_bodies)
    {
        IBody *inf = body->GetInfluence();

        if (inf != nullptr)
        {
            Vector3 force = Gravity::Force(inf, body.get());
            Vector3 vel = body->GetVelocity();

            vel += (force / inf->GetMass()) * dt;

            body->SetVelocity(vel);
        }
    }
    
    for (auto const &body : m_bodies)
    {
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

std::list<ILightSource*> SimpleStarSystem::GetLightSources() const
{
    return m_lights;
}
