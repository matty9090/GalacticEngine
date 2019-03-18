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
#include <functional>

namespace Galactic
{
    /**
     * @brief Implementation of spherical quadtree terrain
     * 
     */
    class SphericalQuadTreeTerrain : public ISphericalTerrain
    {
        public:
            /**
             * @brief Construct a new Spherical Quad Tree Terrain object
             * 
             * @param deviceContext 
             * @param planet 
             */
            SphericalQuadTreeTerrain(Microsoft::WRL::ComPtr<ID3D11DeviceContext> deviceContext, IPlanet *planet);

            /**
             * @brief Generate the geometry
             * 
             */
            void Generate();

            // TODO: Make private
            void CreateEffect();
            void Render(DirectX::SimpleMath::Matrix view, DirectX::SimpleMath::Matrix proj);
            void Update(float dt);
            void Reset();

            float GetRadius() const { return m_radius; }
            size_t GetGridSize() const { return SphericalQuadTreeTerrain::GridSize; }

            __forceinline std::string GetBiome(const DirectX::SimpleMath::Vector2 &lookup);
            __forceinline void GetHeight(const DirectX::SimpleMath::Vector3 &point, float &height, DirectX::SimpleMath::Vector2 &biomeLookup, std::string &texIndex);

            DirectX::SimpleMath::Matrix GetMatrix() const { return m_world; }
            Effect *GetEffect() const { return m_effect; };
            Microsoft::WRL::ComPtr<ID3D11DeviceContext> GetContext() const { return m_context; };

            enum EPermutations { None, Bottom, Left, Top, Right, BottomLeft, LeftTop, TopRight, RightBottom };
            static std::map<EPermutations, std::vector<uint16_t>> Perms;

            static bool   CancelGeneration;
            static size_t GridSize;
            static size_t FrameSplits;
            static size_t MaxSplitsPerFrame;

        private:
            DirectX::SimpleMath::Matrix m_world;

            FastNoise m_biomeMap;
            std::vector<FastNoise> m_noiseMaps;

            std::mutex m_mutex;
            IPlanet *m_planet;
            Effect *m_effect;

            Gradient::Gradient<Gradient::GradientColor> m_gradient;

            std::unique_ptr<DirectX::CommonStates> m_states;
            std::array<std::unique_ptr<TerrainNode>, 6> m_faces;
            std::unique_ptr<ConstantBuffer<ScatterBuffer>> m_buffer;

            Microsoft::WRL::ComPtr<ID3D11SamplerState> m_sampler;
            ID3D11ShaderResourceView *m_texBiomes, *m_textures, *m_normalMaps;

            Microsoft::WRL::ComPtr<ID3D11RasterizerState> m_raster, m_rasterWire;
            Microsoft::WRL::ComPtr<ID3D11Device> m_device;
            Microsoft::WRL::ComPtr<ID3D11DeviceContext> m_context;

            float m_radius;

            void InitEffect();
            void LoadTextures();
            void SetRenderContext();
            void GeneratePermutations();
    };
}