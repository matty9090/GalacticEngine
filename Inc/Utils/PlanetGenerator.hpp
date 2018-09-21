#pragma once

#include <d3d11.h>
#include <map>

#include "Body/IPlanet.hpp"
#include "Render/Gradient.hpp"

namespace Galactic
{
    class PlanetGenerator
    {
        public:
            PlanetGenerator(ID3D11DeviceContext *context);

            std::unique_ptr<IPlanet> CreateRocky(std::string name, double mass, double radius);

        private:
            ID3D11DeviceContext *m_context;

            std::vector<Gradient::Gradient<Gradient::GradientColor>> m_palettes;

            Gradient::Gradient<Gradient::GradientColor> RandomPalette();
    };
}