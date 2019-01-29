#pragma once

#include "Body/IPlanet.hpp"
#include "TerrainNode.hpp"
#include "ISphericalTerrain.hpp"

#include "Noise/Biome.hpp"
#include "Noise/FastNoise.h"
#include "Render/Scatter.hpp"
#include "Render/DirectX/EffectManager.hpp"
#include "Render/DirectX/ConstantBuffer.hpp"

#include "Physics/Constants.hpp"

namespace Galactic
{
    class SphericalQuadTreeTerrain : public ISphericalTerrain
    {
        public:
            SphericalQuadTreeTerrain(Microsoft::WRL::ComPtr<ID3D11DeviceContext> deviceContext, IPlanet *planet);

            void Generate();
            void CreateEffect();
            void Render(DirectX::SimpleMath::Matrix view, DirectX::SimpleMath::Matrix proj);
            void Update(float dt);
            void Reset();

            float GetRadius() const { return m_radius; }
            void  GetHeight(DirectX::SimpleMath::Vector3 point, float &height, DirectX::SimpleMath::Vector2 &biomeLookup, int &texIndex);

            void SetRenderContext();
            DirectX::SimpleMath::Matrix GetMatrix() const { return m_world; }
            Effect *GetEffect() const { return m_effect; };
            Microsoft::WRL::ComPtr<ID3D11DeviceContext> GetContext() const { return m_deviceContext; };
            ID3D11ShaderResourceView *GetTexture(int tex) const { return m_textures[tex]; }

            static bool   CancelGeneration;
            static size_t GridSize;
            static size_t FrameSplits;
            static size_t MaxSplitsPerFrame;

        private:
            std::map<EBiomes, std::unique_ptr<Biome>> m_biomes;
            FastNoise m_noise, m_snoise, m_bnoise;
            DirectX::SimpleMath::Matrix m_world;

            IPlanet *m_planet;
            Effect *m_effect;

            Gradient::Gradient<Gradient::GradientColor> m_gradient;

            std::unique_ptr<DirectX::CommonStates> m_states;
            std::array<std::unique_ptr<TerrainNode>, 6> m_faces;
            std::unique_ptr<ConstantBuffer<ScatterBuffer>> m_buffer;

            ID3D11ShaderResourceView *m_texBiomes;

            Microsoft::WRL::ComPtr<ID3D11RasterizerState> m_raster, m_rasterWire;
            Microsoft::WRL::ComPtr<ID3D11Device> m_device;
            Microsoft::WRL::ComPtr<ID3D11DeviceContext> m_deviceContext;
            
            std::vector<ID3D11ShaderResourceView*> m_textures;

            float m_radius;
    };
}