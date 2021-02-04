#pragma once

#include "HeaderHelper.h"
#include "DXSampleHelper.h"
#include <DirectXMath.h>
#include "Vertex.h"
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>


using namespace Microsoft::WRL;

class Mesh
{
public:
	Mesh(Vertex* vertices, int* indices, int numVertices, int numIndices, ComPtr<ID3D12Device> device);
	Mesh(const Mesh& mesh);
	void Draw(ComPtr<ID3D12GraphicsCommandList> commandList);
	~Mesh();

	UINT m_indexCount;


	D3D12_VERTEX_BUFFER_VIEW GetVertexBufferView();
	D3D12_INDEX_BUFFER_VIEW GetIndexBufferView();	
	UINT GetIndexCount() { return m_indexCount; }

private:
	ComPtr<ID3D12Resource> m_vertexBuffer;
	D3D12_VERTEX_BUFFER_VIEW m_vertexBufferView;
	ComPtr<ID3D12Resource> m_indexBuffer;
	D3D12_INDEX_BUFFER_VIEW m_indexBufferView;

	void CreateBuffers(Vertex* vertices, int* indices, int numVertices, int numIndices, ComPtr<ID3D12Device> device);

};

