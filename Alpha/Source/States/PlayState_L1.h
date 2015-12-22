#ifndef PLAY_STATE_L1_H
#define PLAY_STATE_L1_H

#include "..\GameState.h"
#include "..\SceneManager\SceneManager_Play_L1.h"
#include "..\Application.h"
#include "PauseState.h"

class PlayState_L1 : public GameState
{
private:
	static PlayState_L1 playState_L1;

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

	static PlayState_L1* Instance()
	{
		return &playState_L1;
	}

protected:
	PlayState_L1();
	virtual ~PlayState_L1();

	SceneManagerGameplay* scene;
};

#endif