#pragma once

#include <memory>
#include <wrl/client.h>

#include "PlanetRendererLow.hpp"
#include "PlanetRendererMed.hpp"
#include "PlanetRendererHigh.hpp"

namespace Galactic {
    std::shared_ptr<Galactic::IPlanetRenderer> CreatePlanetRenderer(Microsoft::WRL::ComPtr<ID3D11DeviceContext> deviceContext, std::shared_ptr<IPlanet> planet, EDetail detail)
    {
        std::shared_ptr<IPlanetRenderer> renderer;

        switch (detail) {
            case EDetail::Low:
                renderer = std::make_shared<LowDetailPlanetRenderer>(deviceContext, planet);
                break;

            case EDetail::Medium:
                renderer = std::make_shared<MedDetailPlanetRenderer>(deviceContext, planet);
                break;

            case EDetail::High:
                renderer = std::make_shared<HighDetailPlanetRenderer>(deviceContext, planet);
                break;
        }

        return renderer;
    }
}