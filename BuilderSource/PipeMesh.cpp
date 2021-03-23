#include "PipeMesh.h"
#include <math.h>
#include <cmath>

PipeMesh::PipeMesh(ID3D11Device* device, ID3D11DeviceContext* deviceContext, float radius, unsigned int max_segments, unsigned int slice_count)
{
	device_context_ = deviceContext;
	radius_ = radius;
	slice_count_ = slice_count;
	max_segments_ = max_segments;

	initBuffers(device);

	prev_index_count_ = 0;

	//allow_indices_override_ = false;
}

PipeMesh::~PipeMesh()
{
	// Run parent deconstructor
	BaseMesh::~BaseMesh();
}

void PipeMesh::initBuffers(ID3D11Device* device)
{
	VertexType* vertices;
	unsigned long* indices;
	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData, indexData;

	//	30 Circles per segment.
	vertexCount = 30 * (slice_count_+1) * max_segments_;
	indexCount = 30 * slice_count_ * 6 * max_segments_;

	vertices = new VertexType[vertexCount];
	indices = new unsigned long[indexCount];

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
	device->CreateBuffer(&vertexBufferDesc, &vertexData, &vertexBuffer);

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
	device->CreateBuffer(&indexBufferDesc, &indexData, &indexBuffer);

	// Release the arrays now that the vertex and index buffers have been created and loaded.
	delete[] vertices;
	vertices = 0;

	delete[] indices;
	indices = 0;
}

void PipeMesh::Update()
{
	CalculateVertices();

	if (vertices_.empty() || (vertices_.size() > vertexCount))
	{
		return;
	}

	//	Update the vertex buffer.
	D3D11_MAPPED_SUBRESOURCE vertex_mapped_resource;
	ZeroMemory(&vertex_mapped_resource, sizeof(D3D11_MAPPED_SUBRESOURCE));
	
	device_context_->Map(vertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &vertex_mapped_resource);
	memcpy(vertex_mapped_resource.pData, &vertices_[0], sizeof(VertexType) * vertices_.size());
	device_context_->Unmap(vertexBuffer, 0);


	CalculateIndices();

	if (indices_.size() > indexCount)
	{
		return;
	}

	//	Update the index buffer.
	D3D11_MAPPED_SUBRESOURCE index_mapped_resource;
	ZeroMemory(&index_mapped_resource, sizeof(D3D11_MAPPED_SUBRESOURCE));

	device_context_->Map(indexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &index_mapped_resource);
	memcpy(index_mapped_resource.pData, &indices_[0], sizeof(unsigned long int) * indices_.size());
	device_context_->Unmap(indexBuffer, 0);


	circle_data_.clear();
	vertices_.clear();
	indices_.clear();
}

void PipeMesh::Clear()
{
	//	Update the index buffer.
	D3D11_MAPPED_SUBRESOURCE index_mapped_resource;
	ZeroMemory(&index_mapped_resource, sizeof(D3D11_MAPPED_SUBRESOURCE));
	
	std::vector<unsigned long> indices;
	for (int i = 0; i < indexCount; i++)
	{
		indices.push_back(-1);
	}

	device_context_->Map(indexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &index_mapped_resource);

	memcpy(index_mapped_resource.pData, &indices[0], sizeof(unsigned long) * indexCount);

	device_context_->Unmap(indexBuffer, 0);

	prev_index_count_ = 0;

}

void PipeMesh::CalculateVertices()
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

void PipeMesh::CalculateIndices()
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

	//	Ensure that if the number of indices have decreased, the old indices are overwritten.
	if (indices_.size() < prev_index_count_)
	{
		for (int k = indices_.size()-1; k < prev_index_count_; k++)
		{
			indices_.push_back(-1);
		}
	}

	prev_index_count_ = indices_.size();
}

void PipeMesh::AddCircleOrigin(XMVECTOR centre, XMVECTOR x_axis, XMVECTOR y_axis)
{
	CircleData circle;
	
	circle.centre = centre;
	circle.x_axis = x_axis;
	circle.y_axis = y_axis;

	circle_data_.push_back(circle);
}

void PipeMesh::sendData(ID3D11DeviceContext* deviceContext)
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



