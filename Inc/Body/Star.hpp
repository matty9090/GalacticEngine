#pragma once

#include "Body/IStar.hpp"
#include "Scene/ILightSource.hpp"
#include "Render/IStarRenderer.hpp"

namespace Galactic
{
    class Star : public IStar, public ILightSource
    {
        public:
            Star(std::string name, ID3D11DeviceContext *context);

            void Render(DirectX::SimpleMath::Matrix view, DirectX::SimpleMath::Matrix proj);
            void Update(float dt);
            void Reset();

            void Generate(EDetail detail = EDetail::Medium);

            void SetRadius(double r) { m_radius = r; }
            void SetMass(long double mass) { m_mass = mass; }
            void SetColour(DirectX::SimpleMath::Color col) { m_colour = col; }
            void SetTemperature(int kelvin) { m_temperature = kelvin; }
            void SetPosition(DirectX::SimpleMath::Vector3 pos) { m_position = pos; }
            void SetVelocity(DirectX::SimpleMath::Vector3 vel) { m_velocity = vel; }
            void SetRotation(DirectX::SimpleMath::Vector3 vel) { m_rotation = vel; }
            void SetCameraPos(DirectX::SimpleMath::Vector3 camPos) { m_camPos = camPos; }
            void SetInfluence(std::shared_ptr<IBody> body) { m_influence = body; }

            int                          GetTemperature() const { return m_temperature; }
            double                       GetRadius()   const { return m_radius; }
            std::string                  GetName()     const { return m_name; }
            long double                  GetMass()     const { return m_mass; }
            DirectX::SimpleMath::Color   GetColour()   const { return m_colour; }
            DirectX::SimpleMath::Matrix  GetMatrix()   const { return m_world; }
            DirectX::SimpleMath::Vector3 GetPosition() const { return m_position; }
            DirectX::SimpleMath::Vector3 GetVelocity() const { return m_velocity; }
            DirectX::SimpleMath::Vector3 GetRotation() const { return m_rotation; }
            DirectX::SimpleMath::Vector3 GetCameraPos() const { return m_camPos; }

            bool IsGenerated() const { return m_isGenerated; }

    private:
            std::string m_name;

            bool m_isGenerated;
            int m_temperature;
            double m_radius;
            long double m_mass;

            std::shared_ptr<IBody> m_influence;
            std::unique_ptr<IStarRenderer> m_renderer;

            DirectX::SimpleMath::Color m_colour;
            DirectX::SimpleMath::Matrix  m_world;
            DirectX::SimpleMath::Vector3 m_position, m_velocity, m_rotation, m_camPos;
    };
}