#pragma once

#include <d3d11.h>
#include <wrl/client.h>

namespace Galactic
{
    template <class VertexType>
    class Drawable
    {
        public:
            Drawable(ID3D11DeviceContext *context, D3D_PRIMITIVE_TOPOLOGY topology);

            void Init();
            void PreDraw();
            void Draw();
            void Cleanup();
            
        private:
            D3D_PRIMITIVE_TOPOLOGY m_topology;
            D3D11_BUFFER_DESC m_vertexBufferDesc, m_indexBufferDesc;
            D3D11_SUBRESOURCE_DATA m_vertexData, m_indexData;

            Microsoft::WRL::ComPtr<ID3D11Buffer> m_vertexBuffer, m_indexBuffer;
            
            ID3D11Device *m_device;
            ID3D11DeviceContext *m_context;

        protected:
            std::vector<VertexType> m_vertices;
            std::vector<uint16_t> m_indices;
    };

    template<class VertexType>
    Drawable<VertexType>::Drawable(ID3D11DeviceContext *context, D3D_PRIMITIVE_TOPOLOGY topology)
        : m_context(context),
          m_topology(topology)
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

    template<class VertexType>
    inline void Drawable<VertexType>::Init()
    {
        m_vertexBufferDesc.ByteWidth = sizeof(VertexType) * m_vertices.size();
        m_indexBufferDesc.ByteWidth = sizeof(uint16_t) * m_indices.size();
        
        m_vertexData.pSysMem = &m_vertices[0];
        m_indexData.pSysMem = &m_indices[0];

        DX::ThrowIfFailed(m_device->CreateBuffer(&m_vertexBufferDesc, &m_vertexData, m_vertexBuffer.GetAddressOf()));
        DX::ThrowIfFailed(m_device->CreateBuffer(&m_indexBufferDesc, &m_indexData, m_indexBuffer.GetAddressOf()));
    }

    template<class VertexType>
    inline void Drawable<VertexType>::PreDraw()
    {
        unsigned int stride = sizeof(VertexType);
        unsigned int offset = 0;

        m_context->IASetVertexBuffers(0, 1, m_vertexBuffer.GetAddressOf(), &stride, &offset);
        m_context->IASetIndexBuffer(m_indexBuffer.Get(), DXGI_FORMAT_R16_UINT, 0);
        m_context->IASetPrimitiveTopology(m_topology);
    }

    template<class VertexType>
    inline void Drawable<VertexType>::Draw()
    {
        m_context->DrawIndexed(m_indices.size(), 0, 0);
    }

    template<class VertexType>
    inline void Drawable<VertexType>::Cleanup()
    {
        if (m_vertexBuffer.Get() && m_indexBuffer.Get())
        {
            m_vertexBuffer->Release();
            m_indexBuffer->Release();
        }
    }
}