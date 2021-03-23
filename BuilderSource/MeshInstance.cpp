#include "MeshInstance.h"

MeshInstance::MeshInstance(ID3D11ShaderResourceView* texture, BaseShader* shader, BaseMesh* mesh) : 
	world_matrix_(DirectX::XMMatrixIdentity()), texture_(texture), shader_(shader), mesh_(mesh)
{
	shader_->SetTexture(texture_);
	render_ = true;
	SetColour(XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f));
}

MeshInstance::MeshInstance(BaseShader* shader, BaseMesh* mesh) :
	world_matrix_(DirectX::XMMatrixIdentity()), texture_(nullptr), shader_(shader), mesh_(mesh)
{
	shader_->SetTexture(texture_);
	render_ = true;
	SetColour(XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f));
}

MeshInstance::~MeshInstance()
{
}

bool MeshInstance::Render(ID3D11DeviceContext* device_context, XMMATRIX& view, XMMATRIX& projection)
{
	if (!render_)
	{
		return false;
	}

	shader_->SetTexture(texture_);
	shader_->SetColour(colour_.x, colour_.y, colour_.z);
	shader_->SetShaderParameters(device_context, world_matrix_, view, projection);

	mesh_->sendData(device_context);

	shader_->render(device_context, mesh_->getIndexCount());

	return true;
}

void MeshInstance::SetTexture(ID3D11ShaderResourceView* texture)
{
	texture_ = texture;
}

void MeshInstance::SetWorldMatrix(XMMATRIX wm)
{
	world_matrix_ = wm;

}

XMMATRIX MeshInstance::GetWorldMatrix()
{
	return world_matrix_;
}

void MeshInstance::SetColour(XMFLOAT4 colour)
{
	colour_ = colour;
}
