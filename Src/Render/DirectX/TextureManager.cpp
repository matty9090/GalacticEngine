#include "pch.h"
#include "Render/DirectX/TextureManager.hpp"

#include <d3dx11.h>

using namespace Galactic;

ID3D11ShaderResourceView *TextureManager::GetTexture(ID3D11Device *device, std::string tex)
{
    ID3D11ShaderResourceView *texture = NULL;

    if (m_textures.find(tex) != m_textures.end())
    {
        texture = m_textures[tex];
    }
    else
    {
        std::cout << "Loading " << tex << "\n";
        DX::ThrowIfFailed(D3DX11CreateShaderResourceViewFromFileA(device, tex.c_str(), NULL, NULL, &texture, NULL), "Error loading " + tex);

        m_textures[tex] = texture;
    }

    return texture;
}

Galactic::TextureManager::~TextureManager()
{
    for (auto tex : m_textures)
    {
        if (tex.second)
            tex.second->Release();
    }
}