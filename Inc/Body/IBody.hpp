#pragma once

#include <string>
#include <memory>
#include <thread>

#include <D3DX11.h>
#include <SimpleMath.h>

#include "../Render/IRenderable.hpp"

namespace Galactic
{
    enum EDetail { Low, Medium, High };

    class IBody : public IRenderable
    {
        public:
            virtual void Generate(EDetail detail = EDetail::Medium) = 0;

            virtual void SetMass(long double mass) = 0;
            virtual void SetPosition(DirectX::SimpleMath::Vector3 pos) = 0;
            virtual void SetVelocity(DirectX::SimpleMath::Vector3 vel) = 0;
            virtual void SetRotation(DirectX::SimpleMath::Vector3 rot) = 0;
            virtual void SetCameraPos(DirectX::SimpleMath::Vector3 camPos) = 0;
            virtual void SetInfluence(IBody *body) = 0;

            virtual std::string                  GetName()     const = 0;
            virtual long double                  GetMass()     const = 0;
            virtual IBody                       *GetInfluence() const = 0;
            virtual DirectX::SimpleMath::Vector3 GetPosition() const = 0;
            virtual DirectX::SimpleMath::Vector3 GetVelocity() const = 0;
            virtual DirectX::SimpleMath::Vector3 GetRotation() const = 0;
            virtual DirectX::SimpleMath::Vector3 GetCameraPos() const = 0;
            
            virtual bool IsGenerated() const = 0;
            
            static bool Wireframe;

			virtual ~IBody() {}
    };
}