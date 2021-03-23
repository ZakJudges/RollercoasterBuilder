#include "BuildingState.h"
#include "TrackMesh.h"
#include "TrackLoader.h"

BuildingState::BuildingState()
{
	track_ = nullptr;
	track_builder_ = nullptr;
	track_loader_ = nullptr;
	delta_time_ = 0.0f;
	move_speed_ = 5.0f;
}

void BuildingState::Init(void* ptr)
{
	track_ = static_cast<Track*>(ptr);
	
	track_builder_ = new TrackBuilder(track_);

	track_loader_ = new TrackLoader();
	track_loader_->LoadTrack("Example1.txt", track_);
	track_builder_->SetTrackLoadToggle();
}

void BuildingState::Update(float delta_time)
{
	delta_time_ = delta_time;

	track_builder_->UpdateTrack();
}

void BuildingState::RenderUI()
{
	if (ImGui::BeginMainMenuBar())
	{
		if (ImGui::BeginMenu("File"))
		{
			in_focus_ = false;

			ImGui::Spacing();

			if (ImGui::BeginMenu("New"))
			{
				if(ImGui::Button("Track"))
				{
					track_builder_->EraseTrack();
				}
				ImGui::EndMenu();
			}

			ImGui::Spacing();
			ImGui::Separator();
			ImGui::Spacing();

			if (ImGui::BeginMenu("Save As"))
			{
				ImGui::InputText("File Name", save_buffer_, sizeof(save_buffer_));
				ImGui::Indent(173.0f);
				if (ImGui::Button("Save"))
				{
					strcat_s(save_buffer_, ".txt");
					//	Save the track to file.
					if (track_loader_->SaveTrack(save_buffer_, track_))
					{
						//	clear the save track filename buffer.
						save_buffer_[0] = 0;
					}
				}
				ImGui::EndMenu();
			}

			ImGui::Spacing();
			ImGui::Separator();
			ImGui::Spacing();

			if (ImGui::BeginMenu("Load"))
			{	
				ImGui::InputText("File Name", load_buffer_, sizeof(load_buffer_));
				ImGui::Indent(173.0f);
				if (ImGui::Button("Load"))
				{
					strcat_s(load_buffer_, ".txt");
					//	Load the track from the file.
					if (track_loader_->LoadTrack(load_buffer_, track_))
					{
						//	clear the load track name buffer.
						load_buffer_[0] = 0;
						track_builder_->SetTrackLoadToggle();
					}
				}
				ImGui::EndMenu();
			}

			ImGui::Spacing();

			ImGui::EndMenu();
		}
		else
		{
			in_focus_ = true;
		}

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

		if (ImGui::BeginMenu("User-Guide"))
		{
			ImGui::Text("Camera Controls:");
			ImGui::BulletText("Movement: W,A,S,D");
			ImGui::BulletText("Look: Press SPACEBAR to toggle mouse look or use arrow keys");
			ImGui::Separator();
			ImGui::Text("Track Building:");
			ImGui::BulletText("To begin editing start a new track in the file tab");
			ImGui::BulletText("Select a track piece type to place.");
			ImGui::BulletText("Once the track piece has been placed you can edit it with SHIFT + W,A,S,D,Q,E");
			ImGui::BulletText("You can make further changes in the 'New Track Piece' pop-up menu");
			ImGui::BulletText("Pressing 'Finish Track' will connect the first and last pieces");
			ImGui::BulletText("Select 'Build Support Structures' to add the track supports");
			ImGui::BulletText("Once finished you may ride your newly created roller coaster");
			ImGui::Separator();
			ImGui::Text("Other Stuff:");
			ImGui::BulletText("You can save the current track or load a different one from the file tab");
			ImGui::BulletText("There are three tracks in the example browser available for viewing");
			ImGui::BulletText("Quit: ESC or top-right button");
			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Example Browser"))
		{
			if (ImGui::Button("Example 1"))
			{
				track_loader_->LoadTrack("Example1.txt", track_);
				track_builder_->SetTrackLoadToggle();
			}
			if (ImGui::Button("Example 2"))
			{
				track_loader_->LoadTrack("Example2.txt", track_);
				track_builder_->SetTrackLoadToggle();
			}
			if (ImGui::Button("Example 3"))
			{
				track_loader_->LoadTrack("Example3.txt", track_);
				track_builder_->SetTrackLoadToggle();
			}
			ImGui::EndMenu();
		}
	

		ImGui::Indent(screen_width_ - 43.0f);
		if (ImGui::Button("Exit"))
		{
			application_running_ = false;
		}
		ImGui::EndMainMenuBar();
	}

	ImGui::Checkbox("Ride Coaster", &exit_);
	ImGui::Separator();
	if (track_->GetTrackPieceCount() >= track_->GetMaxTrackPieceCount())
	{
		ImGui::Text("Track has reached max length!");
		ImGui::Text("Remove track pieces to continue.");
	}
	else
	{
		ImGui::Text("Track Piece Type");
		ImGui::Checkbox("Add Straight", track_builder_->SetTrackPieceType(TrackPiece::Tag::STRAIGHT));
		ImGui::Checkbox("Add Right Turn", track_builder_->SetTrackPieceType(TrackPiece::Tag::RIGHT_TURN));
		ImGui::Checkbox("Add Left Turn", track_builder_->SetTrackPieceType(TrackPiece::Tag::LEFT_TURN));
		ImGui::Checkbox("Add Climb Up", track_builder_->SetTrackPieceType(TrackPiece::Tag::CLIMB_UP));
		ImGui::Checkbox("Add Climb Down", track_builder_->SetTrackPieceType(TrackPiece::Tag::CLIMB_DOWN));
		ImGui::Separator();
		ImGui::Checkbox("Finish Track", track_builder_->SetTrackPieceType(TrackPiece::Tag::COMPLETE_TRACK));
	}

	ImGui::Checkbox("Remove Last Piece", track_builder_->SetUndo());
	ImGui::Checkbox("Build Support Structures", track_builder_->SetBuildSupports());
	ImGui::Separator();

	if (track_builder_->GetPreviewActive())
	{
		ImGui::Begin("New Track Piece");

		if (ImGui::Button("Change Edit Mode"))
		{
			ImGui::OpenPopup("EditMode");
		}
		if (ImGui::BeginPopup("EditMode"))
		{
			if (ImGui::MenuItem("Hard Curve"))
			{
				track_builder_->SetEditModeType(EditMode::EditModeTag::HARD_CURVE);
			}
			if (ImGui::MenuItem("Soft Curve"))
			{
				track_builder_->SetEditModeType(EditMode::EditModeTag::SOFT_CURVE);
			}
			if (ImGui::MenuItem("Fixed Ends"))
			{
				track_builder_->SetEditModeType(EditMode::EditModeTag::FIXED_ENDS);
			}
			ImGui::EndPopup();
		}

		ImGui::SliderInt("Roll Target:", track_builder_->SetRollTarget(), -720, 720);

		ImGui::Checkbox("Finish Track Piece", track_builder_->SetPreviewFinished());
		ImGui::Separator();

		ImGui::End();
	}
}

void BuildingState::OnEnter()
{
}

//	Returns which state the application should go to when finished in this state.
ApplicationState::APPLICATIONSTATE BuildingState::OnExit()
{
	exit_ = false;
	
	//	The track preview is still active, add it to the track so that the camera movement matches the spline during simulation.
	if (track_builder_->GetPreviewActive())
	{
		track_builder_->FinishPreview();
	}

	return APPLICATIONSTATE::SIMULATING_STATE;
}

BuildingState::~BuildingState()
{
	if (track_builder_)
	{
		delete track_builder_;
		track_builder_ = 0;
	}

	if (track_loader_)
	{
		delete track_loader_;
		track_loader_ = 0;
	}
}


void BuildingState::OnWPress()
{
	//	Move all active control points forward.
	for (int i = 0; i < 4; i++)
	{
		if (track_builder_->GetActiveControlPoint(i))
		{
			SL::Vector new_pos = camera_look_at_;
			new_pos.SetY(0.0f);
			new_pos.Scale(move_speed_ * delta_time_);
			new_pos = track_builder_->GetControlPoint(i).Add(new_pos);

			track_builder_->SetControlPoint(i, new_pos);
		}
	}
}

void BuildingState::OnSPress()
{
	//	Move all active control points backwards.
	for (int i = 0; i < 4; i++)
	{
		if (track_builder_->GetActiveControlPoint(i))
		{
			SL::Vector new_pos = camera_look_at_;
			new_pos.SetY(0.0f);
			new_pos.Scale(move_speed_ * delta_time_);
			new_pos = track_builder_->GetControlPoint(i).Subtract(new_pos);

			track_builder_->SetControlPoint(i, new_pos);
		}
	}
}

void BuildingState::OnDPress()
{
	//	Move all active control points to the right.
	for (int i = 0; i < 4; i++)
	{
		if (track_builder_->GetActiveControlPoint(i))
		{
			SL::Vector up(0.0f, 1.0f, 0.0f);
			SL::Vector new_pos = camera_look_at_.Cross(up);
			new_pos.SetY(0.0f);
			new_pos.Scale(move_speed_ * delta_time_);
			new_pos = track_builder_->GetControlPoint(i).Subtract(new_pos);

			track_builder_->SetControlPoint(i, new_pos);
		}
	}
}

void BuildingState::OnAPress()
{
	//	Move all active control points to the left.
	for (int i = 0; i < 4; i++)
	{
		if (track_builder_->GetActiveControlPoint(i))
		{
			SL::Vector up(0.0f, 1.0f, 0.0f);
			SL::Vector new_pos = camera_look_at_.Cross(up);
			new_pos.SetY(0.0f);
			new_pos.Scale(move_speed_ * delta_time_);
			new_pos = track_builder_->GetControlPoint(i).Add(new_pos);

			track_builder_->SetControlPoint(i, new_pos);
		}
	}
}

void BuildingState::OnEPress()
{
	for (int i = 0; i < 4; i++)
	{
		if (track_builder_->GetActiveControlPoint(i))
		{
			// Increase z value of all active control points.
			track_builder_->SetControlPoint(i, 'y', track_builder_->GetControlPoint(i, 'y') + (move_speed_ * delta_time_));
		}
	}
}

void BuildingState::OnQPress()
{
	for (int i = 0; i < 4; i++)
	{
		if (track_builder_->GetActiveControlPoint(i))
		{
			// Increase z value of all active control points.
			track_builder_->SetControlPoint(i, 'y', track_builder_->GetControlPoint(i, 'y') - (move_speed_ * delta_time_));
		}
	}
}

void BuildingState::SetCameraLookAt(float x, float y, float z)
{
	camera_look_at_.Set(x, y, z);
}

float* BuildingState::GetTrackPosition()
{
	return track_builder_->GetTranslation();
}
