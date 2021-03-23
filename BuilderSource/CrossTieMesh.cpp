
#include "CrossTieMesh.h"

// Initialise vertex data, buffers and load texture.
CrossTieMesh::CrossTieMesh(ID3D11Device* device, ID3D11DeviceContext* deviceContext, unsigned int max_segments)
{
	cross_tie_count_ = 0;
	max_segments_ = max_segments;
	
	initBuffers(device);

	device_context_ = deviceContext;

	prev_index_count_ = 0;

}


CrossTieMesh::~CrossTieMesh()
{
	// Run parent deconstructor
	BaseMesh::~BaseMesh();
}

//	Each cross tie has faces consisting of:
//		2 triangles and a rectangle to connect them.
void CrossTieMesh::AddCrossTie(XMVECTOR centre, XMVECTOR left, XMVECTOR right, XMVECTOR up, XMVECTOR forward)
{
	VertexType vertex0, vertex1, vertex2, vertex3;
	XMVECTOR btl, btr, bd;
	btl = centre - forward + left;
	btr = centre - forward + right;
	bd = centre - up;
	XMVECTOR ftl, ftr, fd;
	ftl = centre + forward + left;
	ftr = centre + forward + right;
	fd = centre - up;

	//	Back face.
	vertex0.position = XMFLOAT3(XMVectorGetX(btr), XMVectorGetY(btr), XMVectorGetZ(btr));
	vertex1.position = XMFLOAT3(XMVectorGetX(btl), XMVectorGetY(btl), XMVectorGetZ(btl));
	vertex2.position = XMFLOAT3(XMVectorGetX(bd), XMVectorGetY(bd), XMVectorGetZ(bd));
	
	vertex0.normal = XMFLOAT3(XMVectorGetX(-forward), XMVectorGetY(-forward), XMVectorGetZ(-forward));
	vertex1.normal = XMFLOAT3(XMVectorGetX(-forward), XMVectorGetY(-forward), XMVectorGetZ(-forward));
	vertex2.normal = XMFLOAT3(XMVectorGetX(-forward), XMVectorGetY(-forward), XMVectorGetZ(-forward));
	
	vertex0.texture = XMFLOAT2(0.0f, 0.0f);
	vertex1.texture = XMFLOAT2(1.0f, 0.0f);
	vertex2.texture = XMFLOAT2(0.5f, 0.25f);
	
	vertices_.push_back(vertex0);
	vertices_.push_back(vertex1);
	vertices_.push_back(vertex2);
	
	indices_.push_back(0 + (cross_tie_count_ * 3));
	indices_.push_back(1 + (cross_tie_count_ * 3));
	indices_.push_back(2 + (cross_tie_count_ * 3));
	
	cross_tie_count_++;


	//	front face.
	vertex0.position = XMFLOAT3(XMVectorGetX(ftl), XMVectorGetY(ftl), XMVectorGetZ(ftl));
	vertex1.position = XMFLOAT3(XMVectorGetX(ftr), XMVectorGetY(ftr), XMVectorGetZ(ftr));
	vertex2.position = XMFLOAT3(XMVectorGetX(fd), XMVectorGetY(fd), XMVectorGetZ(fd));

	vertex0.normal = XMFLOAT3(XMVectorGetX(forward), XMVectorGetY(forward), XMVectorGetZ(forward));
	vertex1.normal = XMFLOAT3(XMVectorGetX(forward), XMVectorGetY(forward), XMVectorGetZ(forward));
	vertex2.normal = XMFLOAT3(XMVectorGetX(forward), XMVectorGetY(forward), XMVectorGetZ(forward));

	vertex0.texture = XMFLOAT2(0.0f, 0.0f);
	vertex1.texture = XMFLOAT2(1.0f, 0.0f);
	vertex2.texture = XMFLOAT2(0.5f, 0.25f);

	vertices_.push_back(vertex0);
	vertices_.push_back(vertex1);
	vertices_.push_back(vertex2);

	indices_.push_back(0 + (cross_tie_count_ * 3));
	indices_.push_back(1 + (cross_tie_count_ * 3));
	indices_.push_back(2 + (cross_tie_count_ * 3));

	cross_tie_count_++;



	//	Top face.
	vertex0.position = XMFLOAT3(XMVectorGetX(ftl), XMVectorGetY(ftl), XMVectorGetZ(ftl));
	vertex1.position = XMFLOAT3(XMVectorGetX(ftr), XMVectorGetY(ftr), XMVectorGetZ(ftr));
	vertex2.position = XMFLOAT3(XMVectorGetX(btl), XMVectorGetY(btl), XMVectorGetZ(btl));
	vertex3.position = XMFLOAT3(XMVectorGetX(btr), XMVectorGetY(btr), XMVectorGetZ(btr));

	vertex0.normal = XMFLOAT3(XMVectorGetX(up), XMVectorGetY(up), XMVectorGetZ(up));
	vertex1.normal = XMFLOAT3(XMVectorGetX(up), XMVectorGetY(up), XMVectorGetZ(up));
	vertex2.normal = XMFLOAT3(XMVectorGetX(up), XMVectorGetY(up), XMVectorGetZ(up));
	vertex3.normal = XMFLOAT3(XMVectorGetX(up), XMVectorGetY(up), XMVectorGetZ(up));

	vertex0.texture = XMFLOAT2(0.0f, 0.25f);
	vertex1.texture = XMFLOAT2(1.0f, 0.25f);
	vertex2.texture = XMFLOAT2(0.0f, 0.0f);
	vertex3.texture = XMFLOAT2(1.0f, 0.0f);

	vertices_.push_back(vertex0);
	vertices_.push_back(vertex1);
	vertices_.push_back(vertex2);
	//vertices_.push_back(vertex3);

	indices_.push_back(2 + (cross_tie_count_ * 3));
	indices_.push_back(1 + (cross_tie_count_ * 3));
	indices_.push_back(0 + (cross_tie_count_ * 3));
	cross_tie_count_++;
	
	vertices_.push_back(vertex1);
	vertices_.push_back(vertex2);
	vertices_.push_back(vertex3);

	indices_.push_back(1 + (cross_tie_count_ * 3));
	indices_.push_back(2 + (cross_tie_count_ * 3));
	indices_.push_back(0 + (cross_tie_count_ * 3));

	
	cross_tie_count_++;
}

void CrossTieMesh::initBuffers(ID3D11Device* device)
{
	VertexType* vertices;
	unsigned long* indices;
	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData, indexData;

	
	//	12 vertices/indices per cross tie, 15 cross ties per segment.
	vertexCount = 15 * 12 * max_segments_;
	indexCount = vertexCount;

	// Create the vertex and index array.
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

void CrossTieMesh::Update()
{
	if (vertices_.empty())
	{
		return;
	}

	//	Update the vertex buffer.
	D3D11_MAPPED_SUBRESOURCE vertex_mapped_resource;
	ZeroMemory(&vertex_mapped_resource, sizeof(D3D11_MAPPED_SUBRESOURCE));

	device_context_->Map(vertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &vertex_mapped_resource);
	memcpy(vertex_mapped_resource.pData, &vertices_[0], sizeof(VertexType) * vertices_.size());
	device_context_->Unmap(vertexBuffer, 0);

	//	Update the index buffer.
	if (indices_.size() < prev_index_count_)
	{
		//	The number of vertices has been reduced since the last update. 
		//		So write over the indices for the vertices 'ahead'.
		for (int k = indices_.size(); k < prev_index_count_; k++)
		{
			indices_.push_back(-1);
		}
	}

	prev_index_count_ = indices_.size();

	D3D11_MAPPED_SUBRESOURCE index_mapped_resource;
	ZeroMemory(&index_mapped_resource, sizeof(D3D11_MAPPED_SUBRESOURCE));

	device_context_->Map(indexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &index_mapped_resource);
	memcpy(index_mapped_resource.pData, &indices_[0], sizeof(unsigned long int) * indices_.size());
	device_context_->Unmap(indexBuffer, 0);

	vertices_.clear();
	indices_.clear();
	cross_tie_count_ = 0;
}

void CrossTieMesh::Clear()
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
