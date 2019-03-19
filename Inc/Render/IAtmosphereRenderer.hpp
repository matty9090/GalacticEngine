#pragma once

#include <d3d11.h>

#include "IRenderable.hpp"

namespace Galactic {
    /**
     * @brief IAtmosphere renderer interface
     * 
     */
    class IAtmosphereRenderer : public IRenderable
    {
        public:
            /**
             * @brief Destroy the IAtmosphereRenderer object
             * 
             */
            virtual ~IAtmosphereRenderer() {}
    };
}