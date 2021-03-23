
#pragma once

#include "../DXFramework/BaseMesh.h"

using namespace DirectX;

class CylinderMesh : public BaseMesh
{

public:
	CylinderMesh(ID3D11Device* device, ID3D11DeviceContext* deviceContext, int resolution = 20);
	~CylinderMesh();

protected:
	void initBuffers(ID3D11Device* device);
	int resolution;
};

