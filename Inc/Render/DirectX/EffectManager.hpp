#pragma once

#include "Effect.hpp"

#include <map>
#include <string>
#include <memory>

namespace Galactic
{
    class EffectManager
    {
        public:
            static EffectManager &getInstance()
            {
                static EffectManager instance; 
                return instance;
            }

            EffectManager(EffectManager const&) = delete;
            void operator=(EffectManager const&) = delete;

            Effect *GetEffect(ID3D11Device *device, std::wstring vs, std::wstring ps, D3D11_INPUT_ELEMENT_DESC *layout, int num, bool compiled);
            Effect *GetEffect(ID3D11Device *device, std::wstring vs, std::wstring ps, std::wstring hs, std::wstring ds, D3D11_INPUT_ELEMENT_DESC *layout, int num, bool compiled);

        private:
            EffectManager() {}

            std::map<std::wstring, std::unique_ptr<Effect>> m_effects;
    };
}