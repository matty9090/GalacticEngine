#pragma once

#include <D3DX11.h>
#include <memory>
#include <wrl/client.h>
#include <SimpleMath.h>

namespace Galactic
{
    class IRenderable {
        public:
            virtual void Render(DirectX::SimpleMath::Matrix view, DirectX::SimpleMath::Matrix proj) = 0;
            virtual void Update(float dt) = 0;
            virtual void Reset() = 0;
    };
}