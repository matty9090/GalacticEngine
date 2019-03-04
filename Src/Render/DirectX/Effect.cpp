#include "pch.h"
#include "Render/DirectX/Effect.hpp"

#include <d3dcompiler.h>
#include <fstream>

using namespace Galactic;

Effect::Effect(ID3D11Device *device, std::wstring vs, std::wstring ps, D3D11_INPUT_ELEMENT_DESC *layout, int num, bool compiled)
    : m_vertexShader(nullptr),
      m_pixelShader(nullptr)
{
    ID3DBlob *v_buffer = nullptr, *p_buffer = nullptr;
    ID3DBlob *v_error  = nullptr, *p_error  = nullptr;

    if (compiled)
    {
        DX::ThrowIfFailed(D3DReadFileToBlob(vs.c_str(), &v_buffer), "Loading compiled shader");
        DX::ThrowIfFailed(D3DReadFileToBlob(ps.c_str(), &p_buffer));
    }
    else
    {
        UINT flags = 0;

#ifdef _DEBUG
        flags |= D3DCOMPILE_DEBUG;
#endif
        
        HRESULT res;

        std::ofstream file("errors.txt");
        file << "";
        file.close();

        res = D3DCompileFromFile(vs.c_str(), nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, "main", "vs_4_0", flags, 0, &v_buffer, &v_error);
        LogErrors(v_error);
        DX::ThrowIfFailed(res, "Creating vertex shader");
        
        res = D3DCompileFromFile(ps.c_str(), nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, "main", "ps_4_0", flags, 0, &p_buffer, &p_error);
        LogErrors(p_error);
        DX::ThrowIfFailed(res, "Creating pixel shader");
    }

    DX::ThrowIfFailed(device->CreateVertexShader(v_buffer->GetBufferPointer(), v_buffer->GetBufferSize(), nullptr, m_vertexShader.ReleaseAndGetAddressOf()), "Failed to create buffer");
    DX::ThrowIfFailed(device->CreatePixelShader(p_buffer->GetBufferPointer(), p_buffer->GetBufferSize(), nullptr, m_pixelShader.ReleaseAndGetAddressOf()), "Failed to create buffer");
    DX::ThrowIfFailed(device->CreateInputLayout(layout, num, v_buffer->GetBufferPointer(), v_buffer->GetBufferSize(), m_layout.ReleaseAndGetAddressOf()), "Failed to create buffer");

    v_buffer->Release();
    p_buffer->Release();
}

void Effect::Reset()
{
    
}

void Effect::LogErrors(ID3DBlob *error)
{
    if (!error)
        return;

    char *compileErrors;
    size_t bufferSize, i;
    std::ofstream file("errors.txt", std::ios_base::app);

    compileErrors = (char*)(error->GetBufferPointer());
    bufferSize = error->GetBufferSize();

    for (i = 0; i < bufferSize; i++)
        file << compileErrors[i];

    file.close();

    error->Release();
}
