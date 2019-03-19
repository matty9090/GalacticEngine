#pragma once

#include "IBody.hpp"
#include "Render/Gradient.hpp"
#include "Noise/Biome.hpp"

namespace Galactic
{
    /**
     * @brief Structure to hold planet properties
     * 
     */
    struct PlanetSettings
    {
        /// Grid size
        size_t GridSize;

        /// Seed to use in noise generation
        int Seed;

        /// Minimum height value of terrain
        float MinValue;

        /// Radius of planet
        double Radius;

        /// Mass of planet
        double Mass;

        /// Atmosphere height
        double AtmHeight;

        /// Atmosphere radius (usually same as planet radius)
        double AtmRadius;

        /// Colour of the atmosphere
        DirectX::SimpleMath::Color AtmColour;

        /**
         * @brief Describes a biome map
         * 
         */
        struct Map
        {
            /// The relative strength of noise from each layer when compared to the last
            float Gain;

            /// Height modifier overall
            float Height;

            /// Affects how coarse the noise output is
            float Frequency;

            /// The frequency multiplier between each octave
            float Lacunarity;

            /// Height modifier between octaves
            float Mod;

            /// The amount of noise layers used to create the fractal
            unsigned int Octaves;
        };

        /// Biome settings
        BiomeConfig Biomes;

        /// Biome noise settings
        Map BiomeMap;

        /// List of noisemaps
        std::vector<Map> NoiseMaps;
    };

    /**
     * @brief IPlanet interface
     * 
     */
    class IPlanet : public IBody
    {
        public:
            /**
             * @brief Get the planet settings
             * 
             * @return PlanetSettings& 
             */
            virtual PlanetSettings &GetSettings() = 0;

            /**
             * @brief Read planet settings from JSON file
             * 
             * @param file 
             */
            virtual void ReadSettings(std::string file) = 0;

            /**
             * @brief Set the planet settings
             *
             */
            virtual void SetSettings(PlanetSettings settings) = 0;

            /**
             * @brief Set the radius
             * 
             * @param r 
             */
            virtual void SetRadius(double r) = 0;

            /**
             * @brief Get the radius
             * 
             * @return double 
             */
            virtual double GetRadius() const = 0;

            /**
             * @brief Set the mass
             * 
             * @param m 
             */
            virtual void SetMass(long double m) = 0;

            /**
             * @brief Get the mass
             * 
             * @return long double 
             */
            virtual long double GetMass() const = 0;

            /**
             * @brief Enable the water
             * 
             * @param enabled 
             */
            virtual void EnableWater(bool enabled) = 0;

            /**
             * @brief Enable the cloud layer
             * 
             * @param enabled 
             */
            virtual void EnableClouds(bool enabled) = 0;

            /**
             * @brief Enable the atmosphere
             * 
             * @param enabled 
             */
            virtual void EnableAtmosphere(bool enabled) = 0;

            /**
             * @brief Return whether the water is enabled or not
             * 
             * @return bool enabled
             */
            virtual bool IsWaterEnabled() const = 0;

            /**
             * @brief Return whether the clouds are enabled or not
             * 
             * @return bool enabled
             */
            virtual bool IsCloudsEnabled() const = 0;

            /**
             * @brief Return whether the atmosphere is enabled or not
             * 
             * @return bool enabled
             */
            virtual bool IsAtmosphereEnabled() const = 0;

            /**
             * @brief Increment vertices of planet (internally used by TerrainNode)
             *
             */
            virtual void IncrementVertices(int num) = 0;

            /**
             * @brief Get the vertex count of the planet's geometry
             * 
             * @return size_t 
             */
            virtual size_t GetVertexCount() = 0;

            /**
             * @brief Get a point of the surface of the planet at a specified normal on the original sphere
             * 
             * @param normal 
             * @return DirectX::SimpleMath::Vector3 
             */
            virtual DirectX::SimpleMath::Vector3 GetPoint(DirectX::SimpleMath::Vector3 normal) = 0;

            /**
             * @brief Destroy the IPlanet object
             * 
             */
            virtual ~IPlanet() {}
    };
}