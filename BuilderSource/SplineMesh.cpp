#include "SplineMesh.h"

//	Line in 3D space to draw a spline.
SplineMesh::SplineMesh(ID3D11Device* device, ID3D11DeviceContext* device_context, int resolution) : device_(device), device_context_(device_context)
{
	resolution_ = resolution;
	initBuffers(device);
}

SplineMesh::~SplineMesh()
{
	BaseMesh::~BaseMesh();
}

void SplineMesh::initBuffers(ID3D11Device* device)
{
	//	Create vertices for mesh 
	VertexType* vertices;
	unsigned long* indices;
	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData, indexData;
	HRESULT result;

	vertexCount = resolution_;
	indexCount = vertexCount;

	vertices = new VertexType[vertexCount];
	indices = new unsigned long[indexCount];

	for (int i = 0; i < resolution_; i++)
	{
		//	Set colour and index.
		vertices[i].normal = XMFLOAT3(0.0f, 1.0f, 0.0f);
		indices[i] = i;
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

void SplineMesh::sendData(ID3D11DeviceContext* deviceContext)
{
	unsigned int stride;
	unsigned int offset;

	// Set vertex buffer stride and offset.
	stride = sizeof(VertexType);
	offset = 0;

	deviceContext->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
	deviceContext->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);
	deviceContext->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_LINESTRIP);
}

void SplineMesh::Update(SL::CRSplineController* spline_controller)
{
	D3D11_MAPPED_SUBRESOURCE vertex_mapped_resource;

	VertexType* vertices;// = new VertexType[resolution_];

	device_context_->Map(vertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &vertex_mapped_resource);

	//	Update vertex and index data here.
	vertices = (VertexType*)vertex_mapped_resource.pData;

	float t = 0.0f;
	for (int i = 0; i < resolution_; i++)
	{
		SL::Vector point;
	
		point = spline_controller->GetPoint(t);

		vertices[i].position = XMFLOAT3(point.X(), point.Y(), point.Z());
		vertices[i].normal = XMFLOAT3(0.0f, 1.0f, 0.0f);
		t += (1.0f / resolution_);
	}

	device_context_->Unmap(vertexBuffer, 0);
}

