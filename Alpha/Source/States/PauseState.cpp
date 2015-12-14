#include "PauseState.h"


PauseState PauseState::pauseState;

PauseState::PauseState()
{
	this->update = true;
	this->draw = true;
	this->stateName = "Pause";
}

PauseState::~PauseState()
{
}

void PauseState::Init(const int width, const int height, ResourcePool* RP, InputManager* controls)
{
	this->update = true;
	this->draw = true;
	this->stateName = "Pause";

	this->scene = new SceneManager_Pause();

	scene->Init(width, height, RP, controls);

	Application::getMouse()->disableDeadZone();
}

void PauseState::CleanUp()
{
	if (scene)
	{
		scene->Exit();
		delete scene;
		scene = NULL;
	}
}

void PauseState::Config()
{
}

void PauseState::Pause()
{
	this->update = false;
	this->draw = false;
}

void PauseState::Resume()
{
	this->scene->BindShaders();
	this->update = true;
	this->draw = true;
}

void PauseState::HandleEvents(GameStateManager* gameStateManager)
{
	for (unsigned i = 0; i < scene->interactiveButtons.size(); ++i)
	{
		if (scene->interactiveButtons[i].getStatus() == Button2D::BUTTON_RELEASED)
		{
			if (scene->interactiveButtons[i].getName() == "Resume")
			{
				scene->interactiveButtons[i].setStatus(Button2D::BUTTON_IDLE);
				gameStateManager->PopState(this);
				
			}

			else if (scene->interactiveButtons[i].getName() == "Menu")
			{
				scene->interactiveButtons[i].setStatus(Button2D::BUTTON_IDLE);
				gameStateManager->PopToState("Menu");
			}

			gameStateManager->SetOverrideRender(false);

			break;
		}
	}

	/*if (scene->inputManager->getKey("Select"))
	{
		gameStateManager->PopToState("Menu");
		gameStateManager->SetOverrideRender(false);
	}

	else if (scene->inputManager->getKey("RSelect"))
	{
		gameStateManager->PopState(this);
		gameStateManager->SetOverrideRender(false);
	}*/
}

void PauseState::HandleEvents(GameStateManager* gameStateManager, const unsigned char key, const bool status)
{
}

void PauseState::HandleEvents(GameStateManager* gameStateManager, const double mouse_x, const double mouse_y, const int button_Left, const int button_Middle, const int button_Right)
{
}

void PauseState::HandleEvents(GameStateManager* gameStateManager, const double yaw, const double pitch)
{
}

void PauseState::Update(GameStateManager* gameStateManager)
{
}

void PauseState::Update(GameStateManager* gsm, const double dt)
{
	if (update)
	{
		scene->Update(dt);
	}
}

void PauseState::Draw(GameStateManager* gameStateManager)
{
	if (draw)
	{
		scene->Render();
	}
}