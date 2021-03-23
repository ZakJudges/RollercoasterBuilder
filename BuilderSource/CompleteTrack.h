#pragma once

#include "TrackPiece.h"

//	Used to join the end of the track to the start.
class CompleteTrack : public TrackPiece
{
public:
	CompleteTrack(SL::CRSpline* segment);
	Tag GetTag();
	bool ShouldSmooth();
	~CompleteTrack();
};