#include "pch.h"
#include "Render/SphericalQuadTreeTerrain/TerrainNode.hpp"

using namespace Galactic;
using namespace DirectX;
using namespace DirectX::SimpleMath;

TerrainNode::TerrainNode(std::weak_ptr<ISphericalTerrain> terrain, std::weak_ptr<TerrainNode> parent, std::weak_ptr<IPlanet> planet)
    : m_terrain(terrain),
      m_parent(parent),
      m_planet(planet)
{
    
}

void TerrainNode::Generate()
{

}

void TerrainNode::Render(DirectX::SimpleMath::Matrix view, DirectX::SimpleMath::Matrix proj)
{
    if (IsLeaf())
    {

    }
    else
    {
        for (auto &child : m_children)
            child->Render(view, proj);
    }
}

void TerrainNode::Update(float dt)
{
    if (IsLeaf())
    {

    }
    else
    {
        for (auto &child : m_children)
            child->Update(dt);
    }
}

void TerrainNode::Reset()
{

}

void TerrainNode::Split()
{

}

void TerrainNode::Merge()
{

}
