#pragma once

#include "IPlanet.hpp"

#include "Render/IPlanetRenderer.hpp"
#include "Render/IAtmosphereRenderer.hpp"

#include <map>

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

			void SetParam(std::string name, float value) { m_params[name] = value; }
			float GetParam(std::string name) { return m_params[name]; }
            
			void SetRadius(double r) { m_radius = r; }
            double GetRadius() const { return m_radius; }
            
			void SetMass(long double mass) { m_mass = mass; }
            long double GetMass() const { return m_mass; }
            
			void SetPosition(DirectX::SimpleMath::Vector3 pos) { m_position = pos; }
            DirectX::SimpleMath::Vector3 GetPosition() const { return m_position; }
            
			void SetVelocity(DirectX::SimpleMath::Vector3 vel) { m_velocity = vel; }
            DirectX::SimpleMath::Vector3 GetVelocity() const { return m_velocity; }
            
			void SetRotation(DirectX::SimpleMath::Vector3 vel) { m_rotation = vel; }
            DirectX::SimpleMath::Vector3 GetRotation() const { return m_rotation; }
            
			void SetCameraPos(DirectX::SimpleMath::Vector3 camPos) { m_camPos = camPos; }
            DirectX::SimpleMath::Vector3 GetCameraPos() const { return m_camPos; }

            void SetAtmosphereHeight(float h) { m_atmosphereHeight = h; }
            float GetAtmosphereHeight() const { return m_atmosphereHeight; }
            
			void SetAtmosphereColour(DirectX::SimpleMath::Color colour) { m_atmColour = colour; }
            DirectX::SimpleMath::Color GetAtmosphereColour() const { return m_atmColour; }

			void SetSeed(int seed) { m_seed = seed; }
			int GetSeed() { return m_seed; }

            std::string GetName() const { return m_name; }
            IBody *GetInfluence() const { return m_influence; }
            bool IsGenerated() const { return m_isGenerated; }
            DirectX::SimpleMath::Matrix GetMatrix() const { return m_world; }
            void SetInfluence(IBody *body) { m_influence = body; }

        private:
            std::string m_name;

			int m_seed;
            bool m_isGenerated;
            double m_radius;
            long double m_mass;
			float m_atmosphereHeight;

			std::map<std::string, float> m_params;

            DirectX::SimpleMath::Color m_atmColour;
            DirectX::SimpleMath::Matrix  m_world;
            DirectX::SimpleMath::Vector3 m_position, m_velocity, m_rotation, m_camPos;

            IBody *m_influence;
            std::unique_ptr<IPlanetRenderer> m_renderer;
            std::unique_ptr<IAtmosphereRenderer> m_atmosphere;
            Microsoft::WRL::ComPtr<ID3D11DeviceContext> m_deviceContext;
    };
}