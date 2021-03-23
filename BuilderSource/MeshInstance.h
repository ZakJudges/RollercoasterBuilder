#pragma once

#include "../DXFramework/Geometry.h"
#include <DirectXMath.h>
#include "ColourShader.h"
#include "DefaultShader.h"

class MeshInstance
{
public:
	MeshInstance(ID3D11ShaderResourceView* texture, BaseShader* shader, BaseMesh* mesh);
	MeshInstance(BaseShader* shader, BaseMesh* mesh);
	~MeshInstance();
	bool Render(ID3D11DeviceContext* context, XMMATRIX& view, XMMATRIX& projection);
	void SetWorldMatrix(XMMATRIX wm);
	XMMATRIX GetWorldMatrix();
	void SetColour(XMFLOAT4 col);
	void SetTexture(ID3D11ShaderResourceView* texture);
	inline void SetRender(bool render) { render_ = render; }
private:
	bool render_;
protected:
	XMMATRIX world_matrix_;
	XMMATRIX scale_matrix_;
	BaseShader* shader_;
	ID3D11ShaderResourceView* texture_;
	BaseMesh* mesh_;
	XMFLOAT4 colour_;	
};