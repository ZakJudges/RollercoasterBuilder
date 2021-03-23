#pragma once

#include "TrackPiece.h"

class RightTurn : public TrackPiece
{
public:
	RightTurn();
	Tag GetTag();
	~RightTurn();
};