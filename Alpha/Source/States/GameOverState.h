#ifndef GAMEOVER_STATE_H
#define GAMEOVER_STATE_H

#include "..\GameState.h"
#include "..\SceneManager\SceneManager_GameOver.h"
#include "..\Application.h"
#include "MenuState.h"

class GameOverState : public GameState
{
private:
	static GameOverState gameOverState;

public:
	void Init(const int width, const int height, ResourcePool* RP, InputManager* controls);
	void CleanUp();

	void Config();

	void GameOver();
	void Resume();

	void HandleEvents(GameStateManager* gameStateManager);
	void HandleEvents(GameStateManager* gameStateManager, const unsigned char key, const bool status = true);
	void HandleEvents(GameStateManager* gameStateManager, const double mouse_x, const double mouse_y, const int button_Left, const int button_Middle, const int button_Right);
	void HandleEvents(GameStateManager* gameStateManager, const double yaw, const double pitch);

	void Update(GameStateManager* gameStateManager);
	void Update(GameStateManager* gameStateManager, const double dt);

	void Draw(GameStateManager* gameStateManager);

	static GameOverState* Instance()
	{
		return &gameOverState;
	}

protected:
	GameOverState();
	virtual ~GameOverState();

	SceneManagerSelection* scene;
};

#endif