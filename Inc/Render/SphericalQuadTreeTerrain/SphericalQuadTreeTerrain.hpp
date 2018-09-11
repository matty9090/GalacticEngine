#pragma once

#include "Body/IPlanet.hpp"

namespace Galactic
{
    class SphericalQuadTreeTerrain : public IRenderable
    {
        public:
            SphericalQuadTreeTerrain(std::weak_ptr<IPlanet> planet);

            void Render(DirectX::SimpleMath::Matrix view, DirectX::SimpleMath::Matrix proj);
            void Update(float dt);
            void Reset();

        private:
            std::weak_ptr<IPlanet> m_planet;
    };
}