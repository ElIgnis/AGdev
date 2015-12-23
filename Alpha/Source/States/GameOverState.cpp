#include "GameOverState.h"

GameOverState GameOverState::gameOverState;

GameOverState::GameOverState()
{
	this->update = true;
	this->draw = true;
	this->stateName = "GameOver";
}

GameOverState::~GameOverState()
{
}

void GameOverState::Init(const int width, const int height, ResourcePool* RP, InputManager* controls)
{
	this->update = true;
	this->draw = true;
	this->stateName = "GameOver";

	this->scene = new SceneManager_GameOver();

	scene->Init(width, height, RP, controls);

	Application::getMouse()->disableDeadZone();
}

void GameOverState::CleanUp()
{
	if (scene)
	{
		scene->Exit();
		delete scene;
		scene = NULL;
	}
}

void GameOverState::Config()
{
}

void GameOverState::GameOver()
{
	this->update = false;
	this->draw = false;
}

void GameOverState::Resume()
{
	this->scene->BindShaders();
	this->update = true;
	this->draw = true;
}

void GameOverState::HandleEvents(GameStateManager* gameStateManager)
{
	for (unsigned i = 0; i < scene->interactiveButtons.size(); ++i)
	{
		if (scene->interactiveButtons[i].getStatus() == Button2D::BUTTON_RELEASED)
		{
			if (scene->interactiveButtons[i].getName() == "Menu")
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

void GameOverState::HandleEvents(GameStateManager* gameStateManager, const unsigned char key, const bool status)
{
}

void GameOverState::HandleEvents(GameStateManager* gameStateManager, const double mouse_x, const double mouse_y, const int button_Left, const int button_Middle, const int button_Right)
{
}

void GameOverState::HandleEvents(GameStateManager* gameStateManager, const double yaw, const double pitch)
{
}

void GameOverState::Update(GameStateManager* gameStateManager)
{
}

void GameOverState::Update(GameStateManager* gsm, const double dt)
{
	if (update)
	{
		scene->Update(dt);
	}
}

void GameOverState::Draw(GameStateManager* gameStateManager)
{
	if (draw)
	{
		scene->Render();
	}
}