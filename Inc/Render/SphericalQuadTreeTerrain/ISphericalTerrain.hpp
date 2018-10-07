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

            virtual void SetRenderContext() = 0;
            virtual void GetHeight(DirectX::SimpleMath::Vector3 point, float &height, DirectX::SimpleMath::Color &col) = 0;

			virtual ~ISphericalTerrain() {}
    };
}