#pragma once
#include "DXSample.h"
#include <DirectXMath.h>

using namespace DirectX;
class Camera
{
public:
	Camera(float x, float y, float z);
	~Camera();

	// Transformations
	void MoveRelative(float x, float y, float z);
	void MoveAbsolute(float x, float y, float z);
	void Rotate(float x, float y);

	// Updating
	void Update(float dt);
	void UpdateViewMatrix();
	void UpdateProjectionMatrix(float aspectRatio);

	// Getters
	XMFLOAT3 GetPosition() { return m_position; }
	XMFLOAT4X4 GetViewMatrix() { return m_viewMatrix; }
	XMFLOAT4X4 GetProjectionMatrix() { return m_projectionMatrix; }


private:
	XMFLOAT4X4 m_viewMatrix;
	XMFLOAT4X4 m_projectionMatrix;

	XMFLOAT3 m_position;
	XMFLOAT3 m_startPosition;
	XMFLOAT4 m_rotation;
	float m_xRotation;
	float m_yRotation;
};

