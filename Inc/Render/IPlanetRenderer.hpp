#pragma once

#include <GeometricPrimitive.h>

#include "IRenderable.hpp"

namespace Galactic
{
    class IPlanetRenderer : public IRenderable {
        public:
            virtual DirectX::SimpleMath::Matrix &GetMatrix() = 0;
    };
}
