
#pragma once

#include "../DXFramework/BaseMesh.h"
#include <vector>

using namespace DirectX;

class PipeMesh : public BaseMesh
{

public:
	PipeMesh(ID3D11Device* device, ID3D11DeviceContext* deviceContext, float radius, unsigned int max_segments, unsigned int slice_count = 10);
	void Update();
	void AddCircleOrigin(XMVECTOR centre, XMVECTOR x_axis, XMVECTOR y_axis);
	void CalculateVertices();
	void CalculateIndices();
	void Clear();
	//inline void AllowIndicesOverride() { allow_indices_override_ = true; }
	inline void SetSliceCount(int slice_count) { slice_count_ = slice_count; }
	void sendData(ID3D11DeviceContext* deviceContext);
	~PipeMesh();

protected:
	void initBuffers(ID3D11Device* device);
	int resolution;

private:
	struct CircleData
	{
		XMVECTOR centre;
		XMVECTOR x_axis;
		XMVECTOR y_axis;
	};

	ID3D11DeviceContext* device_context_;
	std::vector<VertexType> vertices_;
	std::vector<unsigned long int> indices_;
	unsigned int prev_index_count_;
	std::vector<CircleData> circle_data_;
	unsigned int slice_count_;
	float radius_;
	unsigned int max_segments_;
};

