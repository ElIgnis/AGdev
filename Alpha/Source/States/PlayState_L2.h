#ifndef PLAY_STATE_L2_H
#define PLAY_STATE_L2_H

#include "..\GameState.h"
#include "..\SceneManager\SceneManager_Play_L2.h"
#include "..\Application.h"
#include "PauseState.h"
#include "GameOverState.h"

class PlayState_L2 : public GameState
{
private:
	static PlayState_L2 playState_L2;

public:
	void Init(const int width, const int height, ResourcePool* RP, InputManager* controls);
	void CleanUp();

	void Config();

	void Pause();
	void Resume();

	void HandleEvents(GameStateManager* gameStateManager);
	void HandleEvents(GameStateManager* gameStateManager, const unsigned char key, const bool status = true);
	void HandleEvents(GameStateManager* gameStateManager, const double mouse_x, const double mouse_y, const int button_Left, const int button_Middle, const int button_Right);
	void HandleEvents(GameStateManager* gameStateManager, const double yaw, const double pitch);

	void Update(GameStateManager* gameStateManager);
	void Update(GameStateManager* gameStateManager, const double dt);

	void Draw(GameStateManager* gameStateManager);

	static PlayState_L2* Instance()
	{
		return &playState_L2;
	}

protected:
	PlayState_L2();
	virtual ~PlayState_L2();

	SceneManagerGameplay* scene;
};

#endif