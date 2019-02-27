#include "pch.h"
#include "Render/DirectX/Effect.hpp"

#include <d3dcompiler.h>
#include <fstream>

using namespace Galactic;

Effect::Effect(ID3D11Device *device, std::wstring vs, std::wstring ps, std::wstring hs, std::wstring ds, D3D11_INPUT_ELEMENT_DESC *layout, int num, bool compiled)
    : m_vertexShader(nullptr),
      m_pixelShader(nullptr),
      m_hullShader(nullptr),
      m_domainShader(nullptr)
{
    ID3DBlob *v_buffer = nullptr, *p_buffer = nullptr;
    ID3DBlob *h_buffer = nullptr, *d_buffer = nullptr;
    ID3DBlob *v_error  = nullptr, *p_error  = nullptr;
    ID3DBlob *h_error  = nullptr, *d_error  = nullptr;

    bool useTess = (hs != L"" && ds != L"");

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

        res = D3DCompileFromFile(vs.c_str(), nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, "main", "vs_5_0", flags, 0, &v_buffer, &v_error);
        LogErrors(v_error);
        DX::ThrowIfFailed(res, "Creating vertex shader");
        
        res = D3DCompileFromFile(ps.c_str(), nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, "main", "ps_5_0", flags, 0, &p_buffer, &p_error);
        LogErrors(p_error);
        DX::ThrowIfFailed(res, "Creating pixel shader");

        if(useTess)
        {
            res = D3DCompileFromFile(hs.c_str(), nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, "main", "hs_5_0", flags, 0, &h_buffer, &h_error);
            LogErrors(h_error);
            DX::ThrowIfFailed(res, "Creating hull shader");

            res = D3DCompileFromFile(ds.c_str(), nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, "main", "ds_5_0", flags, 0, &d_buffer, &d_error);
            LogErrors(d_error);
            DX::ThrowIfFailed(res, "Creating domain shader");
        }
    }

    DX::ThrowIfFailed(device->CreateVertexShader(v_buffer->GetBufferPointer(), v_buffer->GetBufferSize(), nullptr, m_vertexShader.ReleaseAndGetAddressOf()), "Failed to create buffer");
    DX::ThrowIfFailed(device->CreatePixelShader(p_buffer->GetBufferPointer(), p_buffer->GetBufferSize(), nullptr, m_pixelShader.ReleaseAndGetAddressOf()), "Failed to create buffer");
    
    if (useTess)
    {
        DX::ThrowIfFailed(device->CreateHullShader(h_buffer->GetBufferPointer(), h_buffer->GetBufferSize(), nullptr, m_hullShader.ReleaseAndGetAddressOf()), "Failed to create buffer");
        DX::ThrowIfFailed(device->CreateDomainShader(d_buffer->GetBufferPointer(), d_buffer->GetBufferSize(), nullptr, m_domainShader.ReleaseAndGetAddressOf()), "Failed to create buffer");
    }
    
    DX::ThrowIfFailed(device->CreateInputLayout(layout, num, v_buffer->GetBufferPointer(), v_buffer->GetBufferSize(), m_layout.ReleaseAndGetAddressOf()), "Failed to create buffer");

    v_buffer->Release();
    p_buffer->Release();

    if (useTess)
    {
        h_buffer->Release();
        d_buffer->Release();
    }
}

Effect::Effect(ID3D11Device *device, std::wstring vs, std::wstring ps, D3D11_INPUT_ELEMENT_DESC *layout, int num, bool compiled)
    : Effect(device, vs, ps, L"", L"", layout, num, compiled)
{

}

void Effect::Reset()
{
    
}

void Effect::LogErrors(ID3DBlob *error)
{
    if (!error)
        return;

    char *compileErrors;
    unsigned long bufferSize, i;
    std::ofstream file("errors.txt", std::ios_base::app);

    compileErrors = (char*)(error->GetBufferPointer());
    bufferSize = error->GetBufferSize();

    for (i = 0; i < bufferSize; i++)
        file << compileErrors[i];

    file.close();

    error->Release();
}
