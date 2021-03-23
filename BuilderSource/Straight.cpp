#include "Straight.h"

Straight::Straight()
{
	SetTension(2.0f);
	spline_segment_ = new SL::CRSpline();
	SL::Vector p0, p1, p2, p3;

	p0.Set(0.0f, 0.0f, 0.0f);
	p1.Set(0.0f, 0.0f, 0.0f);
	p2.Set(0.0f, 0.0f, 10.0f);
	p3.Set(0.0f, 0.0f, 10.0f);

	spline_segment_->SetControlPoints(p0, p1, p2, p3);
}

TrackPiece::Tag Straight::GetTag()
{
	return Tag::STRAIGHT;
}

Straight::~Straight()
{
	
}
