#include "LevelSelectState.h"


LeveSelectState LeveSelectState::levelSelectState;

LeveSelectState::LeveSelectState()
{
	this->update = true;
	this->draw = true;
	this->stateName = "Level Select";
}

LeveSelectState::~LeveSelectState()
{
}

void LeveSelectState::Init(const int width, const int height, ResourcePool* RP, InputManager* controls)
{
	this->update = true;
	this->draw = true;
	this->stateName = "Level Select";

	this->scene = new SceneManager_LevelSelect();

	scene->Init(width, height, RP, controls);
}

void LeveSelectState::CleanUp()
{
	if (scene)
	{
		scene->Exit();
		delete scene;
		scene = NULL;
	}
}

void LeveSelectState::Config()
{
}

void LeveSelectState::Pause()
{
	this->update = false;
	this->draw = false;
}

void LeveSelectState::Resume()
{
	this->scene->BindShaders();
	this->update = true;
	this->draw = true;
}

void LeveSelectState::HandleEvents(GameStateManager* gameStateManager)
{
	for (unsigned i = 0; i < scene->interactiveButtons.size(); ++i)
	{
		if (scene->interactiveButtons[i].getStatus() == Button2D::BUTTON_RELEASED)
		{
			if (scene->interactiveButtons[i].getName() == "LevelOne")
			{
				gameStateManager->PushState(PlayState::Instance());
			}

			else if (scene->interactiveButtons[i].getName() == "LevelTwo")
			{
				//gameStateManager->PushState(PlayState::Instance());
			}

			else if (scene->interactiveButtons[i].getName() == "Back")
			{
				gameStateManager->ChangeState(MenuState::Instance());
			}

			break;
		}
	}
}

void LeveSelectState::HandleEvents(GameStateManager* gameStateManager, const unsigned char key, const bool status)
{
}

void LeveSelectState::HandleEvents(GameStateManager* gameStateManager, const double mouse_x, const double mouse_y, const int button_Left, const int button_Middle, const int button_Right)
{
}

void LeveSelectState::HandleEvents(GameStateManager* gameStateManager, const double yaw, const double pitch)
{
}

void LeveSelectState::Update(GameStateManager* gameStateManager)
{
}

void LeveSelectState::Update(GameStateManager* gsm, const double dt)
{
	if (update)
	{
		scene->Update(dt);
	}
}

void LeveSelectState::Draw(GameStateManager* gameStateManager)
{
	if (draw)
	{
		scene->Render();
	}
}