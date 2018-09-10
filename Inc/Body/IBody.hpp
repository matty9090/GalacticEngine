#pragma once

#include <string>
#include <memory>

#include <D3DX11.h>
#include <SimpleMath.h>

#include "../Render/IRenderable.hpp"

namespace Galactic
{
	class IBody : public IRenderable
	{
		public:
			virtual void Generate() = 0;

			virtual void SetMass(long double mass) = 0;
			virtual void SetPosition(DirectX::SimpleMath::Vector3 pos) = 0;
			virtual void SetVelocity(DirectX::SimpleMath::Vector3 vel) = 0;
			virtual void SetInfluence(std::shared_ptr<IBody> body) = 0;
	};
}