#pragma once

#include <d3d11.h>

#include <map>
#include <string>
#include <memory>

namespace Galactic
{
    class TextureManager
    {
        public:
            static TextureManager &getInstance()
            {
                static TextureManager instance;
                return instance;
            }

            ~TextureManager();

            TextureManager(TextureManager const&) = delete;
            void operator=(TextureManager const&) = delete;

            ID3D11ShaderResourceView *GetTexture(ID3D11Device *device, std::string tex);

        private:
            TextureManager() {}

            std::map<std::string, ID3D11ShaderResourceView*> m_textures;
    };
}