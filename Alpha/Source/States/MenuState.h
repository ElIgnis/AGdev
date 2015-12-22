#ifndef MENU_STATE__H
#define MENU_STATE__H

#include "..\GameState.h"
#include "..\SceneManager\SceneManager_Menu.h"
#include "..\Application.h"
#include "HelpState.h"
#include "OptionState.h"
#include "HighScoreState.h"
#include "LevelSelectState.h"

class MenuState : public GameState
{
private:
	static MenuState menuState;

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

	static MenuState* Instance()
	{
		return &menuState;
	}

protected:
	MenuState();
	virtual ~MenuState();

	SceneManagerSelection* scene;
};

#endif