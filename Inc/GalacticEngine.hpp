#pragma once

#include <string>
#include <memory>
#include <vector>
#include <wrl/client.h>

#include "CommonStates.h"
#include "DDSTextureLoader.h"
#include "DirectXHelpers.h"
#include "Effects.h"
#include "GamePad.h"
#include "GeometricPrimitive.h"
#include "GraphicsMemory.h"
#include "Keyboard.h"
#include "Model.h"
#include "Mouse.h"
#include "PostProcess.h"
#include "PrimitiveBatch.h"
#include "ScreenGrab.h"
#include "SimpleMath.h"
#include "SpriteBatch.h"
#include "SpriteFont.h"
#include "VertexTypes.h"
#include "WICTextureLoader.h"

#include "Body/IBody.hpp"
#include "Body/IStar.hpp"
#include "Body/IPlanet.hpp"
#include "Body/FlatEarth.hpp"

#include "Render/IRenderable.hpp"

#include "Render/PlanetRendererLow.hpp"
#include "Render/PlanetRendererMed.hpp"
#include "Render/PlanetRendererHigh.hpp"

#include "Scene/IStarSystem.hpp"
#include "Scene/Camera.hpp"

#include "Utils/PlanetGenerator.hpp"
#include "Physics/Constants.hpp"

namespace Galactic {
    enum class EStarSystem { Simple, NBody };

    void InitEngine();

    void CancelCurrentGeneration();

    void SetSplitDistance(float d);
    float GetSplitDistance();

    void SetGridSize(size_t gs);
    size_t GetGridSize();

    void SetSplitsPerFrame(size_t splits);
    size_t GetSplitsPerFrame();

    std::unique_ptr<Galactic::IStarSystem> CreateStarSystem(std::string name, EStarSystem type);
    std::unique_ptr<Galactic::IStar> CreateStar(ID3D11DeviceContext *deviceContext, std::string name);
    std::unique_ptr<Galactic::IPlanet> CreatePlanet(ID3D11DeviceContext *deviceContext, std::string name);
    std::unique_ptr<Galactic::FlatEarth> CreateFlatEarth(ID3D11DeviceContext *deviceContext, std::string name, long double mass, double radius);
}
