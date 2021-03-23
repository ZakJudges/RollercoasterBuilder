#include "CompleteTrack.h"

CompleteTrack::CompleteTrack(SL::CRSpline* segment)
{
	spline_segment_ = segment;
	tension_ = 2.0f;
}

TrackPiece::Tag CompleteTrack::GetTag()
{
	return Tag::COMPLETE_TRACK;
}

bool CompleteTrack::ShouldSmooth()
{
	//	We don't want to transform any points in the spline segment as the control points are calculated to match the tangents and join the track, 
	//		...before being added to the spline controller.
	return false;
}

CompleteTrack::~CompleteTrack()
{

}
