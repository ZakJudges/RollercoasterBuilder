#pragma once

#include "TrackPiece.h"

class LeftTurn : public TrackPiece
{
public:
	LeftTurn();
	Tag GetTag();
	~LeftTurn();
};