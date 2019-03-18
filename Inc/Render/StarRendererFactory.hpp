#pragma once

#include <memory>
#include <wrl/client.h>

#include "Body/IStar.hpp"
#include "IStarRenderer.hpp"
#include "SimpleStarRenderer.hpp"

namespace Galactic
{
    /**
     * @brief Factory method to create a star renderer
     * 
     * @param deviceContext 
     * @param star 
     * @return std::unique_ptr<IStarRenderer> 
     */
    std::unique_ptr<IStarRenderer> CreateStarRenderer(ID3D11DeviceContext *deviceContext, IStar *star)
    {
        return std::make_unique<SimpleStarRenderer>(deviceContext, star);
    }
}