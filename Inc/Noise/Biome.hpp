#pragma once

#include <map>
#include <string>

#include "FastNoise.h"
#include "Render/Gradient.hpp"

namespace Galactic
{
    enum class EBiomes { Grass, Mountains, Desert, Ocean, Beach };

    struct NoiseParameters
    {
        int octaves;
        float gain;
        float lacunarity;
        float frequency;
        float height;
        float scale;

        DirectX::SimpleMath::Color colour;
    };

    extern std::map<EBiomes, NoiseParameters> Biomes;

    class Biome
    {
        public:
            Biome(FastNoise &noise, NoiseParameters params) : m_noise(noise), m_params(params) {}

            virtual float GetHeight(float x, float y, float z);
            virtual DirectX::SimpleMath::Color GetColour(float h);

        protected:
            FastNoise &m_noise;
            NoiseParameters m_params;

            void SetNoiseParams();
    };

    class OceanBiome : public Biome
    {
        public:
            OceanBiome(FastNoise &noise, NoiseParameters params) : Biome(noise, params) {}

            float GetHeight(float x, float y, float z);
    };
}