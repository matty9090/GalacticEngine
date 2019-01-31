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

    std::unique_ptr<IPlanet> CreatePlanet(ID3D11DeviceContext *deviceContext, std::string name, long double mass, double radius)
    {
        std::unique_ptr<IPlanet> planet = std::make_unique<Planet>(deviceContext, name);
        planet->SetMass(mass);
        planet->SetRadius(radius);

        return planet;
    }

    std::unique_ptr<Galactic::FlatEarth> CreateFlatEarth(ID3D11DeviceContext *deviceContext, std::string name, long double mass, double radius)
    {
        std::unique_ptr<Galactic::FlatEarth> fe = std::make_unique<Galactic::FlatEarth>(deviceContext, name);
        fe->SetMass(mass);
        fe->SetRadius(radius);

        return fe;
    }
}