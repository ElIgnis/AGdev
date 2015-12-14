#include "HighScoreState.h"

HighScoreState HighScoreState::highscoreState;

HighScoreState::HighScoreState()
{
	this->update = true;
	this->draw = true;
	this->stateName = "HighScore";
}

HighScoreState::~HighScoreState()
{
}

void HighScoreState::Init(const int width, const int height, ResourcePool* RP, InputManager* controls)
{
	this->update = true;
	this->draw = true;
	this->stateName = "HighScore";
	 
	this->scene = new SceneManager_HighScore();

	scene->Init(width, height, RP, controls);
}

void HighScoreState::CleanUp()
{
	if (scene)
	{
		scene->Exit();
		delete scene;
		scene = NULL;
	}
}

void HighScoreState::Config()
{
}

void HighScoreState::Pause()
{
	this->update = false;
	this->draw = false;
}

void HighScoreState::Resume()
{
	this->scene->BindShaders();
	this->update = true;
	this->draw = true;
}

void HighScoreState::HandleEvents(GameStateManager* gameStateManager)
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

void HighScoreState::HandleEvents(GameStateManager* gameStateManager, const unsigned char key, const bool status)
{
}

void HighScoreState::HandleEvents(GameStateManager* gameStateManager, const double mouse_x, const double mouse_y, const int button_Left, const int button_Middle, const int button_Right)
{
}

void HighScoreState::HandleEvents(GameStateManager* gameStateManager, const double yaw, const double pitch)
{
}

void HighScoreState::Update(GameStateManager* gameStateManager)
{
}

void HighScoreState::Update(GameStateManager* gsm, const double dt)
{
	if (update)
	{
		scene->Update(dt);
	}
}

void HighScoreState::Draw(GameStateManager* gameStateManager)
{
	if (draw)
	{
		scene->Render();
	}
}