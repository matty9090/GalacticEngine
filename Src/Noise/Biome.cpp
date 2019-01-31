#include "pch.h"
#include "Noise/Biome.hpp"

using namespace Galactic;

void Galactic::BiomeConfig::Generate(ID3D11Device *device, ID3D11Texture2D **tex, ID3D11ShaderResourceView **srv, size_t width, size_t height)
{
    size_t size = width * height * 4;
    float *pixels = new float[size];

    /*for (int y = 0; y < height; ++y)
    {
        for (int x = 0; x < width * 4; x += 4)
        {
            pixels[y * width + x + 0] = 0.0f;
            pixels[y * width + x + 1] = 150.0f / 255.0f;
            pixels[y * width + x + 2] = 255.0f / 255.0f;
            pixels[y * width + x + 3] = 255.0f / 255.0f;
        }
    }*/

    size_t minY = 0;
    size_t minX = 0;

    for (auto row : biomes)
    {
        float elevation = row.first;
        int maxY = (int)((float)height * elevation);

        for (auto col : row.second.biomes)
        {
            float moisture = col.first;
            int   maxX = (int)((float)width * moisture);
            auto  colour = col.second;

            for (size_t y = minY; y < maxY; ++y) {
                for (size_t x = minX * 4; x < maxX * 4; x += 4) {
                    pixels[y * width + x + 0] = colour.R();
                    pixels[y * width + x + 1] = colour.G();
                    pixels[y * width + x + 2] = colour.B();
                    pixels[y * width + x + 3] = colour.A();
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
    desc.Usage = D3D11_USAGE_DEFAULT;
    desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;

    D3D11_SUBRESOURCE_DATA initialData;
    initialData.pSysMem = pixels;
    initialData.SysMemPitch = width * sizeof(float);
    initialData.SysMemSlicePitch = width * height * sizeof(float);

    D3D11_SHADER_RESOURCE_VIEW_DESC srv_desc = {};
    srv_desc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
    srv_desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    srv_desc.Texture2D.MipLevels = 1;

    DX::ThrowIfFailed(device->CreateTexture2D(&desc, &initialData, tex));
    DX::ThrowIfFailed(device->CreateShaderResourceView((ID3D11Resource*)(*tex), &srv_desc, srv));
}

void Galactic::BiomeConfig::AddBiomeRow(Row row, float elevation)
{
    elevation = (elevation > 1.0f) ? 1.0f : elevation;
    biomes[elevation] = row;
}
