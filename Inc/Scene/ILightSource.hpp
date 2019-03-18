#pragma once

#include "SimpleMath.h"

namespace Galactic
{
    /**
     * @brief Light source interface
     * 
     */
    class ILightSource
    {
        public:
            /**
             * @brief Set the colour of the light
             * 
             * @param col 
             */
            virtual void SetColour(DirectX::SimpleMath::Color col) = 0;

            /**
             * @brief Set the position of the light
             * 
             * @param pos 
             */
            virtual void SetPosition(DirectX::SimpleMath::Vector3 pos) = 0;

            /**
             * @brief Get the colour
             * 
             * @return DirectX::SimpleMath::Color 
             */
            virtual DirectX::SimpleMath::Color GetColour() const = 0;

            /**
             * @brief Get the position
             * 
             * @return DirectX::SimpleMath::Vector3 
             */
            virtual DirectX::SimpleMath::Vector3 GetPosition() const = 0;

            /**
             * @brief Destroy the ILightSource object
             * 
             */
            virtual ~ILightSource() {}
    };
}