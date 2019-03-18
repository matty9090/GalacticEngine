#pragma once

#include <string>
#include <map>
#include <unordered_map>

#include "FastNoise.h"
#include "Utils/Json.hpp"
#include "Render/Gradient.hpp"

namespace Galactic
{
    /**
     * @brief Describes a biome
     * 
     */
    struct Biome
    {
        /// Colour of the biome
        DirectX::SimpleMath::Color Colour;

        /// Index of the diffuse map in the texture array
        int Tex;

        /// Index of the normal map in the texture array
        int NormalMap;
    };

    /**
     * @brief Biome config
     * 
     */
    class BiomeConfig
    {
        public:
            /**
             * @brief Describes a row in the biome lookup texture
             * 
             */
            struct Row
            {
                friend class BiomeConfig;

                public:
                    /**
                     * @brief Add a biome to the row
                     * 
                     * @param moisture 
                     * @param name 
                     * @return Row& 
                     */
                    Row &AddBiome(float moisture, std::string name)
                    {
                        moisture = (moisture > 1.0f) ? 1.0f : moisture;
                        biomes[moisture] = name;
                        return *this;
                    }

                    /**
                     * @brief Get the number of biomes in the row
                     * 
                     * @return size_t 
                     */
                    size_t GetCount() { return biomes.size(); }

                    /**
                     * @brief Get all biomes in the row
                     * 
                     * @return std::map<float, std::string> 
                     */
                    std::map<float, std::string> GetCols() { return biomes; }

                private:
                    std::map<float, std::string> biomes;
            };

            /**
             * @brief Construct a new Biome Config object
             * 
             */
            BiomeConfig() : m_pixels(NULL), m_pixelBiomes(NULL) {}

            /**
             * @brief Destroy the Biome Config object
             * 
             */
            ~BiomeConfig();

            /**
             * @brief Generate the biome lookup texture
             * 
             * @param device 
             * @param srv Shader resource view
             * @param width of the texture
             * @param height of the texture
             */
            void Generate(ID3D11Device *device, ID3D11ShaderResourceView **srv, unsigned int width, unsigned int height);

            /**
             * @brief Add a biome row to the lookup texture
             * 
             * @param row 
             * @param elevation 
             */
            void AddBiomeRow(Row row, float elevation);

            /**
             * @brief Get all the rows in the biome lookup texture
             * 
             * @return std::map<float, Row> 
             */
            std::map<float, Row> GetRows() { return m_biomes; }

            /**
             * @brief Free all data
             * 
             */
            void Clear();

            /**
             * @brief Clear all biome rows
             * 
             */
            void ClearBiomes() { m_biomes.clear(); }

            /**
             * @brief Sample the biome lookup texture at a specific elevation and moisture level
             * 
             * @param moisture 
             * @param elevation 
             * @return std::string biome name
             */
            std::string Sample(float moisture, float elevation);

            /**
             * @brief Associative list of biome names with their corresponding settings
             * 
             */
            static std::unordered_map<std::string, Biome> Biomes;

        private:
            std::map<float, Row> m_biomes;

            ID3D11Texture2D *m_tex;
            float **m_pixels;
            std::string **m_pixelBiomes;

            unsigned int m_width, m_height;
    };
}