#include "pch.h"
#include "SimpleMath.h"
#include "Physics/Gravity.hpp"
#include "Physics/Constants.hpp"

using namespace Galactic;
using DirectX::SimpleMath::Vector3;

Vector3 Gravity::Force(IBody *b1, IBody *b2)
{
    Vector3 pos1 = b1->GetPosition(), pos2 = b2->GetPosition();

    double r = (double)Vector3::Distance(pos1, pos2);
    double dx = pos1.x - pos2.x, dy = pos1.y - pos2.y, dz = pos1.z - pos2.z;
    double f = (Constants::G * b1->GetMass() * b2->GetMass()) / (r * r);

    f /= Constants::Scale;

    return Vector3(f * dx / r, f * dy / r, f * dz / r);
}
