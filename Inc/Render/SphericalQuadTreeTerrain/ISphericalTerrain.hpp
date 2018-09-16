#pragma once

#include "Render/IRenderable.hpp"
#include "Render/DirectX/Effect.hpp"
#include "FastNoise/FastNoise.h"

namespace Galactic
{
    class ISphericalTerrain : public IRenderable
    {
        public:
            virtual float GetRadius() const = 0;
            virtual int GetGridSize() const = 0;

            virtual std::shared_ptr<Effect> GetEffect() const = 0;
            virtual Microsoft::WRL::ComPtr<ID3D11DeviceContext> GetContext() const = 0;

            virtual void SetRenderContext() = 0;
            virtual float GetHeight(DirectX::SimpleMath::Vector3 point) = 0;
    };
}