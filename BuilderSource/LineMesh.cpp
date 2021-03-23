#include "LineMesh.h"

LineMesh::LineMesh(ID3D11Device* device, ID3D11DeviceContext* device_context, unsigned int max_vertices) : device_(device), device_context_(device_context), max_vertices_(max_vertices)
{
	initBuffers(device_);
}

LineMesh::~LineMesh()
{
	BaseMesh::~BaseMesh();
}

void LineMesh::AddLine(Line& line)
{
	lines_.push_back(line);
	UpdateVertexBuffer();
}

void LineMesh::Clear()
{
	lines_.clear();
	UpdateVertexBuffer();
}

void LineMesh::initBuffers(ID3D11Device* device)
{
	//	Create vertices for mesh here. TEMP: Triangle mesh.
	VertexType* vertices;
	unsigned long* indices;
	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData, indexData;
	HRESULT result;

	vertexCount = max_vertices_;
	indexCount = vertexCount;

	vertices = new VertexType[vertexCount];
	indices = new unsigned long[indexCount];

	int index = 0;
	for (int i = 0; i < lines_.size(); i++)
	{
		vertices[index].position = lines_[i].start;
		vertices[index].normal = lines_[i].colour;

		vertices[index + 1].position = lines_[i].end;
		vertices[index + 1].normal = lines_[i].colour;

		index += 2;
	}

	for (int j = 0; j < max_vertices_; j++)
	{
		indices[j] = j;
	}

	// Set up the description of the static vertex buffer.
	vertexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	vertexBufferDesc.ByteWidth = sizeof(VertexType) * vertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;
	// Give the subresource structure a pointer to the vertex data.
	vertexData.pSysMem = vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;
	// Now create the vertex buffer.
	result = device->CreateBuffer(&vertexBufferDesc, &vertexData, &vertexBuffer);

	// Set up the description of the static index buffer.
	indexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	indexBufferDesc.ByteWidth = sizeof(unsigned long) * indexCount;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;
	// Give the subresource structure a pointer to the index data.
	indexData.pSysMem = indices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;
	
	// Create the index buffer.
	result = device->CreateBuffer(&indexBufferDesc, &indexData, &indexBuffer);
	
	// Release the arrays now that the vertex and index buffers have been created and loaded.
	delete[] vertices;
	vertices = 0;
	delete[] indices;
	indices = 0;

	
}

void LineMesh::UpdateVertexBuffer()
{
	D3D11_MAPPED_SUBRESOURCE vertex_mapped_resource;

	VertexType* vertices;// = new VertexType[max_vertices_];

	device_context_->Map(vertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &vertex_mapped_resource);
	
	//	Update vertex and index data.
	vertices = (VertexType*)vertex_mapped_resource.pData;

	int index = 0;
	for (int i = 0; i < lines_.size(); i++)
	{
		vertices[index].position = lines_[i].start;
		vertices[index].normal = lines_[i].colour;

		vertices[index + 1].position = lines_[i].end;
		vertices[index + 1].normal = lines_[i].colour;

		index += 2;
	}
	
	device_context_->Unmap(vertexBuffer, 0);
}

void LineMesh::sendData(ID3D11DeviceContext* deviceContext)
{
	unsigned int stride;
	unsigned int offset;

	// Set vertex buffer stride and offset.
	stride = sizeof(VertexType);
	offset = 0;

	deviceContext->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
	deviceContext->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);
	deviceContext->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_LINELIST);

}
