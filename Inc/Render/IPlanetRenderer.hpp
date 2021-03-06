#pragma once

#include <GeometricPrimitive.h>

#include "IRenderable.hpp"

namespace Galactic
{
    /**
     * @brief IPlanet renderer interface
     * 
     */
    class IPlanetRenderer : public IRenderable {
        public:
            virtual DirectX::SimpleMath::Vector3 GetPoint(DirectX::SimpleMath::Vector3 normal) = 0;
            virtual ~IPlanetRenderer() {}
    };
}
