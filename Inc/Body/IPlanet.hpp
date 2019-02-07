#pragma once

#include "IBody.hpp"
#include "Render/Gradient.hpp"
#include "Render/GrassDistributor.hpp"
#include "Noise/Biome.hpp"

namespace Galactic
{
    struct PlanetSettings
    {
        size_t GridSize;

        int Seed;
        float MinValue;

        double Radius;
        double Mass;
        double AtmHeight;
        double AtmRadius;

        DirectX::SimpleMath::Color AtmColour;

        struct Map
        {
            float Gain;
            float Height;
            float Frequency;
            float Lacunarity;
            float Mod;

            size_t Octaves;
        };

        BiomeConfig Biomes;
        Map BiomeMap;
        std::vector<Map> NoiseMaps;
    };

    class IPlanet : public IBody
    {
        public:
            virtual PlanetSettings &GetSettings() = 0;

            virtual void ReadSettings(std::string file) = 0;

            virtual void SetRadius(double r) = 0;
            virtual double GetRadius() const = 0;

            virtual void SetMass(long double m) = 0;
            virtual long double GetMass() const = 0;

            virtual void EnableClouds(bool enabled) = 0;
            virtual void EnableAtmosphere(bool enabled) = 0;

            virtual bool IsCloudsEnabled() const = 0;
            virtual bool IsAtmosphereEnabled() const = 0;

            virtual void IncrementVertices(size_t num) = 0;
            virtual size_t GetVertexCount() = 0;
            virtual DirectX::SimpleMath::Vector3 GetPoint(DirectX::SimpleMath::Vector3 normal) = 0;
            virtual GrassDistributor &GetGrassDistributor() = 0;

            virtual ~IPlanet() {}
    };
}