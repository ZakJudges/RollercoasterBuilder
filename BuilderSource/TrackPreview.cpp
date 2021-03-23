#include "TrackPreview.h"
#include "../Spline-Library/CRSplineController.h"
#include "TrackPiece.h"
#include "TrackMesh.h"


TrackPreview::TrackPreview(TrackMesh* track_mesh) : track_mesh_(track_mesh)
{
    preview_active_ = false;
    t_ = 0.0f;
    roll_ = 0.0f;
    initial_roll_ = 0.0f;
    track_piece_ = new TrackPiece();
    SL::CRSpline* spline_segment = new SL::CRSpline();
    SL::Vector p0, p1, p2, p3;

    p0.Set(0.0f, 0.0f, 0.0f);
    p1.Set(0.0f, 0.0f, 0.0f);
    p2.Set(0.0f, 0.0f, 0.0f);
    p3.Set(0.0f, 0.0f, 0.0f);

    spline_segment->SetControlPoints(p0, p1, p2, p3);
    track_piece_->SetSplineSegment(spline_segment);

    spline_controller_ = new SL::CRSplineController(100);

    spline_controller_->AddSegment(spline_segment, 1.0f);
}

void TrackPreview::EraseTrack()
{
    SL::Vector p0, p1, p2, p3;
    preview_active_ = false;
    t_ = 0.0f;
    roll_ = 0.0f;
    initial_roll_ = 0.0f;

    p0.Set(0.0f, 0.0f, 0.0f);
    p1.Set(0.0f, 0.0f, 0.0f);
    p2.Set(0.0f, 0.0f, 0.0f);
    p3.Set(0.0f, 0.0f, 0.0f);

    track_piece_->SetControlPoints(p0, p1, p2, p3);
}


void TrackPreview::InitialiseSimulation(float initial_roll, SL::Vector forward, SL::Vector right, SL::Vector up, float previous_roll_target)
{
    InitialiseRoll(initial_roll);
    InitialiseForward(forward);
    InitialiseRight(right);
    InitialiseUp(up);
    SetPreviousRollTarget(previous_roll_target);
}

//  Copy the track piece that has just been created.
void TrackPreview::InitTrackPiece(TrackPiece* track_piece)
{
    //  Make the preview track piece identical to the newly placed track piece.
    SL::Vector p0 = track_piece->GetControlPoint(0);
    SL::Vector p1 = track_piece->GetControlPoint(1);
    SL::Vector p2 = track_piece->GetControlPoint(2);
    SL::Vector p3 = track_piece->GetControlPoint(3);

    track_piece_->SetControlPoints(p0, p1, p2, p3);
    track_piece_->SetTension(track_piece->GetTension());
    track_piece_->SetRollTarget(track_piece->GetRollTarget());
    track_piece_->CalculateSpline();

    spline_controller_->CalculateSplineLength();
    CalculateLength();

    track_mesh_->SetPreviewActive(true);
}

void TrackPreview::SetPreviewActive(bool active)
{
    preview_active_ = active;
    track_mesh_->SetPreviewActive(active);
}

void TrackPreview::GenerateMesh()
{
    //  Simulate this track piece, given the initial conditions from the main track.
    //  Use the simulation to generate the points for the mesh.
    //	Store data needed for the mesh to generate itself.
    for (int i = 0; i < 25; i++)
    {
        float t = (float)i / 24.0f;

        UpdateSimulation(t);

        XMFLOAT3 pos = GetPointAtDistance(t);
        XMVECTOR centre = XMVectorSet(pos.x, pos.y, pos.z, 0.0f);

        XMVECTOR x = XMVectorSet(GetRight().x, GetRight().y, GetRight().z, 0.0f);
        XMVECTOR y = XMVectorSet(GetUp().x, GetUp().y, GetUp().z, 0.0f);
        XMVECTOR z = XMVectorSet(GetForward().x, GetForward().y, GetForward().z, 0.0f);

        track_mesh_->StorePreviewPoints(centre, x, y, z);

        if (i % track_mesh_->GetCrossTieFrequency() == 0)
        {
            track_mesh_->AddPreviewCrossTie(centre, x, y, z);
        }
    }

    track_mesh_->UpdatePreviewMesh();

    Reset();
}

void TrackPreview::InitialiseForward(SL::Vector forward)
{
    forward_ = forward;
    initial_forward_ = forward_;
}

void TrackPreview::InitialiseRight(SL::Vector right)
{
    right_ = right;
    initial_right_ = right_;
}

void TrackPreview::InitialiseUp(SL::Vector up)
{
    up_ = up;
    initial_up_ = up_;
}

void TrackPreview::SetPreviousRollTarget(float roll)
{
    track_piece_->SetInitialRoll(roll);
}

void TrackPreview::UpdateSimulation(float t)
{
    t_ = spline_controller_->GetTimeAtDistance(t);

    forward_ = spline_controller_->GetTangent(t_);
    right_ = up_.Cross(forward_).Normalised();
    up_ = forward_.Cross(right_).Normalised();

    //	The start and target roll for this timestep.
    float start_roll = track_piece_->GetInitRoll();
    
    float target_roll = Lerpf(start_roll * 0.0174533f, track_piece_->GetRollTarget() * 0.0174533f, t_);

    //	Get the difference between the desired roll and the current roll.
    float angle_needed = target_roll - roll_;

    if (angle_needed != 0.0f)
    {
        SL::Matrix3x3 roll_matrix;
        roll_matrix.RotationAxisAngle(forward_, angle_needed);

        up_ = roll_matrix.TransformVector(up_);

        right_ = up_.Cross(forward_);

        roll_ = target_roll;
    }
}

void TrackPreview::Reset()
{
    //  Reset the simulation to the state it was in when it was created.
    roll_ = initial_roll_;
    forward_ = initial_forward_;
    right_ = initial_right_;
    up_ = initial_up_;
    t_ = 0.0f;
}

void TrackPreview::Clear()
{
    track_mesh_->ClearPreview();
}

void TrackPreview::CalculateLength()
{
    spline_controller_->CalculateSplineLength();
    track_piece_->SetLength(spline_controller_->GetArcLength());
}

TrackPiece* TrackPreview::GetPreviewPiece()
{
    return track_piece_;
}

float TrackPreview::Lerpf(float f0, float f1, float t)
{
    return (1.0f - t) * f0 + t * f1;
}

DirectX::XMFLOAT3 TrackPreview::GetPointAtDistance(float d)
{
    SL::Vector point;

    if (d == 1)
    {
        //	Due to the length of the track being estimated, there will be a margin of error between
        //		t = 1 and d = 1, so force the application to get the point at the end of the track.
        point = spline_controller_->GetPoint(1.0f);
    }
    else
    {
        point = spline_controller_->GetPointAtDistance(d);
    }

    return XMFLOAT3(point.X(), point.Y(), point.Z());
}

DirectX::XMFLOAT3 TrackPreview::GetForward()
{
    return XMFLOAT3(forward_.X(), forward_.Y(), forward_.Z());
}

DirectX::XMFLOAT3 TrackPreview::GetUp()
{
    return XMFLOAT3(up_.X(), up_.Y(), up_.Z());
}

DirectX::XMFLOAT3 TrackPreview::GetRight()
{
    return XMFLOAT3(right_.X(), right_.Y(), right_.Z());
}

void TrackPreview::InitialiseRoll(float roll)
{
    roll_ = roll;
    initial_roll_ = roll_;
}

TrackPreview::~TrackPreview()
{
    if (track_piece_)
    {
        delete track_piece_;
        track_piece_ = nullptr;
    }
    
    if (spline_controller_)
    {
        delete spline_controller_;
        spline_controller_ = 0;
    }
}
