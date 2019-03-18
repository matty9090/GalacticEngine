#pragma once

#include <d3d11.h>

#include <map>
#include <string>
#include <memory>

namespace Galactic
{
    /**
     * @brief Manages texture resources
     * 
     */
    class TextureManager
    {
        public:
            /**
             * @brief Get the instance
             * 
             * @return TextureManager& 
             */
            static TextureManager &getInstance()
            {
                static TextureManager instance;
                return instance;
            }

            /**
             * @brief Clean up textures
             * 
             */
            ~TextureManager();

            TextureManager(TextureManager const&) = delete;
            void operator=(TextureManager const&) = delete;

            /**
             * @brief Get a specific texture
             * 
             * @param device 
             * @param tex Texture file
             * @return ID3D11ShaderResourceView* 
             */
            ID3D11ShaderResourceView *GetTexture(ID3D11Device *device, std::string tex);

        private:
            TextureManager() {}

            std::map<std::string, ID3D11ShaderResourceView*> m_textures;
    };
}