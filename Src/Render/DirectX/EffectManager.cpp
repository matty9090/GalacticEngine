#include "pch.h"
#include "Render/DirectX/EffectManager.hpp"

using namespace Galactic;

Effect *EffectManager::GetEffect(ID3D11Device *device, std::wstring vs, std::wstring ps, D3D11_INPUT_ELEMENT_DESC *layout, int num, bool compiled)
{
    Effect *effect;
    std::wstring key = vs + L"-" + ps;

    if (m_effects.find(key) != m_effects.end())
    {
        effect = m_effects[key].get();
    }
    else
    {
        std::unique_ptr<Effect> eff = std::make_unique<Effect>(device, vs, ps, layout, num, compiled);
        effect = eff.get();

        m_effects[key] = std::move(eff);
    }

    return effect;
}
