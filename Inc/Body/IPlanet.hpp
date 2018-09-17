#pragma once

#include "IBody.hpp"
#include "Render/Gradient.hpp"

namespace Galactic
{
    class IPlanet : public IBody
    {
        public:
            virtual void SetParameters(float freq, float lacunarity, float gain, float height, int octaves, float noiseScale) = 0;
            virtual void SetRadius(double r) = 0;
            virtual double GetRadius() const = 0;

            virtual void SetOctaves(int v) = 0;
            virtual void SetGain(float v) = 0;
            virtual void SetHeight(float v) = 0;
            virtual void SetFrequency(float v) = 0;
            virtual void SetLacunarity(float v) = 0;
            virtual void SetNoiseScale(float v) = 0;
            virtual void SetMinValue(float v) = 0;

            virtual int GetOctaves() const = 0;
            virtual float GetGain() const = 0;
            virtual float GetHeight() const = 0;
            virtual float GetFrequency() const = 0;
            virtual float GetLacunarity() const = 0;
            virtual float GetNoiseScale() const = 0;
            virtual float GetMinValue() const = 0;

            virtual Gradient::Gradient<Gradient::GradientColor> &GetGradient() = 0;
    };
}