#include "MenuState.h"


MenuState MenuState::menuState;

MenuState::MenuState()
{
	this->update = true;
	this->draw = true;
	this->stateName = "Menu";
}

MenuState::~MenuState()
{
}

void MenuState::Init(const int width, const int height, ResourcePool* RP, InputManager* controls)
{
	this->update = true;
	this->draw = true;
	this->stateName = "Menu";

	this->scene = new SceneManager_Menu();

	scene->Init(width, height, RP, controls);
}

void MenuState::CleanUp()
{
	if (scene)
	{
		scene->Exit();
		delete scene;
		scene = NULL;
	}
}

void MenuState::Config()
{
}

void MenuState::Pause()
{
	this->update = false;
	this->draw = false;
}

void MenuState::Resume()
{
	this->scene->BindShaders();
	this->update = true;
	this->draw = true;
}

void MenuState::HandleEvents(GameStateManager* gameStateManager)
{
	for (unsigned i = 0; i < scene->interactiveButtons.size(); ++i)
	{
		if (scene->interactiveButtons[i].getStatus() == Button2D::BUTTON_RELEASED)
		{
			if (scene->interactiveButtons[i].getName() == "Play")
			{
				gameStateManager->PushState(PlayState::Instance());
			}
			else if(scene->interactiveButtons[i].getName() == "LevelSelect")
			{
				gameStateManager->PushState(LeveSelectState::Instance());
			}
			else if(scene->interactiveButtons[i].getName() == "Options")
			{
				gameStateManager->ChangeState(OptionState::Instance());
			}
			else if(scene->interactiveButtons[i].getName() == "HighScore")
			{
				gameStateManager->ChangeState(HighScoreState::Instance());
			}
			else if(scene->interactiveButtons[i].getName() == "Help")
			{
				gameStateManager->ChangeState(HelpState::Instance());
			}

			else if (scene->interactiveButtons[i].getName() == "Quit")
			{
				gameStateManager->Quit();
			}

			break;
		}
	}
}

void MenuState::HandleEvents(GameStateManager* gameStateManager, const unsigned char key, const bool status)
{
}

void MenuState::HandleEvents(GameStateManager* gameStateManager, const double mouse_x, const double mouse_y, const int button_Left, const int button_Middle, const int button_Right)
{
}

void MenuState::HandleEvents(GameStateManager* gameStateManager, const double yaw, const double pitch)
{
}

void MenuState::Update(GameStateManager* gameStateManager)
{
}

void MenuState::Update(GameStateManager* gsm, const double dt)
{
	if (update)
	{
		scene->Update(dt);
	}
}

void MenuState::Draw(GameStateManager* gameStateManager)
{
	if (draw)
	{
		scene->Render();
	}
}