#pragma once

#include <string>
#include <memory>
#include <vector>
#include <wrl/client.h>

#include "Body/IBody.hpp"
#include "Body/IStar.hpp"
#include "Body/IPlanet.hpp"

#include "Render/IRenderable.hpp"

#include "Render/PlanetRendererLow.hpp"
#include "Render/PlanetRendererMed.hpp"
#include "Render/PlanetRendererHigh.hpp"

#include "Scene/IStarSystem.hpp"
#include "Scene/Camera.hpp"

#include "Physics/Constants.hpp"

namespace Galactic {
    enum class EStarSystem { Simple, NBody };

    std::unique_ptr<Galactic::IStarSystem> CreateStarSystem(std::string name, EStarSystem type);
    std::unique_ptr<Galactic::IStar> CreateStar(ID3D11DeviceContext *deviceContext, std::string name);
    std::unique_ptr<Galactic::IPlanet> CreatePlanet(ID3D11DeviceContext *deviceContext, std::string name, long double mass, double radius);
}