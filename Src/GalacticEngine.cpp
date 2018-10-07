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
		Biomes[EBiomes::Grass] = { 10, 0.45f, 1.9f, 0.04f, 0.04f, 1.0f, Color(0.0f, 1.0f, 0.0f) };
		Biomes[EBiomes::Desert] = { 10, 0.45f, 1.9f, 0.04f, 0.04f, 1.0f, Color(1.0f, 1.0f, 0.0f) };
		Biomes[EBiomes::Mountains] = { 10, 0.6f, 1.9f, 0.04f, 0.04f, 1.0f, Color(0.6f, 0.6f, 0.6f) };
		Biomes[EBiomes::Ocean].colour = Color(0.0f, 0.0f, 1.0f);
	}

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
}