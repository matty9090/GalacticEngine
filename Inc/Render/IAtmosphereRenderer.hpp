#pragma once

#include <d3d11.h>

#include "IRenderable.hpp"

namespace Galactic {
    class IAtmosphereRenderer : public IRenderable
    {
        public:
            virtual ~IAtmosphereRenderer() {}
    };
}