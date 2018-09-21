#include "pch.h"
#include "GalacticEngine.hpp"
#include "Utils/PlanetGenerator.hpp"
#include "Utils/Random.hpp"

using namespace Galactic;

PlanetGenerator::PlanetGenerator(ID3D11DeviceContext *context) : m_context(context)
{
    Gradient::Gradient<Gradient::GradientColor> green;
    green.addColorStop(0.0f, Gradient::GradientColor(0.0f, 119.f, 189.f, 128.f));
    green.addColorStop(0.06f, Gradient::GradientColor(237.f, 201.f, 176.f, 255.f));
    green.addColorStop(0.2f, Gradient::GradientColor(56.f, 158.f, 36.f, 255.f));
    green.addColorStop(1.0f, Gradient::GradientColor(56.f, 158.f, 36.f, 255.f));
    m_palettes.push_back(green);

    Gradient::Gradient<Gradient::GradientColor> brown;
    brown.addColorStop(0.0f, Gradient::GradientColor(51.f, 44.f, 35.f, 255.f));
    brown.addColorStop(0.06f, Gradient::GradientColor(51.f, 51.f, 44.f, 255.f));
    brown.addColorStop(0.2f, Gradient::GradientColor(97.f, 67.f, 68.f, 255.f));
    brown.addColorStop(1.0f, Gradient::GradientColor(122.f, 109.f, 141.f, 255.f));
    m_palettes.push_back(brown);

    Gradient::Gradient<Gradient::GradientColor> blue;
    blue.addColorStop(0.0f, Gradient::GradientColor(112.f, 108.f, 97.f, 255.f));
    blue.addColorStop(0.06f, Gradient::GradientColor(137.f, 158.f, 139.f, 255.f));
    blue.addColorStop(0.2f, Gradient::GradientColor(153.f, 197.f, 181.f, 255.f));
    blue.addColorStop(1.0f, Gradient::GradientColor(175.f, 236.f, 231.f, 255.f));
    m_palettes.push_back(blue);
   
    for (size_t i = 0; i < 20; ++i)
        m_palettes.push_back(RandomPalette());
}

std::unique_ptr<IPlanet> PlanetGenerator::CreateRocky(std::string name, double mass, double radius)
{
    auto planet = CreatePlanet(m_context, name, mass, radius);
    planet->SetHeight(Utils::Rand(0.2f, 0.8f) / 10.0f);
    planet->SetFrequency(Utils::Rand(0.1f, 0.5f) / 10.0f);
    planet->SetLacunarity(Utils::Rand(1.6f, 2.5f));
    planet->SetGain(Utils::Rand(0.0f, 2.0f) / 10.0f + 0.45f);
    planet->SetNoiseScale(Utils::Rand(0.4f, 1.8f));
    planet->SetMinValue(Utils::Rand(0.0f, 2.0f) / 100.0f - 0.0011f);
    planet->SetPalette(m_palettes[(int)round(Utils::Rand(0.0f, (float)(m_palettes.size() - 1)))]);

    return planet;
}

std::unique_ptr<IPlanet> PlanetGenerator::CreateGasGiant(std::string name, double mass, double radius)
{
    auto col = RandomColour();
    auto palette = Gradient::Gradient<Gradient::GradientColor>();
    palette.addColorStop(0.0f, col);

    auto planet = CreatePlanet(m_context, name, mass, radius);
    planet->SetHeight(0.0f);
    planet->SetOctaves(0);
    planet->SetPalette(palette);
    planet->SetAtmosphereColour(DirectX::SimpleMath::Color(col.r, col.g, col.b));

    return planet;
}

Gradient::GradientColor Galactic::PlanetGenerator::RandomColour() {
    float r = Utils::Rand(0.0f, 255.0f);
    float g = Utils::Rand(0.0f, 255.0f);
    float b = Utils::Rand(0.0f, 255.0f);

    return Gradient::GradientColor(r, g, b, 255.0f);
}

Gradient::Gradient<Gradient::GradientColor> Galactic::PlanetGenerator::RandomPalette()
{
    Gradient::Gradient<Gradient::GradientColor> gradient;

    size_t stops = (size_t)round(Utils::Rand(3.0f, 6.0f));

    for (int i = 0; i < stops; ++i)
    {
        float loc = Utils::Rand(0.0, 1.0f);

        if (i == 0) loc = 0.0f; 
        if (i == stops - 1) loc = 1.0f;

        gradient.addColorStop(loc, RandomColour());
    }

    return gradient;
}
