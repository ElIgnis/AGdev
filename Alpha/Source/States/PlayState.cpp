#include "PlayState.h"

PlayState PlayState::playState;

PlayState::PlayState()
{
	this->update = true;
	this->draw = true;
	this->stateName = "Play";
}

PlayState::~PlayState()
{
}

void PlayState::Init(const int width, const int height, ResourcePool* RP, InputManager* controls)
{
	this->update = true;
	this->draw = true;
	this->stateName = "Play";

	this->scene = new SceneManager_Play();

	scene->Init(width, height, RP, controls);

	Application::getMouse()->enableDeadZone();
}

void PlayState::CleanUp()
{
	if (scene)
	{
		scene->Exit();
		delete scene;
		scene = NULL;
	}
}

void PlayState::Config()
{
}

void PlayState::Pause()
{
	this->update = false;
	this->draw = false;

	Application::getMouse()->disableDeadZone();
}

void PlayState::Resume()
{
	this->scene->BindShaders();
	this->update = true;
	this->draw = true;

	Application::getMouse()->enableDeadZone();
}

void PlayState::HandleEvents(GameStateManager* gameStateManager)
{
	if (scene->inputManager->getKey("Pause"))
	{
		gameStateManager->PushState(PauseState::Instance());
		this->draw = true;
		this->update = true;
		gameStateManager->SetOverrideRender(true);
	}
}

void PlayState::HandleEvents(GameStateManager* gameStateManager, const unsigned char key, const bool status)
{
}

void PlayState::HandleEvents(GameStateManager* gameStateManager, const double mouse_x, const double mouse_y, const int button_Left, const int button_Middle, const int button_Right)
{
}

void PlayState::HandleEvents(GameStateManager* gameStateManager, const double yaw, const double pitch)
{
}

void PlayState::Update(GameStateManager* gameStateManager)
{
}

void PlayState::Update(GameStateManager* gsm, const double dt)
{
	if (update)
	{
		scene->Update(dt);
	}
}

void PlayState::Draw(GameStateManager* gameStateManager)
{
	if (draw)
	{
		scene->Render();
	}
}