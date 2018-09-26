#include "pch.h"
#include "Scene/Camera.hpp"

using namespace Galactic;
using namespace DirectX;
using namespace DirectX::SimpleMath;

Camera::Camera(size_t width, size_t height)
	: m_width(width),
	  m_height(height),
	  m_cameraPos(0.0f, 0.0f, -50.0f)
{
	m_proj = Matrix::CreatePerspectiveFieldOfView(XM_PI / 4.f, float(width) / float(height), 0.01f, 500000.f);
}

void Camera::Update(float dt)
{
	dt;

	float y = sinf(m_pitch);
	float r = cosf(m_pitch);
	float z = r * cosf(m_yaw);
	float x = r * sinf(m_yaw);

	XMVECTOR look = m_cameraPos + Vector3(x, y, z);
	m_view = XMMatrixLookAtRH(m_cameraPos, look, Vector3::Up);
}

void Camera::Events(DirectX::Mouse *mouse, DirectX::Mouse::State &ms)
{
	if (ms.positionMode == Mouse::MODE_RELATIVE)
	{
		Vector3 delta = Vector3(float(ms.x), float(ms.y), 0.f) * 0.002f;

		m_pitch -= delta.y;
		m_yaw -= delta.x;

		float limit = XM_PI / 2.0f - 0.01f;
		m_pitch = std::max(-limit, m_pitch);
		m_pitch = std::min(+limit, m_pitch);

		if (m_yaw > XM_PI)
		{
			m_yaw -= XM_PI * 2.0f;
		}
		else if (m_yaw < -XM_PI)
		{
			m_yaw += XM_PI * 2.0f;
		}
	}

	mouse->SetMode(ms.leftButton ? Mouse::MODE_RELATIVE : Mouse::MODE_ABSOLUTE);
}

DirectX::SimpleMath::Quaternion Camera::GetQuaternion() const
{
	return Quaternion::CreateFromYawPitchRoll(m_yaw, -m_pitch, 0.f);
}
