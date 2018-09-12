#pragma once

#include "Render/IRenderable.hpp"
#include "Render/DirectX/Effect.hpp"

namespace Galactic
{
    class ISphericalTerrain : public IRenderable
    {
        public:
            virtual float GetRadius() const = 0;
            virtual int GetGridSize() const = 0;

            virtual std::shared_ptr<Effect> GetEffect() const = 0;
            virtual Microsoft::WRL::ComPtr<ID3D11DeviceContext> GetContext() const = 0;
    };
}