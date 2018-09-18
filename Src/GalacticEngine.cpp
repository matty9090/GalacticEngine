#include "pch.h"
#include "GalacticEngine.hpp"
#include "Body/Planet.hpp"
#include "Scene/SimpleStarSystem.hpp"

using Microsoft::WRL::ComPtr;

namespace Galactic {
    std::unique_ptr<IStarSystem> CreateStarSystem(std::string name, EStarSystem type)
    {
        std::unique_ptr<IStarSystem> system = std::make_unique<SimpleStarSystem>(name);

        return system;
    }

    std::unique_ptr<IPlanet> CreatePlanet(Microsoft::WRL::ComPtr<ID3D11DeviceContext> deviceContext, std::string name, long double mass, double radius)
    {
        std::unique_ptr<IPlanet> planet = std::make_unique<Planet>(deviceContext, name);
        planet->SetMass(mass);
        planet->SetRadius(radius);

        return planet;
    }
}