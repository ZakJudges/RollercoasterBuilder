
#ifndef _APP1_H
#define _APP1_H

#include "../DXFramework/DXF.h"
#include "ColourShader.h"
#include "../DXFramework/Geometry.h"
#include "DefaultShader.h"
#include "MeshInstance.h"
#include <vector>
#include "CoasterCamera.h"
#include "Track.h"
#include "BuildingState.h"
#include "SimulatingState.h"
#include "LineController.h"
#include "TrackMesh.h"

class App1 : public BaseApplication
{
public:

	App1();
	~App1();
	void init(HINSTANCE hinstance, HWND hwnd, int screenWidth, int screenHeight, Input* in);
	bool frame();

protected:
	bool render();
	void gui();

private:
	void SwitchApplicationState(ApplicationState::APPLICATIONSTATE);
	void StateInput();

private:
	LineController* line_controller_;
	std::vector<MeshInstance*> objects_;
	Track* track_;
	TrackMesh* track_mesh_;
	Camera default_camera_;
	CoasterCamera coaster_camera_;
	ApplicationState* application_state_;
	BuildingState building_state_;
	SimulatingState simulating_state_;
	PlaneMesh* plane_mesh_;
	MeshInstance* plane_;
	bool wireframe_;
	std::vector<BaseShader*> shaders_;
};

#endif