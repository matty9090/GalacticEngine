#pragma once

#include "Body/IBody.hpp"

namespace Galactic
{
    namespace Gravity
    {
        /**
         * @brief Get the force between two bodies
         * 
         * @param b1 body 1
         * @param b2 body 2
         * @return DirectX::SimpleMath::Vector3 force
         */
        DirectX::SimpleMath::Vector3 Force(IBody *b1, IBody *b2);
    }
}