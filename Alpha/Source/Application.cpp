#include "Application.h"

int Application::windowWidth = 1;
int Application::windowHeight = 1;
Mouse* Application::mouse = NULL;
Keyboard* Application::keyboard = NULL;

// opengl stuff
//Define an error callback
static void error_callback(int error, const char* description)
{
	fputs(description, stderr);
	_fgetchar();
}

//Define the key input callback
static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);
}

/********************************************************************************
Callback function when the window is resized
********************************************************************************/
void resize_callback(GLFWwindow* window, int w, int h)
{
	glViewport(0, 0, w, h);
	Application::setWindowWidth(w);
	Application::setWindowHeight(h);
}

Mouse* Application::getMouse(void)
{
	return mouse;
}

Keyboard* Application::getKeyboard(void)
{
	return keyboard;
}

bool Application::getKeyboardUpdate(void)
{
	this->keyboard->Update();

	return true;
}

bool Application::getMouseUpdate(void)
{
	this->mouse->Update();

	return true;
}

void Application::setWindowWidth(int width)
{
	windowWidth = width;
}

void Application::setWindowHeight(int height)
{
	windowHeight = height;
}

int Application::getWindowWidth(void)
{
	return windowWidth;

}

int Application::getWindowHeight(void)
{
	return windowHeight;
}

// Application class
Application::Application() 
: gameStateManager(NULL)
{
}

Application::~Application()
{
	if (gameStateManager)
	{
		delete gameStateManager;
		gameStateManager = NULL;
	}

	if (this->mouse != NULL)
	{
		delete mouse;
		mouse = NULL;
	}
	if (this->keyboard != NULL)
	{
		delete keyboard;
		mouse = NULL;
	}
}

// Setting up of the engine using data from the text files
void Application::SetUp(void)
{
	lua_State *Lua_Init = lua_open();

	//Load the libs
	luaL_openlibs(Lua_Init);

	//Initialise engine with values from Lua file
	if (luaL_loadfile(Lua_Init, "Lua/AppConfig.Lua") || lua_pcall(Lua_Init, 0, 0, 0))
	{
		printf("error: %s", lua_tostring(Lua_Init, -1));
	}

	//Init OpenGL window
	lua_getglobal(Lua_Init, "TITLE");
	if (!lua_isstring(Lua_Init, TITLE))
	{
		printf("Invalid title name specified\n");
	}

	lua_getglobal(Lua_Init, "WIDTH");
	if (!lua_isnumber(Lua_Init, WIDTH))
	{
		printf("Invalid width value specified\n");
	}

	lua_getglobal(Lua_Init, "HEIGHT");
	if (!lua_isnumber(Lua_Init, HEIGHT))
	{
		printf("Invalid height value specified\n");
	}

	lua_getglobal(Lua_Init, "FULLSCREEN");
	if (!lua_isboolean(Lua_Init, FULLSCREEN))
	{
		printf("Invalid screen type specified\n");
	}

	lua_getglobal(Lua_Init, "FRAMERATE");
	if (!lua_isnumber(Lua_Init, FRAMERATE))
	{
		printf("Invalid frame rate value specified\n");
	}

	//Init Mouse
	lua_getglobal(Lua_Init, "INIT_MOUSE");
	if (!lua_isstring(Lua_Init, MOUSE_SETTING))
	{
		printf("Invalid mouse init file name specified\n");
	}

	//Init Resources
	lua_getglobal(Lua_Init, "INIT_INPUT");
	if (!lua_isstring(Lua_Init, INPUT_SETTING))
	{
		printf("Invalid resource init file name specified\n");
	}

	//Init Input
	lua_getglobal(Lua_Init, "INIT_RESOURCE");
	if (!lua_isstring(Lua_Init, RESOURCE_SETTING))
	{
		printf("Invalid input init file name specified\n");
	}

	//Initialise OpenGL window
	string EngineInitString;
	int EngineInitInt;
	bool EngineInitBool;
	//Title
	EngineInitString = (string)lua_tostring(Lua_Init, TITLE);
	this->windowTitle = EngineInitString;
	//Screen width
	EngineInitInt = (int)lua_tonumber(Lua_Init, WIDTH);
	this->windowWidth = EngineInitInt;
	//Screen height
	EngineInitInt = (int)lua_tonumber(Lua_Init, HEIGHT);
	this->windowHeight = EngineInitInt;
	//Full screen
	EngineInitBool = (bool)lua_toboolean(Lua_Init, FULLSCREEN);
	this->fullScreen = EngineInitBool;
	//Framerate
	EngineInitInt = (int)lua_tonumber(Lua_Init, FRAMERATE);
	this->FPS = EngineInitInt;
	this->frameTime = 1000 / FPS;

	//Initialise the mouse
	EngineInitString = (string)lua_tostring(Lua_Init, MOUSE_SETTING);
	this->mouse->Init(EngineInitString);

	//Initialise the inputs
	EngineInitString = (string)lua_tostring(Lua_Init, INPUT_SETTING);
	this->inputConfig = EngineInitString;

	//Initialise the resources
	EngineInitString = (string)lua_tostring(Lua_Init, RESOURCE_SETTING);
	this->resourceConfig = EngineInitString;

	//Close the lua file
	lua_close(Lua_Init);
}

// Init the engine settings
void Application::Init()
{
	this->mouse = new Mouse();
	this->keyboard = new Keyboard();
	this->gameStateManager = new GameStateManager();
	
	//Set the error callback
	glfwSetErrorCallback(error_callback);

	//Initialize GLFW
	if (!glfwInit())
	{
		exit(EXIT_FAILURE);
	}

	//Set the GLFW window creation hints - these are optional
	glfwWindowHint(GLFW_SAMPLES, 4); //Request 4x antialiasing
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); //Request a specific OpenGL version
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3); //Request a specific OpenGL version
	//glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); //We don't want the old OpenGL 

	SetUp();

	//Create a window and create its OpenGL context
	const char* title = windowTitle.c_str();

	if (fullScreen)
	{
		m_window = glfwCreateWindow(windowWidth, windowHeight, title, glfwGetPrimaryMonitor(), NULL);
	}
	else
	{
		m_window = glfwCreateWindow(windowWidth, windowHeight, title, NULL, NULL);
	}
	this->mouse->m_window = m_window;

	//If the window couldn't be created
	if (!m_window)
	{
		fprintf(stderr, "Failed to open GLFW window.\n");
		glfwTerminate();
		exit(EXIT_FAILURE);
	}

	//This function makes the context of the specified window current on the calling thread. 
	glfwMakeContextCurrent(m_window);

	//Sets the key callback
	//glfwSetKeyCallback(m_window, key_callback);
	glfwSetWindowSizeCallback(m_window, resize_callback);

	glewExperimental = true; // Needed for core profile
	//Initialize GLEW
	GLenum err = glewInit();

	//If GLEW hasn't initialized
	if (err != GLEW_OK)
	{
		fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
		//return -1;
	}

	// Hide the cursor
	glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);

	// Set these 2 variables to zero
	m_dElapsedTime = 0.0;
	m_dAccumulatedTime_ThreadOne = 0.0;
	m_dAccumulatedTime_ThreadTwo = 0.0;

	// Init the GameState Manager
	this->gameStateManager->Init(windowTitle, windowWidth, windowHeight);
	this->gameStateManager->InitResources(resourceConfig);
	this->gameStateManager->InitControls(inputConfig);
	//this->gameStateManager->PushState(SplashStateCM::GetInstance());
	this->gameStateManager->PushState(SplashState::Instance());
}

// main game loop
void Application::Run(void)
{
	m_timer.startTimer();	// Start timer to calculate how long it takes to render this frame

	while (!glfwWindowShouldClose(m_window) && gameStateManager->Running())
	{
		// Get the elapsed time
		m_dElapsedTime = m_timer.getElapsedTime();
		m_dAccumulatedTime_ThreadOne += m_dElapsedTime;
		m_dAccumulatedTime_ThreadTwo += m_dElapsedTime;
		if (m_dAccumulatedTime_ThreadOne > 0.03)
		{
			getMouseUpdate();
			getKeyboardUpdate();
			
			if (gameStateManager->validCheck())
			{
				gameStateManager->HandleEvents();
				gameStateManager->Update(m_dElapsedTime);
			}
			
			m_dAccumulatedTime_ThreadOne = 0.0;
		}

		if (m_dAccumulatedTime_ThreadTwo > 1.0)
		{
			m_dAccumulatedTime_ThreadTwo = 0.0;
		}
		// Render the scene
		
		if (gameStateManager->validCheck())
		{
			gameStateManager->Draw();
		}

		//Swap buffers
		glfwSwapBuffers(m_window);
		//Get and organize events, like keyboard and mouse input, window resizing, etc...
		glfwPollEvents();
		m_timer.waitUntil(frameTime);       // Frame rate limiter. Limits each frame to a specified time in ms.   
	}

	// Delete the scene
	if (gameStateManager->validCheck())
	{
		gameStateManager->CleanUp();
	}
}

// clean up
void Application::Exit(void)
{
	if (gameStateManager)
	{
		delete gameStateManager;
		gameStateManager = NULL;
	}
	//Close OpenGL window and terminate GLFW
	glfwDestroyWindow(m_window);
	//Finalize and clean up GLFW
	glfwTerminate();
}