#pragma once

#include <string>
#include <memory>
#include <thread>

#include <D3DX11.h>
#include <SimpleMath.h>

#include "../Render/IRenderable.hpp"

namespace Galactic
{
    /**
     * @brief Detail levels
     * 
     */
    enum EDetail { Low, Medium, High };

    /**
     * @brief IBody interface
     * 
     */
    class IBody : public IRenderable
    {
        public:
            /**
             * @brief Generate the geometry for the body for a specific detail level
             * 
             * @param detail 
             */
            virtual void Generate(EDetail detail = EDetail::Medium) = 0;

            /**
             * @brief Set the mass
             * 
             * @param mass 
             */
            virtual void SetMass(long double mass) = 0;

            /**
             * @brief Set the position
             * 
             * @param pos 
             */
            virtual void SetPosition(DirectX::SimpleMath::Vector3 pos) = 0;

            /**
             * @brief Set the velocity
             * 
             * @param vel 
             */
            virtual void SetVelocity(DirectX::SimpleMath::Vector3 vel) = 0;

            /**
             * @brief Set the rotation
             * 
             * @param rot 
             */
            virtual void SetRotation(DirectX::SimpleMath::Vector3 rot) = 0;

            /**
             * @brief Pass over the camera position (used for calculations and in shaders)
             * 
             * @param camPos 
             */
            virtual void SetCameraPos(DirectX::SimpleMath::Vector3 camPos) = 0;

            /**
             * @brief Set the body of influence
             * 
             * @param body 
             */
            virtual void SetInfluence(IBody *body) = 0;

            /**
             * @brief Get the name
             * 
             * @return std::string name
             */
            virtual std::string GetName() const = 0;

            /**
             * @brief Get the mass
             * 
             * @return long double mass
             */
            virtual long double GetMass() const = 0;

            /**
             * @brief Get the body of influence
             * 
             * @return IBody*
             */
            virtual IBody *GetInfluence() const = 0;

            /**
             * @brief Get the position
             * 
             * @return DirectX::SimpleMath::Vector3 
             */
            virtual DirectX::SimpleMath::Vector3 GetPosition() const = 0;

            /**
             * @brief Get the velocity
             * 
             * @return DirectX::SimpleMath::Vector3 
             */
            virtual DirectX::SimpleMath::Vector3 GetVelocity() const = 0;

            /**
             * @brief Get the rotation
             * 
             * @return DirectX::SimpleMath::Vector3 
             */
            virtual DirectX::SimpleMath::Vector3 GetRotation() const = 0;

            /**
             * @brief Get the camera position
             * 
             * @return DirectX::SimpleMath::Vector3 
             */
            virtual DirectX::SimpleMath::Vector3 GetCameraPos() const = 0;
            
            /**
             * @brief Get whether the geometry has been generated or not
             * 
             * @return bool
             */
            virtual bool IsGenerated() const = 0;
            
            /**
             * @brief Show planetary bodies as wireframe or not
             * 
             */
            static bool Wireframe;

            /**
             * @brief Destroy the IBody
             * 
             */
            virtual ~IBody() {}
    };
}