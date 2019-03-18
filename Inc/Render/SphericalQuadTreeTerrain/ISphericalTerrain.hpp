#pragma once

#include "Render/IRenderable.hpp"
#include "Render/DirectX/Effect.hpp"
#include "Noise/FastNoise.h"

namespace Galactic
{
    /**
     * @brief Sperical Terrain interface
     * 
     */
    class ISphericalTerrain : public IRenderable
    {
        public:
            /**
             * @brief Get the radius
             * 
             * @return float 
             */
            virtual float GetRadius() const = 0;

            /**
             * @brief Get the shader associated with rendering the terrain
             * 
             * @return Effect* 
             */
            virtual Effect *GetEffect() const = 0;

            /**
             * @brief Get the device context
             * 
             * @return Microsoft::WRL::ComPtr<ID3D11DeviceContext> 
             */
            virtual Microsoft::WRL::ComPtr<ID3D11DeviceContext> GetContext() const = 0;

            /**
             * @brief Get the grid size
             * 
             * @return size_t 
             */
            virtual size_t GetGridSize() const = 0;

            //TODO: make private
            virtual void SetRenderContext() = 0; 

            /**
             * @brief Get the biome from the lookup texture
             * 
             * @param lookup Vector consisting of moisure and elevation respectively
             * @return std::string biome name 
             */
            virtual __forceinline std::string GetBiome(const DirectX::SimpleMath::Vector2 &lookup) = 0;

            /**
             * @brief Get the height of the terrain at a specific normal point on the terrain
             * 
             * @param point 
             * @param height Returns the height
             * @param biome Returns the biome lookup UV
             * @param texIndex Returns the biome name
             */
            virtual __forceinline void GetHeight(const DirectX::SimpleMath::Vector3 &point, float &height, DirectX::SimpleMath::Vector2 &biome, std::string &texIndex) = 0;

            /**
             * @brief Destroy the ISphericalTerrain object
             * 
             */
            virtual ~ISphericalTerrain() {}
    };
}