#pragma once

#include <memory>
#include <wrl/client.h>

#include "PlanetRendererLow.hpp"
#include "PlanetRendererMed.hpp"
#include "PlanetRendererHigh.hpp"

namespace Galactic {
    std::shared_ptr<Galactic::IPlanetRenderer> CreatePlanetRenderer(Microsoft::WRL::ComPtr<ID3D11DeviceContext> deviceContext, EDetail detail)
    {
        std::shared_ptr<IPlanetRenderer> renderer;

        switch (detail) {
            case EDetail::Low:
                renderer = std::make_shared<LowDetailPlanetRenderer>(deviceContext);
                break;

            case EDetail::Medium:
                renderer = std::make_shared<MedDetailPlanetRenderer>(deviceContext);
                break;

            case EDetail::High:
                renderer = std::make_shared<HighDetailPlanetRenderer>(deviceContext);
                break;
        }

        return renderer;
    }
}