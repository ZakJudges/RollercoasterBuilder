#pragma once

//	Class lets the application know which control points should be edited, based on which edit mode is active.
class EditMode
{
public:
	enum class EditModeTag
	{
		SOFT_CURVE = 0,
		HARD_CURVE,
		FIXED_ENDS,
		MODE_COUNT
	};
	EditMode();
	virtual bool GetP0State() = 0;
	virtual bool GetP1State() = 0;
	virtual bool GetP2State() = 0;
	virtual bool GetP3State() = 0;
	virtual ~EditMode();
};

class EditModeSoftCurve : public EditMode
{
public:
	virtual bool GetP0State();
	virtual bool GetP1State();
	virtual bool GetP2State();
	virtual bool GetP3State();
};

class EditModeHardCurve : public EditMode
{
public:
	virtual bool GetP0State();
	virtual bool GetP1State();
	virtual bool GetP2State();
	virtual bool GetP3State();
};

class EditModeFixedEnds : public EditMode
{
public:
	virtual bool GetP0State();
	virtual bool GetP1State();
	virtual bool GetP2State();
	virtual bool GetP3State();
};

