#pragma once

#include <memory>
#include <wrl/client.h>

#include "SimpleAtmosphere.hpp"
#include "ScatteredAtmosphere.hpp"

#include "PlanetRendererLow.hpp"
#include "PlanetRendererMed.hpp"
#include "PlanetRendererHigh.hpp"

namespace Galactic {
    /**
     * @brief Create a planet renderer
     * 
     * @param deviceContext 
     * @param planet 
     * @param detail 
     * @return std::unique_ptr<IPlanetRenderer> 
     */
    std::unique_ptr<IPlanetRenderer> CreatePlanetRenderer(Microsoft::WRL::ComPtr<ID3D11DeviceContext> deviceContext, IPlanet *planet, EDetail detail)
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

    /**
     * @brief Create an atmosphere renderer
     * 
     * @param deviceContext 
     * @param planet 
     * @param detail 
     * @return std::unique_ptr<IAtmosphereRenderer> 
     */
    std::unique_ptr<IAtmosphereRenderer> CreateAtmosphereRenderer(Microsoft::WRL::ComPtr<ID3D11DeviceContext> deviceContext, IPlanet *planet, EDetail detail)
    {

        std::unique_ptr<IAtmosphereRenderer> renderer;

        switch (detail) {
            case EDetail::Low:
                renderer = std::make_unique<SimpleAtmosphere>(deviceContext.Get(), planet);
                break;

            case EDetail::Medium:
            case EDetail::High:
                renderer = std::make_unique<ScatteredAtmosphere>(deviceContext.Get(), planet);
                break;
        }

        return renderer;
    }
}