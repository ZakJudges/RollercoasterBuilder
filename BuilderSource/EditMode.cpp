#include "EditMode.h"

EditMode::EditMode()
{

}
EditMode::~EditMode()
{
}

bool EditModeSoftCurve::GetP0State()
{
	return true;
}
bool EditModeSoftCurve::GetP1State()
{
	return false;
}
bool EditModeSoftCurve::GetP2State()
{
	return true;
}
bool EditModeSoftCurve::GetP3State()
{
	return true;
}


bool EditModeHardCurve::GetP0State()
{
	return true;
}
bool EditModeHardCurve::GetP1State()
{
	return false;
}
bool EditModeHardCurve::GetP2State()
{
	return true;
}
bool EditModeHardCurve::GetP3State()
{
	return false;
}


bool EditModeFixedEnds::GetP0State()
{
	return false;
}
bool EditModeFixedEnds::GetP1State()
{
	return false;
}
bool EditModeFixedEnds::GetP2State()
{
	return false;
}
bool EditModeFixedEnds::GetP3State()
{
	return true;
}


