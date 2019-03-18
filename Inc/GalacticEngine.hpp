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

namespace Galactic
{
    /**
     * @brief Type of star system
     * 
     */
    enum class EStarSystem
    {
        Simple, ///< Each planet has a maximum of one influence
        NBody   ///< NBody simulation (not implemented)
    };

    /**
     * @brief Initialise the engine
     * 
     */
    void InitEngine();

    /**
    * @brief Stop generating the quadtrees for the planet
    * 
    */
    void CancelCurrentGeneration();

    /**
     * @brief Set the Split Distance
     * 
     * @param Distance from camera to terrainnode in which it will split the quadtree
     */
    void SetSplitDistance(float d);

    /**
     * @brief Get the ssplit distance
     * 
     * @return float  Split distance
     */
    float GetSplitDistance();

    /**
     * @brief Set the Grid Size
     * 
     * @param size_t Grid size
     */
    void SetGridSize(size_t gs);

    /**
     * @brief Get the Grid Size
     * 
     * @return size_t Grid size
     */
    size_t GetGridSize();

    /**
     * @brief Set the max quadtree Splits Per Frame
     * 
     * @param Splits per frame
     */
    void SetSplitsPerFrame(size_t splits);

    /**
     * @brief Get the max quadtree Splits Per Frame
     * 
     * @return size_t splits per frame
     */
    size_t GetSplitsPerFrame();

    /**
     * @brief Create a Star System with the specificed type
     * 
     * @param name name of the star system
     * @param type 
     * @return std::unique_ptr<Galactic::IStarSystem> 
     */
    std::unique_ptr<Galactic::IStarSystem> CreateStarSystem(std::string name, EStarSystem type);
    std::unique_ptr<Galactic::IStar> CreateStar(ID3D11DeviceContext *deviceContext, std::string name);
    std::unique_ptr<Galactic::IPlanet> CreatePlanet(ID3D11DeviceContext *deviceContext, std::string name);
    std::unique_ptr<Galactic::FlatEarth> CreateFlatEarth(ID3D11DeviceContext *deviceContext, std::string name, long double mass, double radius);
}
