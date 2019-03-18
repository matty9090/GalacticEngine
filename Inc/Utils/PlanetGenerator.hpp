#pragma once

#include <d3d11.h>
#include <map>

#include "Body/IPlanet.hpp"
#include "Render/Gradient.hpp"

namespace Galactic
{
    /**
     * @brief Randomly generate planets
     * 
     */
    class PlanetGenerator
    {
        public:
            /**
             * @brief Construct a new Planet Generator object
             * 
             * @param context 
             */
            PlanetGenerator(ID3D11DeviceContext *context);

            /**
             * @brief Create a rocky planet
             * 
             * @param name 
             * @param mass 
             * @param radius 
             * @return std::unique_ptr<IPlanet> 
             */
            std::unique_ptr<IPlanet> CreateRocky(std::string name, double mass, double radius);

            /**
             * @brief Create a gas giant planet
             * 
             * @param name 
             * @param mass 
             * @param radius 
             * @return std::unique_ptr<IPlanet> 
             */
            std::unique_ptr<IPlanet> CreateGasGiant(std::string name, double mass, double radius);

        private:
            ID3D11DeviceContext *m_context;

            std::vector<Gradient::Gradient<Gradient::GradientColor>> m_palettes;

            Gradient::GradientColor RandomColour();
            Gradient::Gradient<Gradient::GradientColor> RandomPalette();
    };
}