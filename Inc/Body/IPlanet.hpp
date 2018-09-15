#pragma once

#include "IBody.hpp"

namespace Galactic
{
    class IPlanet : public IBody
    {
        public:
            virtual void SetParameters(float freq, float lacunarity, float gain, float height, int octaves) = 0;
            virtual void SetRadius(double r) = 0;
            virtual double GetRadius() const = 0;

            virtual int GetOctaves() const = 0;
            virtual float GetGain() const = 0;
            virtual float GetHeight() const = 0;
            virtual float GetFrequency() const = 0;
            virtual float GetLacunarity() const = 0;
    };
}