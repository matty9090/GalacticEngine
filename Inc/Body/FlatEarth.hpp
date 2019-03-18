#include "IBody.hpp"
#include <GeometricPrimitive.h>

namespace Galactic
{
    /**
     * @brief Flat Earth body
     * 
     */
    class FlatEarth : public IBody
    {
        public:
            FlatEarth(ID3D11DeviceContext *context, std::string name);
            virtual ~FlatEarth();

            void Render(DirectX::SimpleMath::Matrix view, DirectX::SimpleMath::Matrix proj);
            void Update(float dt);
            void Reset();

            void Generate(EDetail detail = EDetail::Medium);

            void SetMass(long double mass) { m_mass = mass; };

            /**
             * @brief Set the radius
             * 
             * @param radius 
             */
            void SetRadius(long double radius) { m_radius = radius; };

            void SetPosition(DirectX::SimpleMath::Vector3 pos) { m_position = pos; }
            void SetVelocity(DirectX::SimpleMath::Vector3 vel) { m_velocity = vel; }
            void SetRotation(DirectX::SimpleMath::Vector3 rot) { m_rotation = rot; }
            void SetCameraPos(DirectX::SimpleMath::Vector3 camPos) { m_camPos = camPos; }
            void SetInfluence(IBody *body) { m_influence = body; }

            std::string                  GetName()     const { return m_name; }
            long double                  GetMass()     const { return m_mass; }
            IBody                       *GetInfluence() const { return m_influence; }
            DirectX::SimpleMath::Vector3 GetPosition() const { return m_position; }
            DirectX::SimpleMath::Vector3 GetVelocity() const { return m_velocity; }
            DirectX::SimpleMath::Vector3 GetRotation() const { return m_rotation; }
            DirectX::SimpleMath::Vector3 GetCameraPos() const { return m_camPos; }

            bool IsGenerated() const { return m_isGenerated; }

            DirectX::SimpleMath::Matrix GetMatrix() const { return m_world; };

        private:
            std::string m_name;

            bool m_isGenerated;
            long double m_mass, m_radius;
            IBody *m_influence;

            ID3D11DeviceContext *m_context;
            ID3D11ShaderResourceView *m_texture;

            std::unique_ptr<DirectX::CommonStates> m_states;
            std::unique_ptr<DirectX::GeometricPrimitive> m_geometry;

            DirectX::SimpleMath::Matrix  m_world;
            DirectX::SimpleMath::Vector3 m_position, m_velocity, m_rotation, m_camPos;
    };
}