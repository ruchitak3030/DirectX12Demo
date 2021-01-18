#include "GameEntity.h"

GameEntity::GameEntity(Mesh* mesh)
{
	this->mesh = mesh;
	XMStoreFloat4x4(&m_worldMatrix, XMMatrixIdentity());
	m_position = XMFLOAT3(0, 0, 0);
	m_rotation = XMFLOAT3(0, 0, 0);
	m_scale = XMFLOAT3(1, 1, 1);
}

GameEntity::~GameEntity()
{
}

void GameEntity::UpdateWorldMatrix()
{
	XMMATRIX translation = XMMatrixTranslation(m_position.x, m_position.y, m_position.z);
	XMMATRIX xRotation = XMMatrixRotationX(m_rotation.x);
	XMMATRIX yRotation = XMMatrixRotationY(m_rotation.y);
	XMMATRIX zRotation = XMMatrixRotationZ(m_rotation.z);
	XMMATRIX scale = XMMatrixScaling(m_scale.x, m_scale.y, m_scale.z);

	XMMATRIX total = scale * zRotation * yRotation * xRotation;
	XMStoreFloat4x4(&m_worldMatrix, XMMatrixTranspose(total));
}
