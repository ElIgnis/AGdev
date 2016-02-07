#ifndef APPLICATION_H
#define APPLICATION_H

// Include GLEW
#include <GL/glew.h>

// Include the standard C++ headers
#include <stdio.h>
#include <stdlib.h>
#include <string>

#include "timer.h"
#include "Mouse.h"
#include "Keyboard.h"
#include "GameStateManager.h"

//#include "SplashStateCM.h"
#include "../Source/States/SplashState.h"
#include "Lua\LuaFileLoader.h"

#define FIRST 0

using std::string;

class Application
{
public:
	enum INIT{
		TITLE = 1,
		WIDTH,
		HEIGHT,
		FULLSCREEN,
		FRAMERATE,
		MOUSE_SETTING,
		INPUT_SETTING,
		RESOURCE_SETTING,
	};

	static Application& getInstance()
	{
		static Application app;
		return app;
	}

	void Init();
	void Run(void);
	void Exit(void);

	static Mouse* getMouse(void);
	static Keyboard* getKeyboard(void);

	bool getKeyboardUpdate(void);
	bool getMouseUpdate(void);

	static void setWindowWidth(int width);
	static void setWindowHeight(int height);

	static int getWindowWidth(void);
	static int getWindowHeight(void);

private:
	Application();
	~Application();

	void SetUp(void);

	GLFWwindow* m_window;
	StopWatch m_timer;
	double m_dElapsedTime;
	double m_dAccumulatedTime_ThreadOne;
	double m_dAccumulatedTime_ThreadTwo;

	static Mouse *mouse;
	static Keyboard *keyboard;

	unsigned char FPS;
	unsigned int frameTime;

	/* following data types will be moved to GameStateManager */
	static int windowWidth;
	static int windowHeight;
	string windowTitle;
	bool fullScreen;

	GameStateManager* gameStateManager;
	string inputConfig;
	string resourceConfig;
};

#endif