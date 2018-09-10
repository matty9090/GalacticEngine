#pragma once

#include <string>
#include <memory>
#include <vector>
#include <wrl/client.h>

#include "Body/IBody.hpp"
#include "Body/IPlanet.hpp"
#include "Body/Planet.hpp"

#include "Render/IRenderable.hpp"

#include "Render/PlanetRendererLow.hpp"
#include "Render/PlanetRendererMed.hpp"
#include "Render/PlanetRendererHigh.hpp"

namespace Galactic {
    std::shared_ptr<Galactic::IBody> CreatePlanet(Microsoft::WRL::ComPtr<ID3D11DeviceContext> deviceContext, std::string name, long double mass, double radius);
}