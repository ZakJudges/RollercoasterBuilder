#include "LineController.h"

LineController::LineController(ID3D11Device* device, ID3D11DeviceContext* device_context, BaseShader* colour_shader, const unsigned int max_vertices ) :
	colour_shader_(colour_shader)
{
	line_mesh_ = new LineMesh(device, device_context, max_vertices);
	should_render_ = false;
}

void LineController::AddLine(XMFLOAT3 start, XMFLOAT3 end, XMFLOAT3 colour)
{
	Line line;
	line.start = start;
	line.end = end;
	line.colour = colour;

	line_mesh_->AddLine(line);
}

void LineController::Clear()
{
	line_mesh_->Clear();
}

void LineController::SetRenderFlag(bool render)
{
	should_render_ = render;
}

bool LineController::GetRenderFlag()
{
	return should_render_;
}

void LineController::Render(ID3D11DeviceContext* device_context, XMMATRIX& world, XMMATRIX& view, XMMATRIX& projection)
{
	if (colour_shader_ && line_mesh_ && should_render_)
	{
		colour_shader_->SetShaderParameters(device_context, world, view, projection);
		line_mesh_->sendData(device_context);
		colour_shader_->render(device_context, line_mesh_->getIndexCount());
	}
}

LineController::~LineController()
{
	if (line_mesh_)
	{
		delete line_mesh_;
		line_mesh_ = 0;
	}
}
