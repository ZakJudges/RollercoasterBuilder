#include "TrackBuilder.h"

#include "TrackPreview.h"
#include "RightTurn.h"
#include "Straight.h"
#include "LeftTurn.h"
#include "ClimbUp.h"
#include "ClimbDown.h"
#include "CompleteTrack.h"
#include "TrackMesh.h"

TrackBuilder::TrackBuilder(Track* track) : track_(track), track_piece_(nullptr)
{
	//	Size based on total number of different track piece types.
	track_piece_types_ = new TrackPieceType[static_cast<int>(TrackPiece::Tag::NUMBER_OF_TYPES)];
	InitTrackPieceTypes();
	InitEditModeTypes();

	edit_mode_ = &soft_curve_;
	active_control_point_[0] = edit_mode_->GetP0State();
	active_control_point_[1] = edit_mode_->GetP1State();
	active_control_point_[2] = edit_mode_->GetP2State();
	active_control_point_[3] = edit_mode_->GetP3State();
	update_preview_mesh_ = false;
	undo_ = false;
	build_supports_ = false;
	track_load_toggle_ = false;
	track_preview_ = new TrackPreview(track->GetTrackMesh());
	track_piece_ = track_preview_->GetPreviewPiece();
	preview_finished_ = false;
	translation_[0] = 0;
	translation_[1] = 0;
	translation_[2] = 0;
}

float* TrackBuilder::GetTranslation()
{
	return translation_;
}

//	Externally set is_active on each track piece.
bool* TrackBuilder::SetTrackPieceType(TrackPiece::Tag tag)
{
	return &track_piece_types_[static_cast<int>(tag)].is_active;
}

//	Externally set is_active on each edit mode.
bool* TrackBuilder::SetEditModeTypeImGui(EditMode::EditModeTag tag)
{
	return &edit_mode_types_[static_cast<int>(tag)].is_active;
}

void TrackBuilder::SetEditModeType(EditMode::EditModeTag tag)
{
	edit_mode_types_[static_cast<int>(tag)].is_active = true;
}

bool* TrackBuilder::SetUndo()
{
	return &undo_;
}

bool* TrackBuilder::SetBuildSupports()
{
	return &build_supports_;
}

//	Update the track based on user input.
void TrackBuilder::UpdateTrack()
{
	//	Determine if the edit mode has been changed by the user.
	for (int i = 0; i < static_cast<int>(EditMode::EditModeTag::MODE_COUNT); i++)
	{
		if (edit_mode_types_[i].is_active)
		{
			SetEditMode(edit_mode_types_[i].tag);
			edit_mode_types_[i].is_active = false;
			active_control_point_[0] = edit_mode_->GetP0State();
			active_control_point_[1] = edit_mode_->GetP1State();
			active_control_point_[2] = edit_mode_->GetP2State();
			active_control_point_[3] = edit_mode_->GetP3State();
		}
	}

	if (build_supports_)
	{
		FinishPreview();
		track_->GenerateSupportStructures();
		build_supports_ = false;
	}

	if (undo_)
	{
		Undo();
		undo_ = false;
	}
	else
	{
		Build();
	}

	//	If the user selects 'finish track piece' then add preview to the track.
	if (preview_finished_)
	{
		FinishPreview();
	}

	if (update_preview_mesh_)
	{
		track_preview_->GenerateMesh();
		update_preview_mesh_ = false;
	}
	
	track_->GetTrackMesh()->SetTranslation(translation_[0], translation_[1], translation_[2]);
}

void TrackBuilder::Build()
{
	//	Determine if the user has added a new track piece.
	for (int i = 0; i < static_cast<int>(TrackPiece::Tag::NUMBER_OF_TYPES); i++)
	{
		if (track_piece_types_[i].is_active)
		{
			track_piece_types_[i].is_active = false;

			//	There will be no track preview to change when there are no track pieces, or when the track has been loaded.
			if (track_->GetTrackPieceCount() != 0 && !track_load_toggle_)
			{
				track_->UpdateBack(track_piece_);
				track_->GenerateMesh();
			}

			//	Reset track load toggle.
			if (track_load_toggle_)
			{
				track_load_toggle_ = false;
			}

			track_->AddTrackPiece(track_piece_types_[i].tag);
			SetTrackPieceData();

			//	Pass the new track piece to the preview track for simulation.
			track_preview_->InitTrackPiece(track_->GetBack());
			track_preview_->SetPreviewActive(true);

			//	Pass starting conditions for simulation to the track preview. 
			track_preview_->InitialiseSimulation(track_->GetRollStore(), track_->GetForwardStore(),
			track_->GetRightStore(), track_->GetUpStore(), track_->GetTargetRollStore());

			update_preview_mesh_ = true;
			
		}
	}

	//	Update the track preview mesh, based on user input.
	if (track_piece_)
	{
		if ((track_piece_->GetRollTarget() != track_piece_data_.roll_target) && track_preview_->GetPreviewActive())
		{
			track_piece_->SetRollTarget(track_piece_data_.roll_target);
			update_preview_mesh_ = true;
		}

		if (update_preview_mesh_)
		{
			SL::Vector p0(track_piece_data_.p0_x, track_piece_data_.p0_y, track_piece_data_.p0_z);
			SL::Vector p1(track_piece_data_.p1_x, track_piece_data_.p1_y, track_piece_data_.p1_z);
			SL::Vector p2(track_piece_data_.p2_x, track_piece_data_.p2_y, track_piece_data_.p2_z);
			SL::Vector p3(track_piece_data_.p3_x, track_piece_data_.p3_y, track_piece_data_.p3_z);
			track_piece_->SetControlPoints(p0, p1, p2, p3);

			track_piece_->GetSpline()->CalculateCoefficients(track_piece_->GetTension());

			track_preview_->CalculateLength();
		}
	}
}

void TrackBuilder::Undo()
{
	if (track_->GetTrackPieceCount() == 0)
	{
		return;
	}
	else if (track_->GetTrackPieceCount() == 1)
	{
		EraseTrack();
		return;
	}

	//	Remove the end-piece of the track.
	track_->RemoveBack();

	//	Pass the new end-piece data to the track preview and disable it.
	track_preview_->InitTrackPiece(track_->GetBack());
	SetTrackPieceData();
	track_preview_->SetPreviewActive(false);
	preview_finished_ = true;
	undo_ = false;
}

void TrackBuilder::EraseTrack()
{
	track_->EraseTrack();
	track_preview_->EraseTrack();
	track_->GetTrackMesh()->Clear();
	track_->Reset();
}

void TrackBuilder::SetTrackLoadToggle()
{
	track_preview_->SetPreviewActive(false);

	track_load_toggle_ = true;
}

void TrackBuilder::SetTrackPieceData()
{
	if(track_->GetTrackPieceCount() == 0)
	{
		return;
	}
	TrackPiece* track_piece = track_->GetBack();

	if (track_piece)
	{
		SL::Vector p0 = track_piece->GetControlPoint(0);
		SL::Vector p1 = track_piece->GetControlPoint(1);
		SL::Vector p2 = track_piece->GetControlPoint(2);
		SL::Vector p3 = track_piece->GetControlPoint(3);

		track_piece_data_.p0_x = p0.X();
		track_piece_data_.p0_y = p0.Y();
		track_piece_data_.p0_z = p0.Z();

		track_piece_data_.p1_x = p1.X();
		track_piece_data_.p1_y = p1.Y();
		track_piece_data_.p1_z = p1.Z();

		track_piece_data_.p2_x = p2.X();
		track_piece_data_.p2_y = p2.Y();
		track_piece_data_.p2_z = p2.Z();

		track_piece_data_.p3_x = p3.X();
		track_piece_data_.p3_y = p3.Y();
		track_piece_data_.p3_z = p3.Z();

		track_piece_data_.roll_target = track_piece->GetRollTarget();
	}
}

void TrackBuilder::InitTrackPieceTypes()
{
	for (int i = 0; i < static_cast<int>(TrackPiece::Tag::NUMBER_OF_TYPES); i++)
	{
		track_piece_types_[i].tag = static_cast<TrackPiece::Tag>(i);
		track_piece_types_[i].is_active = false;
	}
}

void TrackBuilder::InitEditModeTypes()
{
	for (int i = 0; i < 4; i++)
	{
		edit_mode_types_[i].tag = static_cast<EditMode::EditModeTag>(i);
		edit_mode_types_[i].is_active = false;
	}
}

TrackBuilder::~TrackBuilder()
{
	if (track_piece_types_)
	{
		delete[] track_piece_types_;
		track_piece_types_ = 0;
	}

	if (track_preview_)
	{
		delete track_preview_;
		track_preview_ = 0;
	}
}

int* TrackBuilder::SetRollTarget()
{
	return &track_piece_data_.roll_target;
}

bool TrackBuilder::GetActiveControlPoint(int control_point)
{
	return active_control_point_[control_point];
}

bool* TrackBuilder::SetActiveControlPoint(int control_point)
{
	return &active_control_point_[control_point];
}

bool* TrackBuilder::SetPreviewFinished()
{
	return &preview_finished_;
}

void TrackBuilder::FinishPreview()
{
	track_->UpdateBack(track_piece_);
	track_->GenerateMesh();
	track_preview_->SetPreviewActive(false);
	preview_finished_ = false;
}

bool TrackBuilder::GetPreviewActive()
{
	return track_preview_->GetPreviewActive();
}

void TrackBuilder::SetEditMode(EditMode::EditModeTag tag)
{
	switch (tag)
	{
	case EditMode::EditModeTag::HARD_CURVE:
		edit_mode_ = &hard_curve_;
		break;
	case EditMode::EditModeTag::SOFT_CURVE:
		edit_mode_ = &soft_curve_;
		break;
	case EditMode::EditModeTag::FIXED_ENDS:
		edit_mode_ = &fixed_ends_;
		break;
	}
}

void TrackBuilder::UpdatePreviewMesh()
{
	track_preview_->GenerateMesh();
}

void TrackBuilder::SetControlPoint(int control_point, char element, float value)
{
	if (!track_preview_->GetPreviewActive())
	{
		return;
	}
	update_preview_mesh_ = true;

	if (control_point == 0)
	{
		SetP0(element, value);
	}
	else if (control_point == 1)
	{
		SetP1(element, value);
	}
	else if (control_point == 2)
	{
		SetP2(element, value);
	} 
	else
	{
		SetP3(element, value);
	}
}

void TrackBuilder::SetControlPoint(int control_point, SL::Vector values)
{
	if (!track_preview_->GetPreviewActive())
	{
		return;
	}

	update_preview_mesh_ = true;

	switch (control_point)
	{
	case 0: SetP0(values);
		break;
	case 1: SetP1(values);
		break;
	case 2: SetP2(values);
		break;
	case 3: SetP3(values);
		break;
	}
}

SL::Vector TrackBuilder::GetControlPoint(int control_point)
{
	SL::Vector result;

	switch (control_point)
	{
	case 0: result = GetP0();
		break;
	case 1: result = GetP1();
		break;
	case 2: result = GetP2();
		break;
	case 3: result = GetP3();
		break;
	}

	return result;
}

float TrackBuilder::GetControlPoint(int control_point, char element)
{
	if (control_point == 0)
	{
		return GetP0(element);
	}
	else if (control_point == 1)
	{
		return GetP1(element);
	}
	else if (control_point == 2)
	{
		return GetP2(element);
	}
	else
	{
		return GetP3(element);
	}
}

void TrackBuilder::SetP0(char element, float value)
{
	if (element == 'x')
	{
		track_piece_data_.p0_x = value;
	}
	else if (element == 'y')
	{
		track_piece_data_.p0_y = value;
	}
	else
	{
		track_piece_data_.p0_z = value;
	}
}

void TrackBuilder::SetP1(char element, float value)
{
	if (element == 'x')
	{
		track_piece_data_.p1_x = value;
	}
	else if (element == 'y')
	{
		track_piece_data_.p1_y = value;
	}
	else
	{
		track_piece_data_.p1_z = value;
	}
}

void TrackBuilder::SetP2(char element, float value)
{
	if (element == 'x')
	{
		track_piece_data_.p2_x = value;
	}
	else if (element == 'y')
	{
		track_piece_data_.p2_y = value;
	}
	else
	{
		track_piece_data_.p2_z = value;
	}
}

void TrackBuilder::SetP3(char element, float value)
{
	if (element == 'x')
	{
		track_piece_data_.p3_x = value;
	}
	else if (element == 'y')
	{
		track_piece_data_.p3_y = value;
	}
	else
	{
		track_piece_data_.p3_z = value;
	}
}

void TrackBuilder::SetP0(SL::Vector values)
{
	track_piece_data_.p0_x = values.X();
	track_piece_data_.p0_y = values.Y();
	track_piece_data_.p0_z = values.Z();
}

void TrackBuilder::SetP1(SL::Vector values)
{
	track_piece_data_.p1_x = values.X();
	track_piece_data_.p1_y = values.Y();
	track_piece_data_.p1_z = values.Z();
}

void TrackBuilder::SetP2(SL::Vector values)
{
	track_piece_data_.p2_x = values.X();
	track_piece_data_.p2_y = values.Y();
	track_piece_data_.p2_z = values.Z();
}

void TrackBuilder::SetP3(SL::Vector values)
{
	track_piece_data_.p3_x = values.X();
	track_piece_data_.p3_y = values.Y();
	track_piece_data_.p3_z = values.Z();
}

float TrackBuilder::GetP0(char element)
{
	if (element == 'x')
	{
		return track_piece_data_.p0_x;
	}
	else if (element == 'y')
	{
		return track_piece_data_.p0_y;
	}
	else
	{
		return track_piece_data_.p0_z;
	}
}

float TrackBuilder::GetP1(char element)
{
	if (element == 'x')
	{
		return track_piece_data_.p1_x;
	}
	else if (element == 'y')
	{
		return track_piece_data_.p1_y;
	}
	else
	{
		return track_piece_data_.p1_z;
	}
}

float TrackBuilder::GetP2(char element)
{
	if (element == 'x')
	{
		return track_piece_data_.p2_x;
	}
	else if (element == 'y')
	{
		return track_piece_data_.p2_y;
	}
	else
	{
		return track_piece_data_.p2_z;
	}
}

float TrackBuilder::GetP3(char element)
{
	if (element == 'x')
	{
		return track_piece_data_.p3_x;
	}
	else if (element == 'y')
	{
		return track_piece_data_.p3_y;
	}
	else
	{
		return track_piece_data_.p3_z;
	}
}

SL::Vector TrackBuilder::GetP0()
{
	return SL::Vector(track_piece_data_.p0_x, track_piece_data_.p0_y, track_piece_data_.p0_z);
}

SL::Vector TrackBuilder::GetP1()
{
	return SL::Vector(track_piece_data_.p1_x, track_piece_data_.p1_y, track_piece_data_.p1_z);
}

SL::Vector TrackBuilder::GetP2()
{
	return SL::Vector(track_piece_data_.p2_x, track_piece_data_.p2_y, track_piece_data_.p2_z);
}

SL::Vector TrackBuilder::GetP3()
{
	return SL::Vector(track_piece_data_.p3_x, track_piece_data_.p3_y, track_piece_data_.p3_z);
}
