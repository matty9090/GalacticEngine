#pragma once

#include "Effect.hpp"

#include <map>
#include <string>
#include <memory>

namespace Galactic
{
    /**
     * @brief Singleton to manage shaders
     * 
     */
    class EffectManager
    {
        public:
            /**
             * @brief Get the instance
             * 
             * @return EffectManager& 
             */
            static EffectManager &getInstance()
            {
                static EffectManager instance; 
                return instance;
            }

            EffectManager(EffectManager const&) = delete;
            void operator=(EffectManager const&) = delete;

            /**
             * @brief Get a pointer to the shader
             * 
             * @param device 
             * @param vs Vertex shader file
             * @param ps Pixel shader file
             * @param layout Vertex layout
             * @param num Number of vertex layout elements
             * @param compiled Is the shader compiled
             * @return Effect*
             */
            Effect *GetEffect(ID3D11Device *device, std::wstring vs, std::wstring ps, D3D11_INPUT_ELEMENT_DESC *layout, int num, bool compiled);

        private:
            EffectManager() {}

            std::map<std::wstring, std::unique_ptr<Effect>> m_effects;
    };
}