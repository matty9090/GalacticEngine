#pragma once

#include <list>
#include <memory>

#include "Body/IBody.hpp"
#include "Scene/ILightSource.hpp"
#include "Render/IRenderable.hpp"

namespace Galactic
{
    class IStarSystem : public IRenderable
    {
        public:
            virtual void AddBody(std::unique_ptr<IBody> body) = 0;
            virtual void AddLightSource(std::unique_ptr<ILightSource> source) = 0;
            virtual void SetCameraPos(DirectX::SimpleMath::Vector3 pos) = 0;
            
            virtual IBody *FindBody(std::string name) const = 0;

            virtual std::string GetName() const = 0;
            virtual std::vector<ILightSource*> GetLightSources() const = 0;
    };
}