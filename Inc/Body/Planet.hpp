#pragma once

#include "IPlanet.hpp"
#include "Render/IPlanetRenderer.hpp"

namespace Galactic
{
    class Planet : public IPlanet
    {
        public:
            Planet(Microsoft::WRL::ComPtr<ID3D11DeviceContext> deviceContext, std::string name);
            ~Planet();

            void Generate(EDetail detail = EDetail::Medium);

            void Render(DirectX::SimpleMath::Matrix view, DirectX::SimpleMath::Matrix proj);
            void Update(float dt);
            void Reset();

            void SetRadius(double r) { m_radius = r; }
            void SetMass(long double mass) { m_mass = mass; }
            void SetPosition(DirectX::SimpleMath::Vector3 pos) { m_position = pos; }
            void SetVelocity(DirectX::SimpleMath::Vector3 vel) { m_velocity = vel; }
            void SetInfluence(std::shared_ptr<IBody> body) { m_influence = body; }
            bool IsGenerated() const { return m_isGenerated; }

        private:
            std::string m_name;

            bool m_isGenerated;
            double m_radius;
            long double m_mass;

            DirectX::SimpleMath::Vector3 m_position;
            DirectX::SimpleMath::Vector3 m_velocity;

            std::shared_ptr<IBody> m_influence;
            std::shared_ptr<IPlanetRenderer> m_renderer;
            Microsoft::WRL::ComPtr<ID3D11DeviceContext> m_deviceContext;
    };
}