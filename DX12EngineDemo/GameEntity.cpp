#include "GameEntity.h"
#include <fstream>

GameEntity::GameEntity(Mesh* mesh)
{
	this->mesh = mesh;
	XMStoreFloat4x4(&m_worldMatrix, XMMatrixIdentity());
	m_position = XMFLOAT3(0, 0, 0);
	m_rotation = XMFLOAT3(0, 0, 0);
	m_scale = XMFLOAT3(1, 1, 1);
}

GameEntity::GameEntity(const char* objFile, ComPtr<ID3D12Device> device)
{
	
	LoadModel(objFile, device);
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

void GameEntity::LoadModel(const char* objFile, ComPtr<ID3D12Device> device)
{
	// File input object
	std::ifstream obj(objFile);

	// Check for successful open
	if (!obj.is_open())
	{
		// Check the debug Folder 
		char debugFolder[256] = {};
		strcat_s(debugFolder, "Debug/");
		strcat_s(debugFolder, objFile);

		// Attempt to open again
		obj.open(debugFolder);

		// If not found, give up
		if (!obj.is_open())
		{
			return;
		}
	}

	// Varibales used while reading the file
	std::vector<XMFLOAT3> position;
	std::vector<XMFLOAT3> normals;
	std::vector<XMFLOAT2> uvs;
	std::vector<Vertex> vertices;
	std::vector<int> indices;
	int numVertices = 0;
	char chars[100];

	while (obj.good())
	{
		// Get the line
		obj.getline(chars, 100);

		// Check the type of the line
		// Normals
		if (chars[0] == 'v' && chars[1] == 'n')
		{
			XMFLOAT3 norm;
			sscanf_s(
				chars,
				"vn %f %f %f",
				&norm.x, &norm.y, &norm.z);

			// Add the normals to the list
			normals.push_back(norm);
		}
		else if (chars[0] == 'v' && chars[1] == 't')		// Uvs
		{
			XMFLOAT2 uv;
			sscanf_s(
				chars,
				"vt %f %f",
				&uv.x, &uv.y);

			uvs.push_back(uv);
		}
		else if (chars[0] == 'v')		// position
		{
			XMFLOAT3 pos;
			sscanf_s(
				chars,
				"v %f %f %f",
				&pos.x, &pos.y, &pos.z);

			position.push_back(pos);
		}
		else if (chars[0] == 'f')		// face indices
		{
			unsigned int i[12];
			int facesRead = sscanf_s(
				chars,
				"f %d/%d/%d %d/%d/%d %d/%d/%d %d/%d/%d",
				&i[0], &i[1], &i[2],
				&i[3], &i[4], &i[5],
				&i[6], &i[7], &i[8],
				&i[9], &i[10], &i[11]);

			Vertex v1;
			v1.position = position[i[0] - 1];
			v1.uv = uvs[i[1] - 1];
			v1.normal = normals[i[2] - 1];

			Vertex v2;
			v2.position = position[i[3] - 1];
			v2.uv = uvs[i[4] - 1];
			v2.normal = normals[i[5] - 1];

			Vertex v3;
			v3.position = position[i[6] - 1];
			v3.uv = uvs[i[7] - 1];
			v3.normal = normals[i[8] - 1];

			// The model is most likely in a right-handed space,
			// especially if it came from Maya.  We want to convert
			// to a left-handed space for DirectX.  This means we 
			// need to:
			//  - Invert the Z position
			//  - Invert the normal's Z
			//  - Flip the winding order
			// We also need to flip the UV coordinate since DirectX
			// defines (0,0) as the top left of the texture, and many
			// 3D modeling packages use the bottom left as (0,0)

			// Flip the UV's since they're probably "upside down"
			v1.uv.y = 1.0f - v1.uv.y;
			v2.uv.y = 1.0f - v2.uv.y;
			v3.uv.y = 1.0f - v3.uv.y;

			// Flip Z (LH vs. RH)
			v1.position.z *= -1.0f;
			v2.position.z *= -1.0f;
			v3.position.z *= -1.0f;

			// Flip normal Z
			v1.normal.z *= -1.0f;
			v2.normal.z *= -1.0f;
			v3.normal.z *= -1.0f;

			// Add the verts to the vector (flipping the winding order)
			vertices.push_back(v1);
			vertices.push_back(v3);
			vertices.push_back(v2);

			// Add three more indices
			indices.push_back(numVertices); numVertices += 1;
			indices.push_back(numVertices); numVertices += 1;
			indices.push_back(numVertices); numVertices += 1;

			// Was there a 4th face?
			if (facesRead == 12)
			{
				// Make the last vertex
				Vertex v4;
				v4.position = position[i[9] - 1];
				v4.uv = uvs[i[10] - 1];
				v4.normal = normals[i[11] - 1];

				// Flip the UV, Z pos and normal
				v4.uv.y = 1.0f - v4.uv.y;
				v4.position.z *= -1.0f;
				v4.normal.z *= -1.0f;

				// Add a whole triangle (flipping the winding order)
				vertices.push_back(v1);
				vertices.push_back(v4);
				vertices.push_back(v3);

				// Add three more indices
				indices.push_back(numVertices); numVertices += 1;
				indices.push_back(numVertices); numVertices += 1;
				indices.push_back(numVertices); numVertices += 1;
			}
		}
	}

	// CLose the file and create the actual buffers
	obj.close();

	mesh = new Mesh(&vertices[0], &indices[0], numVertices, numVertices, device);
}
