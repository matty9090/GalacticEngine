#include "pch.h"
#include "Utils/Shapes.hpp"

#include <map>

using namespace Galactic;
using namespace DirectX;

std::map<size_t, std::vector<VertexPositionTexture>> SphereVertexCache;
std::map<size_t, std::vector<uint16_t>> SphereIndexCache;

void Utils::CreateSphere(float radius, size_t tessellation, std::vector<VertexPositionTexture> &vertices, std::vector<uint16_t> &indices)
{
    vertices.clear();
    indices.clear();

    if (tessellation < 3)
        throw std::out_of_range("tesselation parameter out of range");

    if (!SphereVertexCache[tessellation].empty() && !SphereIndexCache[tessellation].empty())
    {
        vertices = SphereVertexCache[tessellation];
        indices = SphereIndexCache[tessellation];

        return;
    }

    size_t verticalSegments = tessellation;
    size_t horizontalSegments = tessellation * 2;

    // Create rings of vertices at progressively higher latitudes.
    for (size_t i = 0; i <= verticalSegments; i++)
    {
        float v = 1 - float(i) / verticalSegments;

        float latitude = (i * XM_PI / verticalSegments) - XM_PIDIV2;
        float dy, dxz;

        XMScalarSinCos(&dy, &dxz, latitude);

        // Create a single ring of vertices at this latitude.
        for (size_t j = 0; j <= horizontalSegments; j++)
        {
            float u = float(j) / horizontalSegments;

            float longitude = j * XM_2PI / horizontalSegments;
            float dx, dz;

            XMScalarSinCos(&dx, &dz, longitude);

            dx *= dxz;
            dz *= dxz;

            XMVECTOR normal = XMVectorSet(dx, dy, dz, 0);
            XMVECTOR textureCoordinate = XMVectorSet(u, v, 0, 0);

            vertices.push_back(VertexPositionTexture { XMVectorScale(normal, radius), textureCoordinate });
        }
    }

    // Fill the index buffer with triangles joining each pair of latitude rings.
    size_t stride = horizontalSegments + 1;

    for (size_t i = 0; i < verticalSegments; i++)
    {
        for (size_t j = 0; j <= horizontalSegments; j++)
        {
            size_t nextI = i + 1;
            size_t nextJ = (j + 1) % stride;

            indices.push_back((uint16_t)(i * stride + nextJ));
            indices.push_back((uint16_t)(nextI * stride + j));
            indices.push_back((uint16_t)(i * stride + j));

            indices.push_back((uint16_t)(nextI * stride + nextJ));
            indices.push_back((uint16_t)(nextI * stride + j));
            indices.push_back((uint16_t)(i * stride + nextJ));
        }
    }

    SphereVertexCache[tessellation] = vertices;
    SphereIndexCache[tessellation] = indices;
}