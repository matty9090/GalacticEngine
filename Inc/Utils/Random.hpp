#pragma once

#include <chrono>
#include <random>

namespace Galactic
{
    namespace Utils
    {
        float Rand(float min, float max)
        {
            static std::mt19937 m_gen((unsigned int)std::chrono::system_clock::now().time_since_epoch().count());

            std::uniform_real_distribution<double> dist(min, max);

            return (float)dist(m_gen);
        }
    }
}