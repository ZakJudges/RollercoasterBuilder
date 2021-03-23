#include "TrackPiece.h"

TrackPiece::TrackPiece()
{
	bounding_values_.t0 = 0.0f;
	bounding_values_.t1 = 1.0f;
	length_ = 0.0f;
	roll_target_ = 0.0f;
	roll_initial_ = 0.0f;
	tension_ = 2.0f;
	orientation_stored_ = false;
	spline_segment_ = nullptr;
}

void TrackPiece::SetSplineSegment(SL::CRSpline* spline)
{
	spline_segment_ = spline;
}

void TrackPiece::SetLength(float length)
{
	length_ = length;
}

TrackPiece::Tag TrackPiece::GetTag()
{
	return Tag::NUMBER_OF_TYPES;
}

bool TrackPiece::ShouldSmooth()
{
	return true;
}

float TrackPiece::GetTension()
{
	return tension_;
}

void TrackPiece::SetTension(float tension)
{
	tension_ = tension;
}

void TrackPiece::SetRollTarget(float roll_target)
{
	roll_target_ = roll_target;
}

float TrackPiece::GetRollTarget()
{
	return roll_target_;
}

SL::Vector TrackPiece::GetControlPoint(int element)
{
	return spline_segment_->GetControlPoint(element);
}

void TrackPiece::SetControlPoint(int control_point, SL::Vector point)
{
	spline_segment_->SetControlPoint(point, control_point);
}

void TrackPiece::SetControlPoints(SL::Vector p0, SL::Vector p1, SL::Vector p2, SL::Vector p3)
{
	spline_segment_->SetControlPoints(p0, p1, p2, p3);
}

SL::CRSpline* TrackPiece::GetSpline()
{
	return spline_segment_;
}

void TrackPiece::CalculateSpline()
{
	spline_segment_->CalculateCoefficients(GetTension());
}

void TrackPiece::StoreOrientation(SL::Vector up, SL::Vector right, SL::Vector forward)
{
	initial_up_ = up;
	initial_right_ = right;
	initial_forward_ = forward;
	orientation_stored_ = true;
}

SL::Vector TrackPiece::GetInitUp()
{
	return initial_up_;
}

SL::Vector TrackPiece::GetInitForward()
{
	return initial_forward_;
}

SL::Vector TrackPiece::GetInitRight()
{
	return initial_right_;
}

float TrackPiece::GetInitRoll()
{
	return roll_initial_;
}

void TrackPiece::SetInitialRoll(float roll)
{
	roll_initial_ = roll;
}

TrackPiece::~TrackPiece()
{
}
