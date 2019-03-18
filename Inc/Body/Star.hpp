#pragma once

#include "Body/IStar.hpp"
#include "Scene/ILightSource.hpp"

#include "Render/Billboard.hpp"
#include "Render/IStarRenderer.hpp"

namespace Galactic
{
    /**
     * @brief Star implementation
     * 
     */
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

            /**
             * @brief Set the colour of the star
             * 
             * @param col 
             */
            void SetColour(DirectX::SimpleMath::Color col) { m_colour = col; }

            void SetTemperature(int kelvin) { m_temperature = kelvin; }
            void SetPosition(DirectX::SimpleMath::Vector3 pos) { m_position = pos; }
            void SetVelocity(DirectX::SimpleMath::Vector3 vel) { m_velocity = vel; }
            void SetRotation(DirectX::SimpleMath::Vector3 vel) { m_rotation = vel; }
            void SetCameraPos(DirectX::SimpleMath::Vector3 camPos) { m_camPos = camPos; }
            void SetInfluence(IBody *body) { m_influence = body; }

            IBody                       *GetInfluence() const { return m_influence; }
            int                          GetTemperature() const { return m_temperature; }
            double                       GetRadius()   const { return m_radius; }
            std::string                  GetName()     const { return m_name; }
            long double                  GetMass()     const { return m_mass; }

            /**
             * @brief Get the colour of the star
             * 
             * @return DirectX::SimpleMath::Color 
             */
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

                IBody *m_influence;
                std::unique_ptr<Billboard> m_billboard;
                std::unique_ptr<IStarRenderer> m_renderer;

                DirectX::SimpleMath::Color m_colour;
                DirectX::SimpleMath::Matrix  m_world;
                DirectX::SimpleMath::Vector3 m_position, m_velocity, m_rotation, m_camPos;

                float CalculateGlowSize(float distance);
    };
}