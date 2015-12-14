#include "SplashState.h"

SplashState SplashState::splashState;

SplashState::SplashState()
{
	this->update = true;
	this->draw = true;
	this->stateName = "Splash";
}

SplashState::~SplashState()
{
}

void SplashState::Init(const int width, const int height, ResourcePool* RP, InputManager* controls)
{
	this->update = true;
	this->draw = true;
	this->stateName = "Splash";

	this->scene = new SceneManager_Splash();

	scene->Init(width, height, RP, controls);
}

void SplashState::CleanUp()
{
	if (scene)
	{
		scene->Exit();
		delete scene;
		scene = NULL;
	}
}

void SplashState::Config()
{
}

void SplashState::Pause()
{
	this->update = false;
	this->draw = false;
}

void SplashState::Resume()
{
	this->scene->BindShaders();
	this->update = true;
	this->draw = true;
}

void SplashState::HandleEvents(GameStateManager* gameStateManager)
{
	// if the splash screen completes
	if (scene->getExit())
	{
		gameStateManager->PushState(MenuState::Instance());
	}
}

void SplashState::HandleEvents(GameStateManager* gameStateManager, const unsigned char key, const bool status)
{
}

void SplashState::HandleEvents(GameStateManager* gameStateManager, const double mouse_x, const double mouse_y, const int button_Left, const int button_Middle, const int button_Right)
{
}

void SplashState::HandleEvents(GameStateManager* gameStateManager, const double yaw, const double pitch)
{
}

void SplashState::Update(GameStateManager* gameStateManager)
{
}

void SplashState::Update(GameStateManager* gsm, const double dt)
{
	if (update)
	{
		scene->Update(dt);
	}
}

void SplashState::Draw(GameStateManager* gameStateManager)
{
	if (draw)
	{
		scene->Render();
	}
}