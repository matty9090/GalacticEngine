#pragma once

#include "Body/IPlanet.hpp"
#include "Render/Drawable.hpp"
#include "Render/IAtmosphereRenderer.hpp"

#include "Render/DirectX/Effect.hpp"
#include "Render/DirectX/ConstantBuffer.hpp"

namespace Galactic
{
    struct ScatteredAtmosphereVertex
    {
        DirectX::SimpleMath::Vector3 position;
    };

    struct ScatteredAtmosphereBuffer
    {
		DirectX::SimpleMath::Matrix worldViewProj;
		DirectX::SimpleMath::Vector3 v3CameraPos;
		float fCameraHeight;
		DirectX::SimpleMath::Vector3 v3LightDir;
		float fCameraHeight2;
		DirectX::SimpleMath::Vector3 v3InvWavelength;
		float fOuterRadius;
		float fOuterRadius2;
		float fInnerRadius;
		float fInnerRadius2;
		float fKrESun;
		float fKmESun;
		float fKr4PI;
		float fKm4PI;
		float fScale;
		float fScaleDepth;
		float fScaleOverScaleDepth;
		float _pad0, _pad1;
    };

	struct ScatteredAtmosphereBufferPS
	{
		DirectX::SimpleMath::Vector3 v3LightDir;
		float g;
		float g2;
		float _pad0, _pad1, _pad3;
	};

    class ScatteredAtmosphere : public IAtmosphereRenderer, public Drawable<ScatteredAtmosphereVertex>
    {
        public:
			ScatteredAtmosphere(ID3D11DeviceContext *context, IPlanet *planet);

            void Render(DirectX::SimpleMath::Matrix view, DirectX::SimpleMath::Matrix proj);
            void Update(float dt);
            void Reset();

            DirectX::SimpleMath::Matrix GetMatrix() const { return m_world; }

        private:
            DirectX::SimpleMath::Matrix m_world;

            Microsoft::WRL::ComPtr<ID3D11RasterizerState> m_raster;

            IPlanet *m_planet;
            std::unique_ptr<Effect> m_effect;
            std::unique_ptr<DirectX::CommonStates> m_states;
            std::unique_ptr<ConstantBuffer<ScatteredAtmosphereBuffer>> m_buffer;
            std::unique_ptr<ConstantBuffer<ScatteredAtmosphereBufferPS>> m_buffer2;
    };
}