#pragma once

#include "Body/IPlanet.hpp"
#include "Render/IPlanetRenderer.hpp"

namespace Galactic {
    /**
     * @brief Medium detail planet renderer
     * 
     */
    class MedDetailPlanetRenderer : public IPlanetRenderer
    {
        public:
            /**
             * @brief Construct a new Med Detail Planet Renderer object
             * 
             * @param device 
             * @param planet 
             */
            MedDetailPlanetRenderer(Microsoft::WRL::ComPtr<ID3D11DeviceContext> device, IPlanet *planet);

            void Render(DirectX::SimpleMath::Matrix view, DirectX::SimpleMath::Matrix proj);
            void Update(float dt);
            void Reset();

            DirectX::SimpleMath::Matrix GetMatrix() const { return m_world; }

            /**
             * @brief Get the point on the terrains surface at the specified normal on the sphere
             * 
             * @param normal 
             * @return DirectX::SimpleMath::Vector3 
             */
            DirectX::SimpleMath::Vector3 GetPoint(DirectX::SimpleMath::Vector3 normal) { return normal; }

        private:
            DirectX::SimpleMath::Matrix m_world;

            IPlanet *m_planet;
            Microsoft::WRL::ComPtr<ID3D11DeviceContext> m_deviceContext;
            std::unique_ptr<DirectX::GeometricPrimitive> m_geometry;
    };
}