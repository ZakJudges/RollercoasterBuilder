#pragma once

#include "ApplicationState.h"
#include "TrackBuilder.h"
#include "Track.h"

class TrackLoader;

class BuildingState : public ApplicationState
{
public:
	BuildingState();
	void Init(void* ptr);
	void Update(float delta_time);
	void OnEnter();
	APPLICATIONSTATE OnExit();
	~BuildingState();
	void RenderUI();
	virtual void OnWPress();
	virtual void OnSPress();
	virtual void OnDPress();
	virtual void OnAPress();
	virtual void OnEPress();
	virtual void OnQPress();
	virtual void SetCameraLookAt(float x, float y, float z);
	float* GetTrackPosition();

private:
	TrackBuilder* track_builder_;
	Track* track_;
	float delta_time_;
	float move_speed_;
	TrackLoader* track_loader_;
	char save_buffer_[64];
	char load_buffer_[64];
	float p0_[3];
	float p1_[3];
	float p2_[3];
	float p3_[3];
	SL::Vector camera_look_at_;
};