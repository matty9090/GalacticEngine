#pragma once

#include "IBody.hpp"

namespace Galactic
{
    class IPlanet : public IBody
    {
        public:
            virtual void SetRadius(double r) = 0;
    };
}