#pragma once

#include "../DXFramework/BaseMesh.h"
#include <vector>

struct Line
{
	XMFLOAT3 start;
	XMFLOAT3 end;
	XMFLOAT3 colour;
};

//	For straight line lists.
class LineMesh : public BaseMesh
{
public:
	LineMesh(ID3D11Device* device, ID3D11DeviceContext* device_context, unsigned int max_vertices);
	void sendData(ID3D11DeviceContext* deviceContext);
	void AddLine(Line& line);
	void Clear();
	~LineMesh();

protected:
	void initBuffers(ID3D11Device* device);
	void UpdateVertexBuffer();

private:
	std::vector<Line> lines_;
	ID3D11Device* device_;
	ID3D11DeviceContext* device_context_;
	unsigned int max_vertices_;

};