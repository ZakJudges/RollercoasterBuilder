#pragma once

#include "ApplicationState.h"
#include "Track.h"

#include "LineController.h"

class SimulatingState : public ApplicationState
{
public:
	SimulatingState();
	void Init(void* ptr);
	void Update(float delta_time);
	void RenderUI();
	void OnEnter();
	APPLICATIONSTATE OnExit();

	~SimulatingState();

private:
	void AddLines();

private:
	float t_;
	Track* track_;
	float track_speed_;
	float track_top_speed_;
	float track_min_speed_;

};