#include "pch.h"
#include "Noise/Biome.hpp"

#include <fstream>
#include <wincodecsdk.h>

using namespace Galactic;

std::map<std::string, Biome> BiomeConfig::Biomes;

BiomeConfig::~BiomeConfig()
{
    Clear();
}

void BiomeConfig::Generate(ID3D11Device *device, ID3D11ShaderResourceView **srv, size_t w, size_t h)
{
    Clear();

    m_width = w;
    m_height = h;

    m_pixels = new float*[m_height];
    m_pixelBiomes = new std::string*[m_height];

    for (size_t y = 0; y < m_height; ++y)
        m_pixels[y] = new float[m_width * 4];

    for (size_t y = 0; y < m_height; ++y)
        m_pixelBiomes[y] = new std::string[m_width];

    for (size_t y = 0; y < m_height; ++y)
    {
        for (size_t x = 0; x < m_width * 4; x += 4)
        {
            m_pixels[y][x + 0] = 0.0f;
            m_pixels[y][x + 1] = 0.0f;
            m_pixels[y][x + 2] = 0.0f;
            m_pixels[y][x + 3] = 1.0f;
        }
    }
    
    size_t minY = 0;

    for (auto row : m_biomes)
    {
        float elevation = row.first;
        size_t maxY = (int)((float)m_height * elevation);
        size_t minX = 0;

        for (auto col : row.second.biomes)
        {
            float  moisture = col.first;
            auto   colour = Biomes[col.second].Colour;
            size_t maxX = (int)((float)m_width * moisture);

            for (size_t y = minY; y < maxY; ++y)
            {
                for (size_t x = minX * 4; x < maxX * 4; x += 4)
                {
                    m_pixels[y][x + 0]  = colour.R() - 0.2f;
                    m_pixels[y][x + 1]  = colour.G() - 0.2f;
                    m_pixels[y][x + 2]  = colour.B() - 0.2f;
                    m_pixels[y][x + 3]  = 1.0f;
                }

                for (size_t x = minX; x < maxX; ++x)
                    m_pixelBiomes[y][x] = col.second;
            }

            minX = maxX;
        }
        
        minY = maxY;
    }

    D3D11_TEXTURE2D_DESC desc = {};
    desc.Width = m_width;
    desc.Height = m_height;
    desc.MipLevels = 1;
    desc.ArraySize = 1;
    desc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
    desc.SampleDesc.Count = 1;
    desc.Usage = D3D11_USAGE_DYNAMIC;
    desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
    desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    
    D3D11_SHADER_RESOURCE_VIEW_DESC srv_desc = {};
    srv_desc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
    srv_desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    srv_desc.Texture2D.MipLevels = 1;

    DX::ThrowIfFailed(device->CreateTexture2D(&desc, NULL, &m_tex));

    ID3D11DeviceContext *context;
    device->GetImmediateContext(&context);

    D3D11_MAPPED_SUBRESOURCE mapped;
    DX::ThrowIfFailed(context->Map((ID3D11Resource*)m_tex, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped));

    BYTE *textureRow = reinterpret_cast<BYTE*>(mapped.pData);
    int rowLength = desc.Width * sizeof(float) * 4;

    for (unsigned int height = 0; height < desc.Height; ++height)
    {
        memcpy(textureRow, m_pixels[height], rowLength);
        textureRow += mapped.RowPitch;
    }

    context->Unmap((ID3D11Resource*)m_tex, 0);

    DX::ThrowIfFailed(device->CreateShaderResourceView((ID3D11Resource*)m_tex, &srv_desc, srv));
    DX::ThrowIfFailed(DirectX::SaveWICTextureToFile(context, (ID3D11Resource*)m_tex, GUID_ContainerFormatPng, L"Resources/b.png"));
}

void BiomeConfig::AddBiomeRow(Row row, float elevation)
{
    elevation = (elevation > 1.0f) ? 1.0f : elevation;
    m_biomes[elevation] = row;
}

void BiomeConfig::Clear()
{
    if (m_pixels)
    {
        for (size_t y = 0; y < m_height; ++y)
            delete[] m_pixels[y];

        delete[] m_pixels;
    }

    if (m_pixelBiomes)
    {
        for (size_t y = 0; y < m_height; ++y)
            delete[] m_pixelBiomes[y];

        delete[] m_pixelBiomes;
    }

    m_pixels = NULL;
    m_pixelBiomes = NULL;
}

std::string Galactic::BiomeConfig::Sample(float m, float e)
{
    size_t ee = (int)(e * m_height);
    size_t mm = (int)(m * m_width);

    return m_pixelBiomes[ee > m_height ? m_height : ee][mm > m_width ? m_width : mm];
}
