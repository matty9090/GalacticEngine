#pragma once

#include <GeometricPrimitive.h>

#include "IRenderable.hpp"

namespace Galactic
{
    class IStarRenderer : public IRenderable {
        public:
            virtual ~IStarRenderer() {}
    };
}
