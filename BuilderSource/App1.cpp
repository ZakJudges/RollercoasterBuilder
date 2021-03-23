
#include "App1.h"

App1::App1()
{
	line_controller_ = nullptr;
	track_ = nullptr;
	application_state_ = nullptr;
	wireframe_ = false;
	track_mesh_ = nullptr;
	plane_mesh_ = nullptr;
	plane_ = nullptr;
}

void App1::init(HINSTANCE hinstance, HWND hwnd, int screenWidth, int screenHeight, Input *in)
{
	BaseApplication::init(hinstance, hwnd, screenWidth, screenHeight, in);

	textureMgr->loadTexture("default", L"../res/DefaultDiffuse.png");
	textureMgr->loadTexture("metal4", L"../res/metal4.png");
	textureMgr->loadTexture("metal2", L"../res/metal2.png");
	textureMgr->loadTexture("metal3", L"../res/metal3.png");
	textureMgr->loadTexture("metal", L"../res/metal.png");

	plane_mesh_ = new PlaneMesh(renderer->getDevice(), renderer->getDeviceContext());

	//	Create Shader objects.
	ColourShader* colour_shader = new ColourShader(renderer->getDevice(), hwnd);
	shaders_.push_back(colour_shader);
	DefaultShader* default_shader = new DefaultShader(renderer->getDevice(), hwnd);
	shaders_.push_back(default_shader);

	//	Create Mesh instances and assign shaders.
	plane_ = new MeshInstance(textureMgr->getTexture("default"), colour_shader , plane_mesh_);
	if (plane_)
	{
		XMMATRIX translation_matrix, scale_matrix;
		scale_matrix = XMMatrixScaling(50.0f, 1.0f, 50.0f);
		translation_matrix = XMMatrixTranslation(-150.0f, -3.0f, -70.0f);
		plane_->SetWorldMatrix(scale_matrix * translation_matrix * renderer->getWorldMatrix());
		objects_.push_back(plane_);
	}

	track_mesh_ = new TrackMesh(renderer->getDevice(), renderer->getDeviceContext(), colour_shader, 40);
	track_mesh_->SetLargeRailTexture(textureMgr->getTexture("metal"));
	track_mesh_->SetSmallRailTexture(textureMgr->getTexture("metal3"));
	track_mesh_->SetCrossTieTexture(textureMgr->getTexture("metal4"));

	std::vector<MeshInstance*> track_instances = track_mesh_->GetTrackMeshInstances();
	for (int i = 0; i < track_instances.size(); i++)
	{
		objects_.push_back(track_instances[i]);
	}

	line_controller_ = new LineController(renderer->getDevice(), renderer->getDeviceContext(), default_shader, 6);

	camera = &default_camera_;
	camera->setPosition(0.0f, 1.0f, -10.0f);
	camera->update();

	track_ = new Track(1000, track_mesh_);
	
	//Initialise Application States:
	building_state_.Init(track_);
	building_state_.SetScreenWidth(screenWidth);
	simulating_state_.Init(track_);
	simulating_state_.SetScreenWidth(screenWidth);
	simulating_state_.SetLineController(line_controller_);
	building_state_.SetLineController(line_controller_);
	application_state_ = &building_state_;
}

App1::~App1()
{
	BaseApplication::~BaseApplication();

	objects_.clear();

	if (line_controller_)
	{
		delete line_controller_;
		line_controller_ = 0;
	}

	if (track_)
	{
		delete track_;
		track_ = 0;
	}

	if (track_mesh_)
	{
		delete track_mesh_;
		track_mesh_ = 0;
	}

	if (plane_mesh_)
	{
		delete plane_mesh_;
		plane_mesh_ = 0;
	}

	if (plane_)
	{
		delete plane_;
		plane_ = 0;
	}

	for (int i = 0; i < shaders_.size(); i++)
	{
		if (shaders_[i])
		{
			delete shaders_[i];
			shaders_[i] = 0;
		}
	}
	shaders_.clear();
}

bool App1::frame()
{
	bool result;

	result = BaseApplication::frame();
	if (!result)
	{
		return false;
	}

	if (application_state_)
	{
		if (application_state_->Exit())
		{
			SwitchApplicationState(application_state_->OnExit());
		}

		//	Handle application-state-specific input.
		StateInput();

		application_state_->Update(timer->getTime());

		//	Update the coaster camera.
		if (application_state_ == &simulating_state_)
		{
			coaster_camera_.CalculateMatrix(track_->GetCameraEye(), track_->GetCameraLookAt(), track_->GetCameraUp(), track_->GetTrackMesh()->GetWorldMatrix());
		}

		if (!application_state_->ApplicationRunning())
		{
			return false;
		}
	}

	// Render the graphics.
	result = render();
	if (!result)
	{
		return false;
	}
	
	return true;
}

bool App1::render()
{
	//	Add new mesh instances that have been created.
	if (track_->GetTrackMesh()->HasNewInstances())
	{
		std::vector<MeshInstance*> new_instances = track_->GetTrackMesh()->GetNewInstances();

		for (int i = 0; i < new_instances.size(); i++)
		{
			objects_.push_back(new_instances.at(i));
		}
	}

	//	Remove mesh instances that are no longer used.
	if (track_->GetTrackMesh()->InstancesPendingRemoval())
	{
		//	Remove any of the instances pending removal from the container of objects.
		std::vector<MeshInstance*> pending_removal = track_->GetTrackMesh()->GetInstancesForRemoval();
		for (int i = 0; i < pending_removal.size(); i++)
		{
			//	Loop in reverse order so that addresses are still valid after erasure.
			for (int j = objects_.size() - 1; j >= 0; j--)
			{
				if (pending_removal[i] == objects_[j])
				{
					objects_.erase(objects_.begin() + j);
				}
			}
		}

		track_->GetTrackMesh()->RemoveUnusedInstances();
	}

	XMMATRIX worldMatrix, viewMatrix, projectionMatrix;

	renderer->setWireframeMode(application_state_->GetWireframeState());

	// Clear the scene. 
	renderer->beginScene(0.38f, 0.39f, 0.44f, 1.0f);
	// Generate the view matrix based on the camera's position.
	camera->update();

	// Get the world, view, projection matrices from the camera and Direct3D objects.
	worldMatrix = renderer->getWorldMatrix();
	viewMatrix = camera->getViewMatrix();
	projectionMatrix = renderer->getProjectionMatrix();

	//	Render all mesh instances.
	for (int i = 0; i < objects_.size(); i++)
	{
		objects_.at(i)->Render(renderer->getDeviceContext(), viewMatrix, projectionMatrix);
	}

	line_controller_->Render(renderer->getDeviceContext(), worldMatrix, viewMatrix, projectionMatrix);

	// Render GUI
	gui();

	// Present the rendered scene to the screen.
	renderer->endScene();

	return true;
}

void App1::SwitchApplicationState(ApplicationState::APPLICATIONSTATE state)
{
	switch (state)
	{
	case ApplicationState::APPLICATIONSTATE::BUILDING_STATE: 
		application_state_ = &building_state_;
		camera = &default_camera_;
		break;

	case ApplicationState::APPLICATIONSTATE::SIMULATING_STATE:
		application_state_ = &simulating_state_;
		camera = &coaster_camera_;
		break;
	}

	application_state_->OnEnter();
}

void App1::StateInput()
{
	XMVECTOR look_at = camera->GetLookAt();
	application_state_->SetCameraLookAt(XMVectorGetX(look_at), XMVectorGetY(look_at), XMVectorGetZ(look_at));

	//	If the application is out of focus then do not process user input.
	if (!application_state_->GetFocus())
	{
		input->DeactivateInput();
	}
	else
	{
		input->ActivateInput();
	}

	if (input->IsActive())
	{
		if (input->isKeyDown(VK_SHIFT))
		{
			input->DeactivateInput();
			
			if (input->isKeyDown('W'))
			{
				application_state_->OnWPress();
			}
			if (input->isKeyDown('S'))
			{
				application_state_->OnSPress();
			}
			if (input->isKeyDown('D'))
			{
				application_state_->OnDPress();
			}
			if (input->isKeyDown('A'))
			{
				application_state_->OnAPress();
			}
			if (input->isKeyDown('E'))
			{
				application_state_->OnEPress();
			}
			if (input->isKeyDown('Q'))
			{
				application_state_->OnQPress();
			}

		}
		else
		{
			input->ActivateInput();
		}
	}
}

void App1::gui()
{
	// Force turn off on Geometry shader
	renderer->getDeviceContext()->GSSetShader(NULL, NULL, 0);

	// Build UI
	if (application_state_->ShowFPS())
	{
		ImGui::Text("FPS: %.f", timer->getFPS());
	}

	application_state_->RenderUI();

	// Render UI
	ImGui::Render();
}

