#pragma once

#include "IPlanet.hpp"
#include "Render/IPlanetRenderer.hpp"

namespace Galactic
{
    class Planet : public IPlanet, public std::enable_shared_from_this<Planet>
    {
        public:
            Planet(Microsoft::WRL::ComPtr<ID3D11DeviceContext> deviceContext, std::string name);
            ~Planet();

            void Generate(EDetail detail = EDetail::Medium);
            void SetParameters(float freq = 0.1f, float lacunarity = 2.0f, float gain = 0.5f, float height = 1.0f, int octaves = 6, float noiseScale = 1.0f);

            void Render(DirectX::SimpleMath::Matrix view, DirectX::SimpleMath::Matrix proj);
            void Update(float dt);
            void Reset();

            void SetRadius(double r) { m_radius = r; }
            void SetMass(long double mass) { m_mass = mass; }
            void SetPosition(DirectX::SimpleMath::Vector3 pos) { m_position = pos; }
            void SetVelocity(DirectX::SimpleMath::Vector3 vel) { m_velocity = vel; }
            void SetRotation(DirectX::SimpleMath::Vector3 vel) { m_rotation = vel; }
            void SetCameraPos(DirectX::SimpleMath::Vector3 camPos) { m_camPos = camPos; }
            void SetInfluence(std::shared_ptr<IBody> body) { m_influence = body; }

            double GetRadius() const { return m_radius; }
            long double GetMass() const { return m_mass; }

            void SetOctaves(int v) { m_octaves = v; }
            void SetGain(float v) { m_gain = v; }
            void SetHeight(float v) { m_height = v; }
            void SetFrequency(float v) { m_freq = v; }
            void SetLacunarity(float v) { m_lacunarity = v; }
            void SetNoiseScale(float v) { m_noiseScale = v; }
            void SetMinValue(float v) { m_minValue = v; }

            int GetOctaves() const { return m_octaves; }
            float GetGain() const { return m_gain; }
            float GetHeight() const { return m_height; }
            float GetFrequency() const { return m_freq; }
            float GetLacunarity() const { return m_lacunarity; }
            float GetNoiseScale() const { return m_noiseScale; }
            float GetMinValue() const { return m_minValue; }

            DirectX::SimpleMath::Matrix &GetMatrix()         { return m_world; };
            DirectX::SimpleMath::Vector3 GetPosition() const { return m_position; }
            DirectX::SimpleMath::Vector3 GetVelocity() const { return m_velocity; }
            DirectX::SimpleMath::Vector3 GetRotation() const { return m_rotation; }
            DirectX::SimpleMath::Vector3 GetCameraPos() const { return m_camPos; }

            bool IsGenerated() const { return m_isGenerated; }

        private:
            std::string m_name;

            bool m_isGenerated;
            double m_radius;
            long double m_mass;

            int m_octaves;
            float m_gain, m_lacunarity, m_freq, m_height, m_noiseScale, m_minValue;

            DirectX::SimpleMath::Matrix  m_world;
            DirectX::SimpleMath::Vector3 m_position, m_velocity, m_rotation, m_camPos;

            std::shared_ptr<IBody> m_influence;
            std::shared_ptr<IPlanetRenderer> m_renderer;
            Microsoft::WRL::ComPtr<ID3D11DeviceContext> m_deviceContext;
    };
}