#pragma once

#include <string>
#include <map>
#include <unordered_map>

#include "FastNoise.h"
#include "Utils/Json.hpp"
#include "Render/Gradient.hpp"

namespace Galactic
{
    struct Biome
    {
        std::string Texture, NormalMap;
        DirectX::SimpleMath::Color Colour;
    };

    class BiomeConfig
    {
        public:
            struct Row
            {
                friend class BiomeConfig;

                public:
                    Row &AddBiome(float moisture, std::string name)
                    {
                        moisture = (moisture > 1.0f) ? 1.0f : moisture;
                        biomes[moisture] = name;
                        return *this;
                    }

                    size_t GetCount() { return biomes.size(); }
                    std::map<float, std::string> GetCols() { return biomes; }

                private:
                    std::map<float, std::string> biomes;
            };

            BiomeConfig() : m_pixels(NULL), m_pixelBiomes(NULL) {}
            ~BiomeConfig();

            void Generate(ID3D11Device *device, ID3D11ShaderResourceView **srv, size_t width, size_t height);
            void AddBiomeRow(Row row, float elevation);
            std::map<float, Row> GetRows() { return m_biomes; }

            void Clear();
            void ClearBiomes() { m_biomes.clear(); }

            std::string Sample(float moisture, float elevation);

            static std::unordered_map<std::string, Biome> Biomes;

        private:
            std::map<float, Row> m_biomes;

            ID3D11Texture2D *m_tex;
            float **m_pixels;
            std::string **m_pixelBiomes;

            size_t m_width, m_height;
    };
}