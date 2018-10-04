#pragma once

#include "Body/IBody.hpp"
#include "SimpleMath.h"

namespace Galactic
{
	class Camera
	{
		public:
			Camera(size_t width, size_t size_t);

			void Update(float dt);
			void Events(DirectX::Mouse *mouse, DirectX::Mouse::State &ms, float dt);
			
			void Move(DirectX::SimpleMath::Vector3 v) { m_relPos += v; }
			void Attach(IBody *body) { m_body = body; }

			DirectX::XMMATRIX GetViewMatrix() const { return m_view; }
			DirectX::XMMATRIX GetProjectionMatrix() const { return m_proj; }
			DirectX::SimpleMath::Vector3 GetPosition() const { return m_cameraPos; }
			DirectX::SimpleMath::Quaternion GetQuaternion() const;

		private:
			size_t m_width, m_height;
			float m_pitch, m_yaw;

			IBody *m_body;

			DirectX::SimpleMath::Matrix m_view, m_proj;
			DirectX::SimpleMath::Vector3 m_cameraPos, m_relPos;
	};
}