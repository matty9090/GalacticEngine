#pragma once

#include "IPlanet.hpp"

#include "Render/IPlanetRenderer.hpp"
#include "Render/IAtmosphereRenderer.hpp"
#include "Render/NoiseCloudRenderer.hpp"
#include "Render/GrassRenderer.hpp"
#include "Render/SphericalQuadTreeTerrain/ISphericalTerrain.hpp"

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
            void ReadSettings(std::string file);
            void SetSettings(PlanetSettings settings) { m_settings = settings; }

            void EnableWater(bool enabled) { m_waterEnabled = enabled; Generate(m_detail); }
            void EnableClouds(bool enabled) { m_cloudsEnabled = enabled; }
            void EnableAtmosphere(bool enabled) { m_atmEnabled = enabled; Generate(m_detail); }

            bool IsWaterEnabled() const { return m_waterEnabled; }
            bool IsCloudsEnabled() const { return m_cloudsEnabled; }
            bool IsAtmosphereEnabled() const { return m_atmEnabled; }
            
            void SetRadius(double r) { m_settings.Radius = r; }
            double GetRadius() const { return m_settings.Radius; }

            void SetMass(long double m) { m_settings.Mass = m; }
            long double GetMass() const { return m_settings.Mass; }

            void SetPosition(DirectX::SimpleMath::Vector3 pos) { m_position = pos; }
            DirectX::SimpleMath::Vector3 GetPosition() const { return m_position; }
            
            void SetVelocity(DirectX::SimpleMath::Vector3 vel) { m_velocity = vel; }
            DirectX::SimpleMath::Vector3 GetVelocity() const { return m_velocity; }
            
            void SetRotation(DirectX::SimpleMath::Vector3 vel) { m_rotation = vel; }
            DirectX::SimpleMath::Vector3 GetRotation() const { return m_rotation; }
            
            void SetCameraPos(DirectX::SimpleMath::Vector3 camPos) { m_camPos = camPos; }
            DirectX::SimpleMath::Vector3 GetCameraPos() const { return m_camPos; }

            PlanetSettings &GetSettings() { return m_settings; };
            std::string GetName() const { return m_name; }
            IBody *GetInfluence() const { return m_influence; }
            bool IsGenerated() const { return m_isGenerated; }
            DirectX::SimpleMath::Matrix GetMatrix() const { return m_world; }
            void SetInfluence(IBody *body) { m_influence = body; }

            void IncrementVertices(size_t num) { m_vertexCount += num; }
            size_t GetVertexCount() { return m_vertexCount; }

            DirectX::SimpleMath::Vector3 GetPoint(DirectX::SimpleMath::Vector3 normal);

        private:
            std::string m_name;
            PlanetSettings m_settings;

            EDetail m_detail;
            bool m_isGenerated, m_waterEnabled, m_cloudsEnabled, m_atmEnabled;

            size_t m_vertexCount;

            DirectX::SimpleMath::Matrix  m_world;
            DirectX::SimpleMath::Vector3 m_position, m_velocity, m_rotation, m_camPos;

            IBody *m_influence;
            std::unique_ptr<IPlanetRenderer> m_renderer;
            std::unique_ptr<IAtmosphereRenderer> m_atmosphere;
            std::unique_ptr<NoiseCloudRenderer> m_clouds;
            std::unique_ptr<ISphericalTerrain> m_water;
            Microsoft::WRL::ComPtr<ID3D11DeviceContext> m_deviceContext;
    };
}