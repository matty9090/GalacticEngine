#pragma once

#include "Body/IPlanet.hpp"
#include "Render/IPlanetRenderer.hpp"
#include "Render/SphericalQuadTreeTerrain/SphericalQuadTreeTerrain.hpp"

namespace Galactic {
    /**
     * @brief High detail planet renderer
     * 
     */
    class HighDetailPlanetRenderer : public IPlanetRenderer
    {
        public:
            /**
             * @brief Construct a new High Detail Planet Renderer object
             * 
             * @param device 
             * @param planet 
             */
            HighDetailPlanetRenderer(Microsoft::WRL::ComPtr<ID3D11DeviceContext> device, IPlanet *planet);

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
            DirectX::SimpleMath::Vector3 GetPoint(DirectX::SimpleMath::Vector3 normal);

        private:
            DirectX::SimpleMath::Matrix m_world;

            Microsoft::WRL::ComPtr<ID3D11DeviceContext> m_deviceContext;
            
            std::shared_ptr<SphericalQuadTreeTerrain> m_terrain;
            IPlanet *m_planet;
    };
}