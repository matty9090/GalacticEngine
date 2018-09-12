#pragma once

#include <D3D11.h>
#include <wrl/client.h>
#include <DirectXHelpers.h>

namespace Galactic
{
    template <class T>
    class ConstantBuffer
    {
        public:
            ConstantBuffer(ID3D11Device *device);

            void SetData(ID3D11DeviceContext *deviceContext, const T &data);
            ID3D11Buffer *GetBuffer() { return m_buffer.Get(); }

        private:
            Microsoft::WRL::ComPtr<ID3D11Buffer> m_buffer;
    };

    template<class T>
    inline ConstantBuffer<T>::ConstantBuffer(ID3D11Device *device)
    {
        D3D11_BUFFER_DESC desc = {};

        desc.ByteWidth = sizeof(T);
        desc.Usage = D3D11_USAGE_DYNAMIC;
        desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
        desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

        DX::ThrowIfFailed(device->CreateBuffer(&desc, nullptr, m_buffer.ReleaseAndGetAddressOf()));
    }

    template<class T>
    inline void ConstantBuffer<T>::SetData(ID3D11DeviceContext *deviceContext, const T &data)
    {
        D3D11_MAPPED_SUBRESOURCE mappedResource;

        DX::ThrowIfFailed(deviceContext->Map(m_buffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource));

        *static_cast<T*>(mappedResource.pData) = data;

        deviceContext->Unmap(m_buffer.Get(), 0);
    }


}