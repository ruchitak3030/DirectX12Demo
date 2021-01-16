#pragma once

#include "HeaderHelper.h"
#include "DXSampleHelper.h"
#include <DirectXMath.h>
#include "Vertex.h"

using namespace Microsoft::WRL;

class Mesh
{
public:
	Mesh(Vertex* vertices, int* indices, int numVertices, int numIndices, ComPtr<ID3D12Device> device);
	~Mesh();

	UINT m_indexCount;


	D3D12_VERTEX_BUFFER_VIEW GetVertexBufferView();
	D3D12_INDEX_BUFFER_VIEW GetIndexBufferView();	

private:
	ComPtr<ID3D12Resource> m_vertexBuffer;
	D3D12_VERTEX_BUFFER_VIEW m_vertexBufferView;
	ComPtr<ID3D12Resource> m_indexBuffer;
	D3D12_INDEX_BUFFER_VIEW m_indexBufferView;

};

