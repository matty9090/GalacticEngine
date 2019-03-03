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

#include <mutex>

namespace Galactic
{
    struct WaterBuffer
    {
        float Scroll;
        float p0, p1, p2;
    };

    class SphericalQuadTreeWater : public ISphericalTerrain
    {
        public:
            SphericalQuadTreeWater(Microsoft::WRL::ComPtr<ID3D11DeviceContext> deviceContext, IPlanet *planet);

            void Generate();
            void CreateEffect();
            void Render(DirectX::SimpleMath::Matrix view, DirectX::SimpleMath::Matrix proj);
            void Update(float dt);
            void Reset();

            float GetRadius() const { return m_radius; }
            size_t GetGridSize() const { return SphericalQuadTreeWater::GridSize; }

            void RequestNoiseSet(int x, int y, int z, int w, int h, int d, float scale) {}
            void FreeNoiseSet() {}

            __forceinline std::string GetBiome(const DirectX::SimpleMath::Vector2 &lookup);
            __forceinline void GetHeight(int index, float &height, DirectX::SimpleMath::Vector2 &biomeLookup, std::string &texIndex);

            void SetRenderContext();
            DirectX::SimpleMath::Matrix GetMatrix() const { return m_world; }
            Effect *GetEffect() const { return m_effect; };
            Microsoft::WRL::ComPtr<ID3D11DeviceContext> GetContext() const { return m_context; };

            static bool   CancelGeneration;
            static size_t GridSize;
            static size_t FrameSplits;
            static size_t MaxSplitsPerFrame;

        private:
            DirectX::SimpleMath::Matrix m_world;

            float m_scroll, m_waterLevel;

            std::mutex m_mutex;
            IPlanet *m_planet;
            Effect *m_effect;

            Gradient::Gradient<Gradient::GradientColor> m_gradient;

            std::unique_ptr<DirectX::CommonStates> m_states;
            std::array<std::unique_ptr<TerrainNode>, 6> m_faces;
            std::unique_ptr<ConstantBuffer<WaterBuffer>> m_waterBuffer;
            std::unique_ptr<ConstantBuffer<ScatterBuffer>> m_buffer;

            ID3D11ShaderResourceView *m_texNormalMap;

            Microsoft::WRL::ComPtr<ID3D11RasterizerState> m_raster, m_rasterWire;
            Microsoft::WRL::ComPtr<ID3D11Device> m_device;
            Microsoft::WRL::ComPtr<ID3D11DeviceContext> m_context;

            float m_radius;

            void InitEffect();
    };
}