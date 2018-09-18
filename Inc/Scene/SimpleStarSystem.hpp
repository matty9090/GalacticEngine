#pragma once

#include <list>

#include "Scene/IStarSystem.hpp"
#include "Scene/ILightSource.hpp"

namespace Galactic
{
    class SimpleStarSystem : public IStarSystem
    {
        public:
            SimpleStarSystem(std::string name);

            void AddBody(std::unique_ptr<IBody> body) { m_bodies.push_back(std::move(body)); }
            void AddLightSource(std::unique_ptr<ILightSource> source) { m_lights.push_back(std::move(source)); }
            void SetCameraPos(DirectX::SimpleMath::Vector3 pos) { m_cameraPos = pos; };

            IBody *FindBody(std::string name) const;

            void Update(float dt);
            void Render(DirectX::SimpleMath::Matrix view, DirectX::SimpleMath::Matrix proj);
            void Reset();

            std::string GetName() const { return m_name; }
            std::vector<ILightSource*> GetLightSources() const;

            DirectX::SimpleMath::Matrix &GetMatrix() { return m_world; }

        private:
            std::string m_name;

            DirectX::SimpleMath::Matrix m_world;;
            DirectX::SimpleMath::Vector3 m_cameraPos;

            std::list<std::unique_ptr<ILightSource>> m_lights;
            std::list<std::unique_ptr<IBody>> m_bodies;
    };
}