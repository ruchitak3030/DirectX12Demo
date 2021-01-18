#include "Camera.h"

Camera::Camera(float x, float y, float z)
{
	m_position = XMFLOAT3(x, y, z);
	m_startPosition = XMFLOAT3(x, y, z);
	XMStoreFloat4(&m_rotation, XMQuaternionIdentity());
	m_xRotation = 0;
	m_yRotation = 0;

	XMStoreFloat4x4(&m_viewMatrix, XMMatrixIdentity());
	XMStoreFloat4x4(&m_projectionMatrix, XMMatrixIdentity());

}

Camera::~Camera()
{
}

void Camera::MoveRelative(float x, float y, float z)
{
	XMVECTOR dir = XMVector3Rotate(XMVectorSet(x, y, z, 0), XMLoadFloat4(&m_rotation));

	XMStoreFloat3(&m_position, XMLoadFloat3(&m_position) + dir);
}

void Camera::MoveAbsolute(float x, float y, float z)
{
	m_position.x += x;
	m_position.y += y;
	m_position.z += z;
}

void Camera::Rotate(float x, float y)
{
	m_xRotation += x;
	m_yRotation += y;

	// Clamp the x between PI/2 and -PI/2
	m_xRotation = max(min(m_xRotation, XM_PIDIV2), -XM_PIDIV2);

	// Recreate the quaternion
	XMStoreFloat4(&m_rotation, XMQuaternionRotationRollPitchYaw(m_xRotation, m_yRotation, 0));
}

// Looks for key presses
void Camera::Update(float dt)
{
	// Current speed 
	float speed = dt * 3;

	// Speed up or down as necessary
	if (GetAsyncKeyState(VK_SHIFT)) { speed *= 5; }
	if (GetAsyncKeyState(VK_CONTROL)) { speed *= 0.1f; }

	/// Movement
	if (GetAsyncKeyState('W') & 0x8000) { MoveRelative(0, 0, speed); }
	if (GetAsyncKeyState('S') & 0x8000) { MoveRelative(0, 0, -speed); }
	if (GetAsyncKeyState('A') & 0x8000) { MoveRelative(-speed, 0, 0); }
	if (GetAsyncKeyState('D') & 0x8000) { MoveRelative(speed, 0, 0); }
	if (GetAsyncKeyState('X') & 0x8000) { MoveAbsolute(0, -speed, 0); }
	if (GetAsyncKeyState(' ') & 0x8000) { MoveAbsolute(0, speed, 0); }

	// Check for reset
	if (GetAsyncKeyState('R') & 0x8000)
	{
		m_position = m_startPosition;
		m_xRotation = 0;
		m_yRotation = 0;
		XMStoreFloat4(&m_rotation, XMQuaternionIdentity());
	}

	// Update the view every frame - could be optimized
	UpdateViewMatrix();

}

void Camera::UpdateViewMatrix()
{
	XMVECTOR dir = XMVector3Rotate(XMVectorSet(0, 0, 1, 0), XMLoadFloat4(&m_rotation));

	XMMATRIX view = XMMatrixLookToLH(
		XMLoadFloat3(&m_position),
		dir,
		XMVectorSet(0, 1, 0, 0));

	XMStoreFloat4x4(&m_viewMatrix, XMMatrixTranspose(view));
}

void Camera::UpdateProjectionMatrix(float aspectRatio)
{
	XMMATRIX proj = XMMatrixPerspectiveFovLH(
		XM_PIDIV4,
		aspectRatio,
		0.1f,
		100.0f);

	XMStoreFloat4x4(&m_projectionMatrix, XMMatrixTranspose(proj));
}
