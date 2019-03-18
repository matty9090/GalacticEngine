#pragma once

#include <chrono>
#include <random>

namespace Galactic
{
    namespace Utils
    {
        /**
         * @brief Generate a random number
         * 
         * @param min 
         * @param max 
         * @return float 
         */
        float Rand(float min, float max)
        {
            // TODO: Shouldn't seed every time
            static std::mt19937 m_gen((unsigned int)std::chrono::system_clock::now().time_since_epoch().count());

            std::uniform_real_distribution<double> dist(min, max);

            return (float)dist(m_gen);
        }
    }
}