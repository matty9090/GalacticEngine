#pragma once

#include "Body/IBody.hpp"
#include "Render/IRenderable.hpp"

namespace Galactic
{
    class IStar : public IBody
    {
        public:
            virtual void SetRadius(double radius) = 0;
            virtual void SetTemperature(int kelvin) = 0;

            virtual double GetRadius() const = 0;
            virtual int GetTemperature() const = 0;
    };
}