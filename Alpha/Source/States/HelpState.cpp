#include "HelpState.h"

HelpState HelpState::helpState;

HelpState::HelpState()
{
	this->update = true;
	this->draw = true;
	this->stateName = "Help";
}

HelpState::~HelpState()
{
}

void HelpState::Init(const int width, const int height, ResourcePool* RP, InputManager* controls)
{
	this->update = true;
	this->draw = true;
	this->stateName = "Help";

	this->scene = new SceneManager_Help();

	scene->Init(width, height, RP, controls);
}

void HelpState::CleanUp()
{
	if (scene)
	{
		scene->Exit();
		delete scene;
		scene = NULL;
	}
}

void HelpState::Config()
{
}

void HelpState::Pause()
{
	this->update = false;
	this->draw = false;
}

void HelpState::Resume()
{
	this->scene->BindShaders();
	this->update = true;
	this->draw = true;
}

void HelpState::HandleEvents(GameStateManager* gameStateManager)
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

void HelpState::HandleEvents(GameStateManager* gameStateManager, const unsigned char key, const bool status)
{
}

void HelpState::HandleEvents(GameStateManager* gameStateManager, const double mouse_x, const double mouse_y, const int button_Left, const int button_Middle, const int button_Right)
{
}

void HelpState::HandleEvents(GameStateManager* gameStateManager, const double yaw, const double pitch)
{
}

void HelpState::Update(GameStateManager* gameStateManager)
{
}

void HelpState::Update(GameStateManager* gsm, const double dt)
{
	if (update)
	{
		scene->Update(dt);
	}
}

void HelpState::Draw(GameStateManager* gameStateManager)
{
	if (draw)
	{
		scene->Render();
	}
}