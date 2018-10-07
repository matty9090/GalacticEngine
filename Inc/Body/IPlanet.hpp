#pragma once

#include "IBody.hpp"
#include "Render/Gradient.hpp"

namespace Galactic
{
    class IPlanet : public IBody
    {
        public:
			virtual void SetParam(std::string name, float value) = 0;
			virtual float GetParam(std::string name) = 0;

			virtual void SetSeed(int seed) = 0;
			virtual int GetSeed() = 0;

            virtual void SetRadius(double r) = 0;
            virtual double GetRadius() const = 0;

            virtual void SetAtmosphereHeight(float h) = 0;
            virtual void SetAtmosphereColour(DirectX::SimpleMath::Color colour) = 0;

            virtual float GetAtmosphereHeight() const = 0;
            virtual DirectX::SimpleMath::Color GetAtmosphereColour() const = 0;

			virtual ~IPlanet() {}
    };
}