#pragma once

#include "../DXFramework/BaseMesh.h"
#include <vector>

using namespace DirectX;

class CrossTieMesh : public BaseMesh
{

public:
	CrossTieMesh(ID3D11Device* device, ID3D11DeviceContext* deviceContext, unsigned int max_segments);
	void Update();
	void AddCrossTie(XMVECTOR centre, XMVECTOR left, XMVECTOR right, XMVECTOR up, XMVECTOR forward);
	void Clear();
	~CrossTieMesh();

protected:
	void initBuffers(ID3D11Device* device);
	int resolution;

private:
	std::vector<VertexType> vertices_;
	std::vector<unsigned long int> indices_;
	unsigned int cross_tie_count_;
	unsigned int prev_index_count_;
	ID3D11DeviceContext* device_context_;
	unsigned int max_segments_;
};
