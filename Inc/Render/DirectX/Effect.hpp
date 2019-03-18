#pragma once

#include <d3d11.h>
#include <wrl/client.h>

namespace Galactic
{
    /**
     * @brief DirectX shader helper
     * 
     */
    class Effect
    {
        public:
            /**
             * @brief Construct a new Effect object
             * 
             * @param device 
             * @param vs Vertex shader file
             * @param ps Pixel shader file
             * @param layout Vertex layout pointer
             * @param num Number of vertex elements
             * @param compiled Is the shader compiled
             */
            Effect(ID3D11Device *device, std::wstring vs, std::wstring ps, D3D11_INPUT_ELEMENT_DESC *layout, int num, bool compiled);

            /**
             * @brief Destroy the Effect object
             * 
             */
            ~Effect() {};

            /**
             * @brief Get the Vertex Shader buffer
             * 
             * @return ID3D11VertexShader* 
             */
            ID3D11VertexShader *GetVertexShader() const { return m_vertexShader.Get(); }

            /**
             * @brief Get the Pixel Shader buffer
             * 
             * @return ID3D11PixelShader* 
             */
            ID3D11PixelShader  *GetPixelShader()  const { return m_pixelShader.Get();  }

            /**
             * @brief Get the Input Layout pointer
             * 
             * @return ID3D11InputLayout* 
             */
            ID3D11InputLayout  *GetInputLayout()  const { return m_layout.Get(); }

            /**
             * @brief Clean up resources
             * 
             */
            void Reset();

        private:
            Microsoft::WRL::ComPtr<ID3D11InputLayout> m_layout;

            Microsoft::WRL::ComPtr<ID3D11VertexShader> m_vertexShader;
            Microsoft::WRL::ComPtr<ID3D11PixelShader> m_pixelShader;

            void LogErrors(ID3DBlob *error);
    };
}