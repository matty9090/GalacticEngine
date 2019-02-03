#pragma once

#include <map>
#include <string>

#include "FastNoise.h"
#include "Render/Gradient.hpp"

namespace Galactic
{
    class BiomeConfig
    {
        public:
            struct Row
            {
                friend class BiomeConfig;

                public:
                    Row &AddBiome(float moisture, DirectX::SimpleMath::Color colour)
                    {
                        moisture = (moisture > 1.0f) ? 1.0f : moisture;
                        biomes[moisture] = colour;
                        return *this;
                    }

                    size_t GetCount() { return biomes.size(); }

                private:
                    std::map<float, DirectX::SimpleMath::Color> biomes;
            };

            BiomeConfig() {}

            void Generate(ID3D11Device *device, ID3D11Texture2D **tex, ID3D11ShaderResourceView **srv, size_t width, size_t height);
            void AddBiomeRow(Row row, float elevation);

        private:
            std::map<float, Row> biomes;
    };

    namespace Biomes
    {
        namespace
        {
            DirectX::SimpleMath::Color Scorched         = { 0.33f, 0.33f, 0.33f };
            DirectX::SimpleMath::Color Bare             = { 0.53f, 0.53f, 0.53f };
            DirectX::SimpleMath::Color Tundra           = { 0.73f, 0.73f, 0.67f };
            DirectX::SimpleMath::Color Snowy            = { 0.86f, 0.86f, 0.89f };
            DirectX::SimpleMath::Color Desert           = { 0.79f, 0.82f, 0.61f };
            DirectX::SimpleMath::Color ShrubLand        = { 0.53f, 0.67f, 0.46f };
            DirectX::SimpleMath::Color Taiga            = { 0.60f, 0.67f, 0.46f };
            DirectX::SimpleMath::Color GrassLand        = { 0.53f, 0.67f, 0.33f };
            DirectX::SimpleMath::Color Forest           = { 0.40f, 0.59f, 0.35f };
            DirectX::SimpleMath::Color TempRainForest   = { 0.26f, 0.53f, 0.33f };
            DirectX::SimpleMath::Color DryDesert        = { 0.82f, 0.72f, 0.54f };
            DirectX::SimpleMath::Color TropForest       = { 0.33f, 0.60f, 0.26f };
            DirectX::SimpleMath::Color TropRainForest   = { 0.20f, 0.46f, 0.33f };
            DirectX::SimpleMath::Color Beach            = { 0.63f, 0.56f, 0.46f };
            DirectX::SimpleMath::Color Ocean            = { 0.26f, 0.26f, 0.48f };
        }
    }
}