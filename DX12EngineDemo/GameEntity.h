#pragma once
#include "Mesh.h"

class GameEntity
{
public:

	GameEntity(Mesh* mesh);
	//GameEntity(const char* objFile, ComPtr<ID3D12Device> device);
	GameEntity(const std::string& filePath, XMFLOAT3 position, ComPtr<ID3D12Device> device);
	void Draw(ComPtr<ID3D12GraphicsCommandList> commandList);
	~GameEntity();

	void UpdateWorldMatrix();

	void Move(float x, float y, float z) { m_position.x += x; m_position.y += y; m_position.z += z; }
	void Rotate(float x, float y, float z) { m_rotation.x += x; m_rotation.y += y; m_rotation.z += z; }

	void SetPosition(float x, float y, float z) { m_position.x = x; m_position.y = y; m_position.z = z; }
	void SetRotation(float x, float y, float z) { m_rotation.x = x; m_rotation.y = y; m_rotation.z = z; }
	void SetScale(float x, float y, float z) { m_scale.x = x; m_scale.y = y, m_scale.z = z; }

	// Getters
	XMFLOAT3 GetPosition() { return m_position; }

	Mesh* GetMesh() { return mesh; }
	XMFLOAT4X4* GetWorldMatrix() { return &m_worldMatrix; }

private:
	Mesh* mesh;
	XMFLOAT4X4 m_worldMatrix;
	XMFLOAT3 m_position;
	XMFLOAT3 m_rotation;
	XMFLOAT3 m_scale;
	ComPtr<ID3D12Device> m_device;

	void LoadModel(const char* objFile, ComPtr<ID3D12Device> device);

	// Assimp related stuff
	std::vector<Mesh> meshes;
	bool LoadAssimpModel(const std::string& filePath);
	void ProcessAssimpNode(aiNode* node, const aiScene* scene);
	Mesh ProcessAssimpMesh(aiMesh* mesh, const aiScene* scene);
};

