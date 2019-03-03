#pragma once

#include "Render/IRenderable.hpp"
#include "Render/DirectX/Effect.hpp"
#include "Noise/FastNoise.h"

namespace Galactic
{
    class ISphericalTerrain : public IRenderable
    {
        public:
            virtual float GetRadius() const = 0;

            virtual Effect *GetEffect() const = 0;
            virtual Microsoft::WRL::ComPtr<ID3D11DeviceContext> GetContext() const = 0;

            virtual size_t GetGridSize() const = 0;

            virtual void SetRenderContext() = 0;

            virtual void RequestNoiseSet(int x, int y, int z, int w, int h, int d, float scale) = 0;
            virtual void FreeNoiseSet() = 0;

            virtual __forceinline std::string GetBiome(const DirectX::SimpleMath::Vector2 &lookup) = 0;
            virtual __forceinline void GetHeight(int index, float &height, DirectX::SimpleMath::Vector2 &biome, std::string &texIndex) = 0;

            virtual ~ISphericalTerrain() {}
    };
}