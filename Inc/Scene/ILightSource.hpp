#pragma once

#include "SimpleMath.h"

namespace Galactic
{
    class ILightSource
    {
        public:
            virtual void SetColour(DirectX::SimpleMath::Color col) = 0;
            virtual void SetPosition(DirectX::SimpleMath::Vector3 pos) = 0;

            virtual DirectX::SimpleMath::Color GetColour() const = 0;
            virtual DirectX::SimpleMath::Vector3 GetPosition() const = 0;

            virtual ~ILightSource() {}
    };
}