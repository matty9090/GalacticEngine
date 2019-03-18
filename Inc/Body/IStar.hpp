#pragma once

#include "Body/IBody.hpp"
#include "Render/IRenderable.hpp"

namespace Galactic
{
    /**
     * @brief IStar interface
     * 
     */
    class IStar : public IBody
    {
        public:
            /**
             * @brief Set the radius
             * 
             * @param radius 
             */
            virtual void SetRadius(double radius) = 0;
            
            /**
             * @brief Set the temperature
             * 
             * @param kelvin 
             */
            virtual void SetTemperature(int kelvin) = 0;

            /**
             * @brief Get the radius
             * 
             * @return double 
             */
            virtual double GetRadius() const = 0;

            /**
             * @brief Get the temperature
             * 
             * @return int 
             */
            virtual int GetTemperature() const = 0;

            /**
             * @brief Destroy the IStar object
             * 
             */
            virtual ~IStar() {}
    };
}