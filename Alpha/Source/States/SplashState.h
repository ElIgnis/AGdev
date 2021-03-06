#ifndef SPLASH_STATE_H
#define SPLASH_STATE_H

#include "..\GameState.h"
#include "..\SceneManager\SceneManager_Splash.h"
#include "..\Application.h"
#include "MenuState.h"

class SplashState : public GameState
{
private:
	static SplashState splashState;

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

	static SplashState* Instance()
	{
		return &splashState;
	}

protected:
	SplashState();
	virtual ~SplashState();

	SceneManagerTransition* scene;
};

#endif