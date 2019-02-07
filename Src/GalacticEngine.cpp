#include "pch.h"
#include "GalacticEngine.hpp"
#include "Body/Star.hpp"
#include "Body/Planet.hpp"
#include "Noise/Biome.hpp"
#include "Scene/SimpleStarSystem.hpp"

using Microsoft::WRL::ComPtr;
using DirectX::SimpleMath::Color;

namespace Galactic {
    void InitEngine()
    {
        BiomeConfig::Biomes["Bare"]             = { "test0.png"     , "whiten.png",      { 0.53f, 0.53f, 0.53f } };
        BiomeConfig::Biomes["Beach"]            = { "sand01d.jpg"   , "grass01n.png",    { 0.63f, 0.56f, 0.46f } };
        BiomeConfig::Biomes["Desert"]           = { "sand01d.jpg"   , "grass01n.png",    { 0.79f, 0.82f, 0.61f } };
        BiomeConfig::Biomes["DryDesert"]        = { "sand01d.jpg"   , "grass01n.png",    { 0.82f, 0.72f, 0.54f } };
        BiomeConfig::Biomes["Forest"]           = { "grass03d.jpg"  , "grass01n.png",    { 0.40f, 0.59f, 0.35f } };
        BiomeConfig::Biomes["GrassLand"]        = { "grass01d.jpg"  , "grass01n.png",    { 0.53f, 0.67f, 0.33f } };
        BiomeConfig::Biomes["Ocean"]            = { "white.png"     , "water.png",       { 0.26f, 0.26f, 0.48f } };
        BiomeConfig::Biomes["Tundra"]           = { "grass03d.jpg"  , "grass01n.png",    { 0.73f, 0.73f, 0.67f } };
        BiomeConfig::Biomes["Scorched"]         = { "rock01d.png"   , "rock01n.png",     { 0.33f, 0.33f, 0.33f } };
        BiomeConfig::Biomes["Snowy"]            = { "snow02d.jpg"   , "snow02n.jpg",     { 0.86f, 0.86f, 0.89f } };
        BiomeConfig::Biomes["ShrubLand"]        = { "grass02d.jpg"  , "grass02n.jpg",    { 0.53f, 0.67f, 0.46f } };
        BiomeConfig::Biomes["Taiga"]            = { "snow02d.jpg"   , "snow02n.jpg",     { 0.60f, 0.67f, 0.46f } };
        BiomeConfig::Biomes["TempRainForest"]   = { "grass01d.jpg"  , "grass01n.png",    { 0.26f, 0.53f, 0.33f } };
        BiomeConfig::Biomes["TropForest"]       = { "grass01d.jpg"  , "grass01n.png",    { 0.33f, 0.60f, 0.26f } };
        BiomeConfig::Biomes["TropRainForest"]   = { "grass01d.jpg"  , "grass01n.png",    { 0.20f, 0.46f, 0.33f } };
    }

    void CancelCurrentGeneration()
    {
        SphericalQuadTreeTerrain::CancelGeneration = true;
    }

    void SetSplitDistance(float d) { TerrainNode::SplitDistance = d; }
    float GetSplitDistance() { return TerrainNode::SplitDistance; }

    void SetGridSize(size_t gs) { SphericalQuadTreeTerrain::GridSize = gs; }
    size_t GetGridSize() { return SphericalQuadTreeTerrain::GridSize; }

    void SetSplitsPerFrame(size_t splits) { SphericalQuadTreeTerrain::MaxSplitsPerFrame = splits; }
    size_t GetSplitsPerFrame() { return SphericalQuadTreeTerrain::MaxSplitsPerFrame;  }

    std::unique_ptr<IStarSystem> CreateStarSystem(std::string name, EStarSystem type)
    {
        type;
        std::unique_ptr<IStarSystem> system = std::make_unique<SimpleStarSystem>(name);

        return system;
    }

    std::unique_ptr<IStar> CreateStar(ID3D11DeviceContext *deviceContext, std::string name)
    {
        std::unique_ptr<IStar> star = std::make_unique<Star>(name, deviceContext);

        return star;
    }

    std::unique_ptr<IPlanet> CreatePlanet(ID3D11DeviceContext *deviceContext, std::string name)
    {
        return std::make_unique<Planet>(deviceContext, name);
    }

    std::unique_ptr<Galactic::FlatEarth> CreateFlatEarth(ID3D11DeviceContext *deviceContext, std::string name, long double mass, double radius)
    {
        std::unique_ptr<Galactic::FlatEarth> fe = std::make_unique<Galactic::FlatEarth>(deviceContext, name);
        fe->SetMass(mass);
        fe->SetRadius(radius);

        return fe;
    }
}