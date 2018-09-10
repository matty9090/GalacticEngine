#include "pch.h"
#include "GalacticEngine.hpp"

using Microsoft::WRL::ComPtr;

namespace Galactic {
    std::shared_ptr<IBody> CreatePlanet(Microsoft::WRL::ComPtr<ID3D11DeviceContext> deviceContext, std::string name, long double mass, double radius)
    {
        std::shared_ptr<IPlanet> planet = std::make_shared<Planet>(deviceContext, name);
        planet->SetMass(mass);
        planet->SetRadius(radius);

        return planet;
    }
}