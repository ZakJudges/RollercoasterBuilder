#include "SimulatingState.h"

#include "TrackMesh.h"

SimulatingState::SimulatingState()
{
	t_ = 0.0f;
	track_ = nullptr;
	track_top_speed_ = 0.5f;
	track_min_speed_ = 0.2f;
	track_speed_ = track_min_speed_;
}

void SimulatingState::Init(void* ptr)
{
	track_ = static_cast<Track*>(ptr);
}

void SimulatingState::Update(float delta_time)
{
	SL::Vector tangent = track_->GetTangent();
	SL::Vector down(0.0f, -1.0f, 0.0f);

	//	Calculate the change in speed based on the orientation of the track.
	//	If the tangent is pointing downwards then the speed will be higher.
	float speed = tangent.Dot(down) * delta_time;
	if (speed > 0.00005f)
	{
		speed = 0.00005f;
	}
	else if (speed < -0.00003f)
	{
		speed = -0.00003f;
	}

	//	Change the speed and keep within range.
	track_speed_ += speed;
	if (track_speed_ > (track_top_speed_ / track_->GetTrackPieceCount()))
	{
		track_speed_ = track_top_speed_ / track_->GetTrackPieceCount();
	}
	else if (track_speed_ < (track_min_speed_ / track_->GetTrackPieceCount()))
	{
		track_speed_ = track_min_speed_ / track_->GetTrackPieceCount();
	}

	t_ += (track_speed_ * delta_time);
	
	if (t_ > 1.0f || t_ < 0.0f)
	{
		t_ = 0.0f;
		track_->Reset();
	}

	track_->UpdateSimulation(t_);

	AddLines();
}

//	Calculate the lines for the reference frame.
void SimulatingState::AddLines()
{
	if (line_controller_)
	{
		XMMATRIX track_matrix = track_->GetTrackMesh()->GetWorldMatrix();
		XMFLOAT3 offset;
		offset.x = XMVectorGetX(track_matrix.r[3]);
		offset.y = XMVectorGetY(track_matrix.r[3]);
		offset.z = XMVectorGetZ(track_matrix.r[3]);

		line_controller_->Clear();

		//	Build the transform for the object travelling along the spline.
		XMFLOAT3 start = track_->GetPoint();
		start = XMFLOAT3(start.x + offset.x, start.y + offset.y, start.z + offset.z);

		XMFLOAT3 forward = track_->GetForward();
		XMFLOAT3 end(start.x + forward.x, start.y + forward.y, start.z + forward.z);
		line_controller_->AddLine(start, end, XMFLOAT3(1.0f, 0.0f, 0.0f));

		XMFLOAT3 right = track_->GetRight();
		end = XMFLOAT3(start.x + right.x, start.y + right.y, start.z + right.z);
		line_controller_->AddLine(start, end, XMFLOAT3(0.0f, 0.0f, 1.0f));

		XMFLOAT3 up = track_->GetUp();
		end = XMFLOAT3(start.x + up.x, start.y + up.y, start.z + up.z);
		line_controller_->AddLine(start, end, XMFLOAT3(0.0f, 1.0f, 0.0f));
	}
}

void SimulatingState::RenderUI()
{
	if (ImGui::BeginMainMenuBar())
	{
		ImGui::Indent(43.0f);
		if (ImGui::BeginMenu("Settings"))
		{
			if (ImGui::Button("Toggle Frame Reference"))
			{
				line_controller_->SetRenderFlag(!line_controller_->GetRenderFlag());
			}

			if (ImGui::Button("Toggle Wireframe"))
			{
				SetWireframeState(!wireframe_state_);
			}

			if (ImGui::Button("Toggle FPS"))
			{
				ToggleFPS();
			}

			ImGui::EndMenu();
		}

		ImGui::Indent(screen_width_ - 86.0f);
		if (ImGui::Button("Exit"))
		{
			application_running_ = false;
		}
		
		ImGui::EndMainMenuBar();
	}

	ImGui::Text("Riding Coaster");
	ImGui::Spacing();
	ImGui::Separator();
	ImGui::Spacing();
	ImGui::Checkbox("Back to Editing", &exit_);
}


void SimulatingState::OnEnter()
{
}

ApplicationState::APPLICATIONSTATE SimulatingState::OnExit()
{
	exit_ = false;
	track_->Reset();
	t_ = 0.0f;

	return APPLICATIONSTATE::BUILDING_STATE;
}

SimulatingState::~SimulatingState()
{

}

