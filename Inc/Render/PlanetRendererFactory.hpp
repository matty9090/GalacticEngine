#pragma once

#include <memory>
#include <wrl/client.h>

#include "SimpleAtmosphere.hpp"

#include "PlanetRendererLow.hpp"
#include "PlanetRendererMed.hpp"
#include "PlanetRendererHigh.hpp"

namespace Galactic {
    std::unique_ptr<Galactic::IPlanetRenderer> CreatePlanetRenderer(Microsoft::WRL::ComPtr<ID3D11DeviceContext> deviceContext, std::shared_ptr<IPlanet> planet, EDetail detail)
    {
        std::unique_ptr<IPlanetRenderer> renderer;

        switch (detail) {
            case EDetail::Low:
                renderer = std::make_unique<LowDetailPlanetRenderer>(deviceContext, planet);
                break;

            case EDetail::Medium:
                renderer = std::make_unique<MedDetailPlanetRenderer>(deviceContext, planet);
                break;

            case EDetail::High:
                renderer = std::make_unique<HighDetailPlanetRenderer>(deviceContext, planet);
                break;
        }

        return renderer;
    }

    std::unique_ptr<Galactic::IAtmosphereRenderer> CreateAtmosphereRenderer(Microsoft::WRL::ComPtr<ID3D11DeviceContext> deviceContext, std::shared_ptr<IPlanet> planet)
    {
        return std::make_unique<SimpleAtmosphere>(deviceContext.Get(), planet);
    }
}