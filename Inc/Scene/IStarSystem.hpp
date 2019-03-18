#pragma once

#include <list>
#include <memory>

#include "Body/IBody.hpp"
#include "Scene/ILightSource.hpp"
#include "Render/IRenderable.hpp"

namespace Galactic
{
    /**
     * @brief IStar system interface
     * 
     */
    class IStarSystem : public IRenderable
    {
        public:
            /**
             * @brief Add a body to the star system
             * 
             * @param body 
             */
            virtual void AddBody(std::unique_ptr<IBody> body) = 0;

            /**
             * @brief Remove a body from the star system
             * 
             * @param name 
             */
            virtual void RemoveBody(std::string name) = 0;

            /**
             * @brief Add a light source to the system
             * 
             * @param source 
             */
            virtual void AddLightSource(ILightSource *source) = 0;

            /**
             * @brief Pass over the current camera position
             * 
             * @param pos 
             */
            virtual void SetCameraPos(DirectX::SimpleMath::Vector3 pos) = 0;
            
            /**
             * @brief Find a body in the system by name
             * 
             * @param name 
             * @return IBody* 
             */
            virtual IBody *FindBody(std::string name) const = 0;

            /**
             * @brief Find the closest body to a particular position
             * 
             * @param pos 
             * @return IBody* 
             */
            virtual IBody *GetClosestBody(DirectX::SimpleMath::Vector3 pos) const = 0;

            /**
             * @brief Get the name of the star system
             * 
             * @return std::string 
             */
            virtual std::string GetName() const = 0;

            /**
             * @brief Get a list of light sources in the system
             * 
             * @return std::list<ILightSource*> 
             */
            virtual std::list<ILightSource*> GetLightSources() const = 0;

            /**
             * @brief Destroy the IStarSystem object
             * 
             */
            virtual ~IStarSystem() {}
    };
}