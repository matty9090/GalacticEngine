#include "pch.h"
#include "Noise/Biome.hpp"
#include <fstream>

using namespace Galactic;

void Galactic::BiomeConfig::Generate(ID3D11Device *device, ID3D11Texture2D **tex, ID3D11ShaderResourceView **srv, size_t width, size_t height)
{
    float **pixels = new float*[height];

    for (size_t y = 0; y < height; ++y)
        pixels[y] = new float[width * 4];

    for (size_t y = 0; y < height; ++y)
    {
        for (size_t x = 0; x < width * 4; x += 4)
        {
            pixels[y][x + 0] = 0.0f;
            pixels[y][x + 1] = 0.0f;
            pixels[y][x + 2] = 0.0f;
            pixels[y][x + 3] = 1.0f;
        }
    }

    
    size_t minY = 0;

    for (auto row : biomes)
    {
        float elevation = row.first;
        size_t maxY = (int)((float)height * elevation);
        size_t minX = 0;

        for (auto col : row.second.biomes)
        {
            float  moisture = col.first;
            auto   colour = col.second;
            size_t maxX = (int)((float)width * moisture);

            for (size_t y = minY; y < maxY; ++y) {
                for (size_t x = minX * 4; x < maxX * 4; x += 4) {
                    pixels[y][x + 0] = colour.R() - 0.2f;
                    pixels[y][x + 1] = colour.G() - 0.2f;
                    pixels[y][x + 2] = colour.B() - 0.2f;
                    pixels[y][x + 3] = 1.0f;
                }
            }

            minX = maxX;
        }
        
        minY = maxY;
    }

    D3D11_TEXTURE2D_DESC desc = {};
    desc.Width = width;
    desc.Height = height;
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

    DX::ThrowIfFailed(device->CreateTexture2D(&desc, NULL, tex));

    ID3D11DeviceContext *context;
    device->GetImmediateContext(&context);

    D3D11_MAPPED_SUBRESOURCE mapped;
    DX::ThrowIfFailed(context->Map((ID3D11Resource*)(*tex), 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped));

    BYTE *textureRow = reinterpret_cast<BYTE*>(mapped.pData);
    int rowLength = desc.Width * sizeof(float) * 4;

    for (unsigned int h = 0; h < desc.Height; ++h)
    {
        memcpy(textureRow, pixels[h], rowLength);
        textureRow += mapped.RowPitch;
    }

    context->Unmap((ID3D11Resource*)(*tex), 0);

    DX::ThrowIfFailed(device->CreateShaderResourceView((ID3D11Resource*)(*tex), &srv_desc, srv));

    for (size_t y = 0; y < height; ++y)
        delete[] pixels[y];

    delete[] pixels;
}

void Galactic::BiomeConfig::AddBiomeRow(Row row, float elevation)
{
    elevation = (elevation > 1.0f) ? 1.0f : elevation;
    biomes[elevation] = row;
}
