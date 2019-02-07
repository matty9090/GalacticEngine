#pragma once

#include <d3d11.h>
#include <wrl/client.h>

#include <mutex>

//std::mutex d_mutex;

namespace Galactic
{
    template <class VertexType, class IndexType = uint16_t>
    class Drawable
    {
        public:
            Drawable(ID3D11DeviceContext *context, D3D_PRIMITIVE_TOPOLOGY topology);

            void Init();
            void Draw();
            void Cleanup();
            
        private:
            D3D_PRIMITIVE_TOPOLOGY m_topology;
            D3D11_BUFFER_DESC m_vertexBufferDesc, m_indexBufferDesc;
            D3D11_SUBRESOURCE_DATA m_vertexData, m_indexData;

            ID3D11Buffer *m_vertexBuffer, *m_indexBuffer;
            ID3D11Device *m_device;

            bool m_generated;

            std::mutex mutex;

        protected:
            std::vector<VertexType> m_vertices;
            std::vector<IndexType> m_indices;

            ID3D11DeviceContext *m_context;
    };

    template<class VertexType, class IndexType>
    Drawable<VertexType, IndexType>::Drawable(ID3D11DeviceContext *context, D3D_PRIMITIVE_TOPOLOGY topology)
        : m_context(context),
          m_topology(topology),
          m_generated(false)
    {
        context->GetDevice(&m_device);

        m_vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
        m_vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
        m_vertexBufferDesc.CPUAccessFlags = 0;
        m_vertexBufferDesc.MiscFlags = 0;
        m_vertexBufferDesc.StructureByteStride = 0;

        m_vertexData.SysMemPitch = 0;
        m_vertexData.SysMemSlicePitch = 0;

        m_indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
        m_indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
        m_indexBufferDesc.CPUAccessFlags = 0;
        m_indexBufferDesc.MiscFlags = 0;
        m_indexBufferDesc.StructureByteStride = 0;

        m_indexData.SysMemPitch = 0;
        m_indexData.SysMemSlicePitch = 0;
    }

    template<class VertexType, class IndexType>
    inline void Drawable<VertexType, IndexType>::Init()
    {
        std::lock_guard<std::mutex> lock(mutex);

        Cleanup();

        m_vertexBufferDesc.ByteWidth = sizeof(VertexType) * m_vertices.size();
        m_indexBufferDesc.ByteWidth = sizeof(IndexType) * m_indices.size();
        
        m_vertexData.pSysMem = &m_vertices[0];
        m_indexData.pSysMem = &m_indices[0];

        DX::ThrowIfFailed(m_device->CreateBuffer(&m_vertexBufferDesc, &m_vertexData, &m_vertexBuffer));
        DX::ThrowIfFailed(m_device->CreateBuffer(&m_indexBufferDesc, &m_indexData, &m_indexBuffer));

        m_generated = true;
    }

    template<class VertexType, class IndexType>
    inline void Drawable<VertexType, IndexType>::Draw()
    {
        unsigned int stride = sizeof(VertexType);
        unsigned int offset = 0;

        m_context->IASetVertexBuffers(0, 1, &m_vertexBuffer, &stride, &offset);
        m_context->IASetIndexBuffer(m_indexBuffer, DXGI_FORMAT_R16_UINT, 0);
        m_context->IASetPrimitiveTopology(m_topology);

        m_context->DrawIndexed(m_indices.size(), 0U, 0U);
    }

    template<class VertexType, class IndexType>
    inline void Drawable<VertexType, IndexType>::Cleanup()
    {
        if (m_generated)
        {
            m_vertexBuffer->Release();
            m_indexBuffer->Release();

            m_generated = false;
        }
    }
}
