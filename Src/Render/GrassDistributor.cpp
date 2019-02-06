#include "pch.h"
#include "Render/GrassDistributor.hpp"

using namespace Galactic;
using namespace DirectX::SimpleMath;

GrassDistributor::GrassDistributor() : m_patchRadius(1.0f), m_patchNum(100)
{

}

void GrassDistributor::AddPatch(std::string key, Vector3 pos, Vector3 normal)
{
    m_patches[key] = { pos, normal };
}
