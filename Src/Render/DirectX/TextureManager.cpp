#include "pch.h"
#include "Render/DirectX/TextureManager.hpp"

#include <d3dx11.h>

using namespace Galactic;

ID3D11ShaderResourceView *TextureManager::GetTexture(ID3D11Device *device, std::string tex)
{
    ID3D11ShaderResourceView *texture;

    if (m_textures.find(tex) != m_textures.end())
    {
        texture = m_textures[tex];
    }
    else
    {
        D3DX11CreateShaderResourceViewFromFileA(device, tex.c_str(), NULL, NULL, &texture, NULL);

        m_textures[tex] = texture;
    }

    return texture;
}

Galactic::TextureManager::~TextureManager()
{
    for (auto tex : m_textures)
    {
        //delete tex.second;
    }
}