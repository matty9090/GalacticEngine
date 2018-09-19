#pragma once

#include "Body/IBody.hpp"

namespace Galactic
{
    namespace Gravity
    {
        DirectX::SimpleMath::Vector3 Force(IBody *b1, IBody *b2);
    }
}