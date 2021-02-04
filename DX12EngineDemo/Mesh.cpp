#include "Mesh.h"


Mesh::Mesh(Vertex* vertices, int* indices, int numVertices, int numIndices, ComPtr<ID3D12Device> device)
{
	CreateBuffers(vertices, indices, numVertices, numIndices, device);
}

Mesh::Mesh(const Mesh& mesh)
{
	this->m_indexBuffer = mesh.m_indexBuffer;
	this->m_vertexBuffer = mesh.m_vertexBuffer;
	this->m_vertexBufferView = mesh.m_vertexBufferView;
	this->m_indexBufferView = mesh.m_indexBufferView;
	this->m_indexCount = mesh.m_indexCount;
}

void Mesh::Draw(ComPtr<ID3D12GraphicsCommandList> commandList)
{
	commandList->IASetVertexBuffers(0, 1, &this->m_vertexBufferView);
	commandList->IASetIndexBuffer(&this->m_indexBufferView);
	commandList->DrawIndexedInstanced(this->m_indexCount, 1, 0, 0, 0);
}

Mesh::~Mesh()
{
}

D3D12_VERTEX_BUFFER_VIEW Mesh::GetVertexBufferView()
{

	return m_vertexBufferView;
}

D3D12_INDEX_BUFFER_VIEW Mesh::GetIndexBufferView()
{
	return m_indexBufferView;
}

void Mesh::CreateBuffers(Vertex* vertices, int* indices, int numVertices, int numIndices, ComPtr<ID3D12Device> device)
{
	int vertexBufferSize = sizeof(vertices[0]) * numVertices;

	// TODO: Move this to use Default heap since the vertices will be static
	ThrowIfFailed(device->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer(vertexBufferSize),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&m_vertexBuffer)));

	// Copy the traingle data to the vertex buffer
	UINT8* pVertexDataBegin;
	CD3DX12_RANGE readRange(0, 0);
	ThrowIfFailed(m_vertexBuffer->Map(0, &readRange, reinterpret_cast<void**>(&pVertexDataBegin)));
	memcpy(pVertexDataBegin, vertices, vertexBufferSize);
	m_vertexBuffer->Unmap(0, nullptr);

	m_vertexBufferView.BufferLocation = m_vertexBuffer->GetGPUVirtualAddress();
	m_vertexBufferView.SizeInBytes = vertexBufferSize;
	m_vertexBufferView.StrideInBytes = sizeof(Vertex);

	int indexBufferSize = sizeof(indices[0]) * numIndices;

	// Create Index Buffer and View
	// TODO: Change this to use upload heap to transfer data to default heap
	ThrowIfFailed(device->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer(indexBufferSize),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&m_indexBuffer)));

	// Copy the traingle data to the vertex buffer
	UINT8* pIndexDataBegin;
	CD3DX12_RANGE indexReadRange(0, 0);
	ThrowIfFailed(m_indexBuffer->Map(0, &indexReadRange, reinterpret_cast<void**>(&pIndexDataBegin)));
	memcpy(pIndexDataBegin, indices, indexBufferSize);
	m_indexBuffer->Unmap(0, nullptr);

	m_indexBufferView.BufferLocation = m_indexBuffer->GetGPUVirtualAddress();
	m_indexBufferView.SizeInBytes = indexBufferSize;
	m_indexBufferView.Format = DXGI_FORMAT_R32_UINT;

	m_indexCount = numIndices;
}
