#include "pch.h"
#include "GalacticEngine.hpp"
#include "Body/Star.hpp"
#include "Body/Planet.hpp"
#include "Noise/Biome.hpp"
#include "Scene/SimpleStarSystem.hpp"
#include "Utils/Json.hpp"

#include <fstream>
#include <set>

using Microsoft::WRL::ComPtr;
using DirectX::SimpleMath::Color;

namespace Galactic {
    void InitEngine()
    {
        std::ifstream f("biomes.json");

        picojson::value json;
        picojson::parse(json, f);

        f.close();

        picojson::object obj = json.get<picojson::object>();

        std::set<std::string> textures;
        std::set<std::string> ntextures;

        size_t numBiomes = 0;

        for (const auto &biome : obj)
        {
            auto arr = biome.second.get("Colour").get<picojson::array>();

            float r = static_cast<float>(arr[0].get<double>());
            float g = static_cast<float>(arr[1].get<double>());
            float b = static_cast<float>(arr[2].get<double>());
            float a = static_cast<float>(arr[3].get<double>());

            DirectX::SimpleMath::Color col(r, g, b, a);

            std::string tex = biome.second.get("Diffuse").get<std::string>();
            std::string norm = biome.second.get("Normal").get<std::string>();

            BiomeConfig::Biomes[biome.first] = { col };

            textures.insert(tex);
            ntextures.insert(norm);

            ++numBiomes;
        }

        for (const auto &biome : obj)
        {
            std::string tex = biome.second.get("Diffuse").get<std::string>();
            std::string norm = biome.second.get("Normal").get<std::string>();

            BiomeConfig::Biomes[biome.first].Tex = static_cast<int>(std::distance(textures.begin(), textures.find(tex)));
            BiomeConfig::Biomes[biome.first].NormalMap = static_cast<int>(std::distance(ntextures.begin(), ntextures.find(norm)));
        }
        
        std::string cmd = "texassemble.exe array -w 1024 -h 1024 -f R32G32B32A32_FLOAT -y -o Resources/Biomes.dds ";
        std::string ncmd = "texassemble.exe array -w 1024 -h 1024 -f R32G32B32A32_FLOAT -y -o Resources/NormalMaps.dds ";

        for (const auto &tex : textures)
            cmd += "Resources/Biomes/" + tex + " ";

        for (const auto &tex : ntextures)
            ncmd += "Resources/Biomes/" + tex + " ";

        system(cmd.c_str());
        system(ncmd.c_str());

        std::cout << "Loaded " << numBiomes << " biomes\n";
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