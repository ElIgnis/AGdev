#include "PlayState_L2.h"

PlayState_L2 PlayState_L2::playState_L2;

PlayState_L2::PlayState_L2()
{
	this->update = true;
	this->draw = true;
	this->stateName = "Play";
}

PlayState_L2::~PlayState_L2()
{
}

void PlayState_L2::Init(const int width, const int height, ResourcePool* RP, InputManager* controls)
{
	this->update = true;
	this->draw = true;
	this->stateName = "Play";

	this->scene = new SceneManager_Play_L2();

	scene->Init(width, height, RP, controls);

	Application::getMouse()->enableDeadZone();
}

void PlayState_L2::CleanUp()
{
	if (scene)
	{
		scene->Exit();
		delete scene;
		scene = NULL;
	}
}

void PlayState_L2::Config()
{
}

void PlayState_L2::Pause()
{
	this->update = false;
	this->draw = false;

	Application::getMouse()->disableDeadZone();
}

void PlayState_L2::Resume()
{
	this->scene->BindShaders();
	this->update = true;
	this->draw = true;

	Application::getMouse()->enableDeadZone();
}

void PlayState_L2::HandleEvents(GameStateManager* gameStateManager)
{
	if (scene->inputManager->getKey("Pause") && !scene->GetGameOver())
	{
		gameStateManager->PushState(PauseState::Instance());
		this->draw = true;
		this->update = true;
		gameStateManager->SetOverrideRender(true);
	}
	else if (scene->GetGameOver())
	{
		gameStateManager->PushState(GameOverState::Instance());
		this->draw = true;
		this->update = true;
		gameStateManager->SetOverrideRender(true);
	}
}

void PlayState_L2::HandleEvents(GameStateManager* gameStateManager, const unsigned char key, const bool status)
{
}

void PlayState_L2::HandleEvents(GameStateManager* gameStateManager, const double mouse_x, const double mouse_y, const int button_Left, const int button_Middle, const int button_Right)
{
}

void PlayState_L2::HandleEvents(GameStateManager* gameStateManager, const double yaw, const double pitch)
{
}

void PlayState_L2::Update(GameStateManager* gameStateManager)
{
}

void PlayState_L2::Update(GameStateManager* gsm, const double dt)
{
	if (update)
	{
		scene->Update(dt);
	}
}

void PlayState_L2::Draw(GameStateManager* gameStateManager)
{
	if (draw)
	{
		scene->Render();
	}
}