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
}