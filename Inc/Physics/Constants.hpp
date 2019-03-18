#pragma once

namespace Galactic
{
    /**
     * @brief Physics constance
     * 
     */
    namespace Constants {
        /// World scale
        const double Scale = 800.0;

        /// Newtons gravitational constant
        const double G = 6.674e-11;

        /// Rayleigh scattering constant
        const float Kr = 0.0025f;

        /// Mie scattering constant
        const float Km = 0.0010f;

        /// Atmospheric scattering constant
        const float Af = -0.990f;

        /// Mie phase asymmetry factor
        const float ESun = 15.0f;
    }
}