#pragma once

#include <GeometricPrimitive.h>

#include "IRenderable.hpp"

namespace Galactic
{
    /**
     * @brief IStar renderer interface
     * 
     */
    class IStarRenderer : public IRenderable {
        public:
            /**
             * @brief Destroy the IStarRenderer object
             * 
             */
            virtual ~IStarRenderer() {}
    };
}
