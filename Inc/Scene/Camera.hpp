#pragma once

#include "SimpleMath.h"

namespace Galactic
{
	class Camera
	{
		public:
			Camera(size_t width, size_t size_t);

			void Update(float dt);
			void Events(DirectX::Mouse *mouse, DirectX::Mouse::State &ms);
			
			void Move(DirectX::SimpleMath::Vector3 v) { m_cameraPos += v; }

			DirectX::XMMATRIX GetViewMatrix() const { return m_view; }
			DirectX::XMMATRIX GetProjectionMatrix() const { return m_proj; }
			DirectX::SimpleMath::Vector3 GetPosition() const { return m_cameraPos; }
			DirectX::SimpleMath::Quaternion GetQuaternion() const;

		private:
			size_t m_width, m_height;
			float m_pitch, m_yaw;

			DirectX::XMMATRIX m_view, m_proj;
			DirectX::SimpleMath::Vector3 m_cameraPos;
	};
}