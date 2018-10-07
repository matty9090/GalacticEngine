#include "pch.h"
#include "Noise/Biome.hpp"

using namespace Galactic;

std::map<EBiomes, NoiseParameters> Galactic::Biomes;

void Biome::SetNoiseParams()
{
	m_noise.SetFractalOctaves(m_params.octaves);
	m_noise.SetFractalGain(m_params.gain);
	m_noise.SetFractalLacunarity(m_params.lacunarity);
	m_noise.SetFrequency(m_params.frequency);
}

float Biome::GetHeight(float x, float y, float z)
{
	SetNoiseParams();
	return m_noise.GetNoise(x, y, z) * m_params.height;
}

DirectX::SimpleMath::Color Biome::GetColour(float h)
{
	h /= m_params.height;

	if (h < 0.1f)
		return DirectX::SimpleMath::Color(0.0f, 0.0f, 1.0f);

	if (h < 0.12f)
		return DirectX::SimpleMath::Color(1.0f, 1.0f, 0.0f);

	return m_params.colour;
}

float OceanBiome::GetHeight(float x, float y, float z)
{
	return 0;
}