#include "OptionState.h"

OptionState OptionState::optionState;

OptionState::OptionState()
{
	this->update = true;
	this->draw = true;
	this->stateName = "Options";
}

OptionState::~OptionState()
{
}

void OptionState::Init(const int width, const int height, ResourcePool* RP, InputManager* controls)
{
	this->update = true;
	this->draw = true;
	this->stateName = "Options";

	this->scene = new SceneManager_Options();

	scene->Init(width, height, RP, controls);
}

void OptionState::CleanUp()
{
	if (scene)
	{
		scene->Exit();
		delete scene;
		scene = NULL;
	}
}

void OptionState::Config()
{
}

void OptionState::Pause()
{
	this->update = false;
	this->draw = false;
}

void OptionState::Resume()
{
	this->scene->BindShaders();
	this->update = true;
	this->draw = true;
}

void OptionState::HandleEvents(GameStateManager* gameStateManager)
{
	for (unsigned i = 0; i < scene->interactiveButtons.size(); ++i)
	{
		if (scene->interactiveButtons[i].getStatus() == Button2D::BUTTON_RELEASED)
		{
			if (scene->interactiveButtons[i].getName() == "Back")
			{
				gameStateManager->ChangeState(MenuState::Instance());
			}
			break;
		}
	}
}

void OptionState::HandleEvents(GameStateManager* gameStateManager, const unsigned char key, const bool status)
{
}

void OptionState::HandleEvents(GameStateManager* gameStateManager, const double mouse_x, const double mouse_y, const int button_Left, const int button_Middle, const int button_Right)
{
}

void OptionState::HandleEvents(GameStateManager* gameStateManager, const double yaw, const double pitch)
{
}

void OptionState::Update(GameStateManager* gameStateManager)
{
}

void OptionState::Update(GameStateManager* gsm, const double dt)
{
	if (update)
	{
		scene->Update(dt);
	}
}

void OptionState::Draw(GameStateManager* gameStateManager)
{
	if (draw)
	{
		scene->Render();
	}
}