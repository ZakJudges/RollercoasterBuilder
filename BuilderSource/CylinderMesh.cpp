
#include "CylinderMesh.h"
#include <math.h>
#include <vector>

// Initialise vertex data, buffers and load texture.
CylinderMesh::CylinderMesh(ID3D11Device* device, ID3D11DeviceContext* deviceContext, int lresolution)
{
	resolution = lresolution;
	initBuffers(device);
}


CylinderMesh::~CylinderMesh()
{
	// Run parent deconstructor
	BaseMesh::~BaseMesh();
}

void CylinderMesh::initBuffers(ID3D11Device* device)
{
	VertexType* vertices;
	unsigned long* indices;
	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData, indexData;
	
	int stack_count = 20;
	int slice_count = 20;
	float height = 10.0f;
	float bottom_radius = 0.2;
	float top_radius = 4;

	//	The height of each stack.
	float stack_height = height / stack_count;

	//	The amount to increment the radius by each time we move up a stack.
	//		0 = symmetrical cylinder.
	float radius_step = (top_radius - bottom_radius) / stack_count;

	//	The number of rings.
	int ring_count = stack_count + 1;

	std::vector<VertexType> mesh_verts;
	std::vector<unsigned long int> mesh_indices;

	//	Calculate the vertices of each ring.
	//		Centered at (0, 0, 0).
	for (int i = 0; i < ring_count; ++i)
	{
		float y = (-0.5f * height) + (i * stack_height);

		float radius = bottom_radius + (i * radius_step);

		float slice_angle = (2.0f * 3.14159265359f) / slice_count;

		for (int j = 0; j <= slice_count; ++j)
		{
			float cos_angle = cosf(j * slice_angle);
			float sin_angle = sinf(j * slice_angle);

			XMFLOAT3 tangent = XMFLOAT3(-sin_angle, 0.0f, cos_angle);
			float radius_diff = bottom_radius - top_radius;
			XMFLOAT3 bi_tangent = XMFLOAT3(radius_diff * cos_angle, -height, radius_diff * sin_angle);

			XMVECTOR t = XMLoadFloat3(&tangent);
			XMVECTOR b = XMLoadFloat3(&bi_tangent);
			XMVECTOR n = XMVector3Normalize(XMVector3Cross(t, b));

			VertexType v;
			v.position = XMFLOAT3(radius * cos_angle, y, radius * sin_angle);
			v.texture = XMFLOAT2((float)j / slice_count, 1.0f - (float)i / stack_count);
			v.normal = XMFLOAT3(XMVectorGetX(n), XMVectorGetY(n), XMVectorGetZ(n));

			mesh_verts.push_back(v);
		}
	}

	//	Calculate the indices for each stack.
	int ring_vertex_count = slice_count + 1;

	for (int i = 0; i < stack_count; ++i)
	{
		for (int j = 0; j < slice_count; ++j)
		{
			mesh_indices.push_back(i * ring_vertex_count + j);
			mesh_indices.push_back((i + 1) * ring_vertex_count + j);
			mesh_indices.push_back((i + 1) * ring_vertex_count + j + 1);
			mesh_indices.push_back(i * ring_vertex_count + j);
			mesh_indices.push_back((i + 1) * ring_vertex_count + j + 1);
			mesh_indices.push_back(i * ring_vertex_count + j + 1);

			mesh_indices.push_back(i * ring_vertex_count + j + 1);
			mesh_indices.push_back((i + 1) * ring_vertex_count + j + 1);
			mesh_indices.push_back(i * ring_vertex_count + j);
			mesh_indices.push_back((i + 1) * ring_vertex_count + j + 1);
			mesh_indices.push_back((i + 1) * ring_vertex_count + j);
			mesh_indices.push_back(i * ring_vertex_count + j);	
		}
	} 

	vertexCount = mesh_verts.size();
	indexCount = mesh_indices.size();

	// Create the vertex and index array.
	vertices = new VertexType[vertexCount];
	indices = new unsigned long[indexCount];

	for (int i = 0; i < mesh_verts.size(); i++)
	{
		vertices[i] = mesh_verts[i];
	}

	for (int i = 0; i < mesh_indices.size(); i++)
	{
		indices[i] = mesh_indices[i];
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

