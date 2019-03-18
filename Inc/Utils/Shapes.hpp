#pragma once

namespace Galactic
{
    namespace Utils
    {
        /**
         * @brief Create a sphere
         * 
         * @param radius 
         * @param tessellation 
         * @param vertices 
         * @param indices 
         */
        void CreateSphere(float radius, size_t tessellation, std::vector<DirectX::VertexPositionTexture> &vertices, std::vector<uint16_t> &indices);
    }
}