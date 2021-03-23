#include "SupportMesh.h"
#include <math.h>
#include <cmath>

//	Segmented support mesh.
SupportMesh::SupportMesh(ID3D11Device* device, ID3D11DeviceContext* deviceContext, XMVECTOR vertical_from, XMVECTOR vertical_to,
	XMVECTOR angled_from, XMVECTOR angled_to, XMVECTOR angled_x, XMVECTOR angled_z)
{
	vertical_from_ = vertical_from;
	vertical_to_ = vertical_to;
	angled_from_ = angled_from;
	angled_to_ = angled_to;
	angled_x_ = angled_x;
	angled_z_ = angled_z;

	XMVECTOR x = XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f);
	XMVECTOR z = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
	
	AddCircleOrigin(angled_from, angled_x, angled_z);
	AddCircleOrigin(angled_to, angled_x, angled_z);

	AddCircleOrigin(vertical_from, x, z);
	AddCircleOrigin(vertical_to, x, z);

	device_context_ = deviceContext;

	radius_ = 0.2f;
	slice_count_ = 8;

	initBuffers(device);
}

//	Vertical support mesh.
SupportMesh::SupportMesh(ID3D11Device* device, ID3D11DeviceContext* deviceContext, XMVECTOR vertical_from, XMVECTOR vertical_to)
{
	vertical_from_ = vertical_from;
	vertical_to_ = vertical_to;

	XMVECTOR x = XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f);
	XMVECTOR z = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);

	AddCircleOrigin(vertical_from, x, z);
	AddCircleOrigin(vertical_to, x, z);

	device_context_ = deviceContext;

	radius_ = 0.2f;
	slice_count_ = 10;

	initBuffers(device);
}

SupportMesh::~SupportMesh()
{
	// Run parent deconstructor
	BaseMesh::~BaseMesh();
}

// Initialise geometry buffers (vertex and index).
void SupportMesh::initBuffers(ID3D11Device* device)
{
	CalculateVertices();
	CalculateIndices();

	VertexType* vertices;
	unsigned long* indices;
	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData, indexData;

	vertexCount = vertices_.size();	
	indexCount = indices_.size();

	vertices = new VertexType[vertexCount];
	indices = new unsigned long[indexCount];

	for (int i = 0; i < vertexCount; i++)
	{
		vertices[i] = vertices_[i];
	}

	for (int i = 0; i < indexCount; i++)
	{
		indices[i] = indices_[i];
	}

	// Set up the description of the static vertex buffer.
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(VertexType) * vertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;
	// Give the subresource structure a pointer to the vertex data.
	vertexData.pSysMem = vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;
	// Now create the vertex buffer.
	device->CreateBuffer(&vertexBufferDesc, &vertexData, &vertexBuffer);

	// Set up the description of the static index buffer.
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(unsigned long) * indexCount;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;
	// Give the subresource structure a pointer to the index data.
	indexData.pSysMem = indices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;
	// Create the index buffer.
	device->CreateBuffer(&indexBufferDesc, &indexData, &indexBuffer);

	// Release the arrays now that the vertex and index buffers have been created and loaded.
	delete[] vertices;
	vertices = 0;

	delete[] indices;
	indices = 0;
}

void SupportMesh::CalculateVertices()
{
	float slice_angle = 2.0f * 3.14159265359f / slice_count_;

	for (int j = 0; j < circle_data_.size(); j++)
	{
		for (int i = 0; i <= slice_count_; i++)
		{
			VertexType vertex;
			XMVECTOR pos = circle_data_[j].centre + (radius_ * cosf(slice_angle * i) * circle_data_[j].x_axis)
				+ (radius_ * sinf(slice_angle * i) * circle_data_[j].y_axis);
			vertex.position = XMFLOAT3(XMVectorGetX(pos), XMVectorGetY(pos), XMVectorGetZ(pos));

			vertex.texture = XMFLOAT2(((float)i / slice_count_), (1.0f - ((float)j / circle_data_.size())) * 40.0f);

			XMVECTOR normal = XMVector3Normalize(pos - circle_data_[j].centre);
			vertex.normal = XMFLOAT3(XMVectorGetX(normal), XMVectorGetY(normal), XMVectorGetZ(normal));

			vertices_.push_back(vertex);
		}
	}
}

void SupportMesh::CalculateIndices()
{
	for (int i = 0; i < circle_data_.size() - 1; i++)
	{
		for (int j = 0; j < slice_count_; j++)
		{
			indices_.push_back(i * (slice_count_ + 1) + j);
			indices_.push_back((i + 1) * (slice_count_ + 1) + j);
			indices_.push_back((i + 1) * (slice_count_ + 1) + (j + 1));

			indices_.push_back(i * (slice_count_ + 1) + j);
			indices_.push_back((i + 1) * (slice_count_ + 1) + (j + 1));
			indices_.push_back(i * (slice_count_ + 1) + (j + 1));
		}
	}
}

void SupportMesh::AddCircleOrigin(XMVECTOR centre, XMVECTOR x_axis, XMVECTOR y_axis)
{
	CircleData circle;

	circle.centre = centre;
	circle.x_axis = x_axis;
	circle.y_axis = y_axis;

	circle_data_.push_back(circle);
}

void SupportMesh::sendData(ID3D11DeviceContext* deviceContext)
{
	unsigned int stride;
	unsigned int offset;

	// Set vertex buffer stride and offset.
	stride = sizeof(VertexType);
	offset = 0;

	deviceContext->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
	deviceContext->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

}







