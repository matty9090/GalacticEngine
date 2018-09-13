#pragma once

#include "Body/IPlanet.hpp"
#include "TerrainNode.hpp"
#include "ISphericalTerrain.hpp"

#include "Render/DirectX/Effect.hpp"
#include "Render/DirectX/ConstantBuffer.hpp"

namespace Galactic
{
    class SphericalQuadTreeTerrain : public ISphericalTerrain, public std::enable_shared_from_this<SphericalQuadTreeTerrain>
    {
        public:
            SphericalQuadTreeTerrain(Microsoft::WRL::ComPtr<ID3D11DeviceContext> deviceContext, std::weak_ptr<IPlanet> planet);
            
            void Generate();
            void CreateEffect();
            void Render(DirectX::SimpleMath::Matrix view, DirectX::SimpleMath::Matrix proj);
            void Update(float dt);
            void Reset();

            int GetGridSize() const { return 17; }
            float GetRadius() const { return m_radius; }

            DirectX::SimpleMath::Matrix &GetMatrix() { return m_world; }
            std::shared_ptr<Effect> GetEffect() const { return m_effect; };
            Microsoft::WRL::ComPtr<ID3D11DeviceContext> GetContext() const { return m_deviceContext; };

        private:
            DirectX::SimpleMath::Matrix &m_world;

            std::weak_ptr<IPlanet> m_planet;
            std::shared_ptr<Effect> m_effect;
            std::unique_ptr<DirectX::CommonStates> m_states;
            std::array<std::shared_ptr<TerrainNode>, 6> m_faces;

            Microsoft::WRL::ComPtr<ID3D11RasterizerState> m_raster, m_rasterWire;
            Microsoft::WRL::ComPtr<ID3D11Device> m_device;
            Microsoft::WRL::ComPtr<ID3D11DeviceContext> m_deviceContext;

            float m_radius;
    };
}