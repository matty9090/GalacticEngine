#pragma once

#include <d3d11.h>
#include <wrl/client.h>

namespace Galactic
{
    class Effect
    {
        public:
            Effect(ID3D11Device *device, std::wstring vs, std::wstring ps, D3D11_INPUT_ELEMENT_DESC *layout, int num, bool compiled);
            Effect(ID3D11Device *device, std::wstring vs, std::wstring ps, std::wstring hs, std::wstring ds, D3D11_INPUT_ELEMENT_DESC *layout, int num, bool compiled);
            ~Effect() {};

            ID3D11VertexShader *GetVertexShader() const { return m_vertexShader.Get(); }
            ID3D11PixelShader  *GetPixelShader()  const { return m_pixelShader.Get();  }
            ID3D11HullShader   *GetHullShader()   const { return m_hullShader.Get();  }
            ID3D11DomainShader *GetDomainShader() const { return m_domainShader.Get();  }
            ID3D11InputLayout  *GetInputLayout()  const { return m_layout.Get(); }

            void Reset();

        private:
            Microsoft::WRL::ComPtr<ID3D11InputLayout> m_layout;

            Microsoft::WRL::ComPtr<ID3D11VertexShader> m_vertexShader;
            Microsoft::WRL::ComPtr<ID3D11PixelShader> m_pixelShader;
            Microsoft::WRL::ComPtr<ID3D11HullShader> m_hullShader;
            Microsoft::WRL::ComPtr<ID3D11DomainShader> m_domainShader;

            void LogErrors(ID3DBlob *error);
    };
}