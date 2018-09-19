#pragma once

namespace Galactic
{
    namespace Utils
    {
        void CreateSphere(float radius, size_t tessellation, std::vector<DirectX::VertexPositionTexture> &vertices, std::vector<uint16_t> &indices);
    }
}