#pragma once

#include <SimpleMath.h>
#include <map>

namespace Galactic
{
    // TODO: Remove
    class GrassDistributor
    {
        public:
            GrassDistributor();

            struct Patch
            {
                DirectX::SimpleMath::Vector3 pos, normal;
            };

            void AddPatch(std::string key, DirectX::SimpleMath::Vector3 pos, DirectX::SimpleMath::Vector3 normal);
            void RemovePatch(std::string key) { if(m_patches.find(key) != m_patches.end()) m_patches.erase(key); }
            const std::map<std::string, Patch> &GetPatches() { return m_patches; }

        private:
            float m_patchRadius;
            size_t m_patchNum;

            std::map<std::string, Patch> m_patches;
    };
}
