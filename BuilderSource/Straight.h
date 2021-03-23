#pragma once

#include "TrackPiece.h"

class Straight : public TrackPiece
{
public:
	Straight();
	Tag GetTag();
	~Straight();
};