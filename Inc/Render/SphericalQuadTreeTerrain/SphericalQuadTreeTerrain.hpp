#pragma once

#include "Body/IPlanet.hpp"

namespace Galactic
{
    class SphericalQuadTreeTerrain : public IRenderable
    {
        public:
            SphericalQuadTreeTerrain(std::weak_ptr<IPlanet> planet);
            ~SphericalQuadTreeTerrain();

            void Render(DirectX::SimpleMath::Matrix view, DirectX::SimpleMath::Matrix proj);
            void Update(float dt);
            void Reset();

            DirectX::SimpleMath::Matrix &GetMatrix() { return m_world; }

        private:
            DirectX::SimpleMath::Matrix &m_world;
            std::weak_ptr<IPlanet> m_planet;
    };
}