#pragma once

#include <memory>
#include <wrl/client.h>

#include "Body/IStar.hpp"
#include "IStarRenderer.hpp"
#include "SimpleStarRenderer.hpp"

namespace Galactic {
    std::unique_ptr<IStarRenderer> CreateStarRenderer(ID3D11DeviceContext *deviceContext, IStar *star)
    {
        return std::make_unique<SimpleStarRenderer>(deviceContext, star);
    }
}