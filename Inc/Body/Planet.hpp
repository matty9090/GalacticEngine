#pragma once

#include "IPlanet.hpp"

namespace Galactic
{
    class Planet : public IPlanet
    {
        public:
            Planet(Microsoft::WRL::ComPtr<ID3D11DeviceContext> deviceContext, std::string name);
            ~Planet();

            void Generate();
            void Render(DirectX::SimpleMath::Matrix view, DirectX::SimpleMath::Matrix proj);
            void Update(float dt);
            void Reset();

            void SetRadius(double r) { m_radius = r; }
            void SetMass(long double mass) { m_mass = mass; }
            void SetPosition(DirectX::SimpleMath::Vector3 pos) { m_position = pos; }
            void SetVelocity(DirectX::SimpleMath::Vector3 vel) { m_velocity = vel; }
            void SetInfluence(std::shared_ptr<IBody> body) { m_influence = body; }

        private:
            std::string m_name;

            double m_radius;
            long double m_mass;

            DirectX::SimpleMath::Vector3 m_position;
            DirectX::SimpleMath::Vector3 m_velocity;

            std::shared_ptr<IBody> m_influence;
            std::shared_ptr<IRenderable> m_renderer;
    };
}