#pragma once

#include "Body/IPlanet.hpp"
#include "IRenderable.hpp"
#include "Render/Drawable.hpp"
#include "Render/Billboard.hpp"
#include "Render/GrassDistributor.hpp"
#include "Render/DirectX/EffectManager.hpp"
#include "Render/DirectX/ConstantBuffer.hpp"

#include <SimpleMath.h>

namespace Galactic
{
    // TODO: Remove
    class GrassRenderer : public IRenderable
    {
        public:
            GrassRenderer(ID3D11DeviceContext *context, IPlanet *planet);
            virtual ~GrassRenderer();

            void Render(DirectX::SimpleMath::Matrix view, DirectX::SimpleMath::Matrix proj);
            void Update(float dt);
            void Reset();

            GrassDistributor &GetDistributor() { return m_distributor; }

            DirectX::SimpleMath::Matrix GetMatrix() const { return m_world; }

        private:
            GrassDistributor m_distributor;

            DirectX::SimpleMath::Matrix m_world;

            Microsoft::WRL::ComPtr<ID3D11RasterizerState> m_raster;

            IPlanet *m_planet;
            Effect *m_effect;

            std::unique_ptr<Billboard> m_grass;
    };
}