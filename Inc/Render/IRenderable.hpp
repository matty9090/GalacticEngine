#pragma once

#include <D3DX11.h>
#include <memory>
#include <wrl/client.h>
#include <SimpleMath.h>

namespace Galactic
{
	/**
	 * @brief Renderable interface
	 * 
	 */
    class IRenderable {
        public:
            /**
             * @brief Render the object
             * 
             * @param view matrix
             * @param proj matrix
             */
            virtual void Render(DirectX::SimpleMath::Matrix view, DirectX::SimpleMath::Matrix proj) = 0;

            /**
             * @brief Frame update
             * 
             * @param dt 
             */
            virtual void Update(float dt) = 0;

            /**
             * @brief Clean up resources
             * 
             */
            virtual void Reset() = 0;

            /**
             * @brief Get the world matrix
             * 
             * @return DirectX::SimpleMath::Matrix 
             */
            virtual DirectX::SimpleMath::Matrix GetMatrix() const = 0;

            /**
             * @brief Destroy the IRenderable object
             * 
             */
            virtual ~IRenderable() {}
    };
}