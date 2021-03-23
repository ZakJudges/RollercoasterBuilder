//	Class for the support structure of the track.
//		Stores mesh data.
//		Each support mesh consists of  1 or 2 strucres.
//		The first structure is optional and can be any orientation.
//		The second structure is mandatory and is always vertical.
//		A track piece will only have two support structure segments if it cannot be supported by a single vertical segment 
//			(It is upside down)
#pragma once

#include "../DXFramework/BaseMesh.h"
#include <vector>

using namespace DirectX;

class SupportMesh : public BaseMesh
{

public:
	SupportMesh(ID3D11Device* device, ID3D11DeviceContext* deviceContext, XMVECTOR vertical_from_, XMVECTOR vertical_to_,
		XMVECTOR angled_from_, XMVECTOR angled_to_, XMVECTOR angled_x_, XMVECTOR angled_z_);
	SupportMesh(ID3D11Device* device, ID3D11DeviceContext* deviceContext, XMVECTOR vertical_from_, XMVECTOR vertical_to_);
	void AddCircleOrigin(XMVECTOR centre, XMVECTOR x_axis, XMVECTOR y_axis);
	void CalculateVertices();
	void CalculateIndices();
	inline void SetSliceCount(int slice_count) { slice_count_ = slice_count; }
	void sendData(ID3D11DeviceContext* deviceContext);
	virtual ~SupportMesh();

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
	std::vector<CircleData> circle_data_;
	ID3D11DeviceContext* device_context_;
	std::vector<VertexType> vertices_;
	std::vector<unsigned long int> indices_;
	unsigned int slice_count_;
	float radius_;
	XMVECTOR vertical_from_;
	XMVECTOR vertical_to_;
	XMVECTOR angled_from_;
	XMVECTOR angled_to_;
	XMVECTOR angled_x_;
	XMVECTOR angled_z_;
};

