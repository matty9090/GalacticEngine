#pragma once

#include "Body/IPlanet.hpp"
#include "ISphericalTerrain.hpp"

#include <array>

namespace Galactic
{
    class TerrainNode : public IRenderable
    {
        public:
            TerrainNode(std::weak_ptr<ISphericalTerrain> terrain, std::weak_ptr<TerrainNode> parent, std::weak_ptr<IPlanet> planet);

            void Render(DirectX::SimpleMath::Matrix view, DirectX::SimpleMath::Matrix proj);
            void Update(float dt);
            void Reset();

            DirectX::SimpleMath::Matrix &GetMatrix() { return m_world; }

            void Generate();
            bool IsLeaf() { return m_children[0] == nullptr; }
            void Split();
            void Merge();

        private:
            std::weak_ptr<TerrainNode> m_parent;
            std::weak_ptr<IPlanet> m_planet;
            std::weak_ptr<ISphericalTerrain> m_terrain;

            std::array<std::shared_ptr<TerrainNode>, 4> m_children;

            DirectX::SimpleMath::Matrix m_world;
    };
}