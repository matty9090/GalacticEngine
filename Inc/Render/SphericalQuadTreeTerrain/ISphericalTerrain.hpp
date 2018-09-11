#pragma once

#include "Render/IRenderable.hpp"

namespace Galactic
{
    class ISphericalTerrain : public IRenderable
    {
        public:
            virtual float GetRadius() const = 0;
            virtual int GetGridSize() const = 0;
    };
}