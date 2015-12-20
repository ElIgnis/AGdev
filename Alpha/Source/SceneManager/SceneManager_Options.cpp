#include "SceneManager_Options.h"

SceneManager_Options::SceneManager_Options()
{
}

SceneManager_Options::~SceneManager_Options()
{
}

void SceneManager_Options::Init(const int width, const int height, ResourcePool *RM, InputManager* controls)
{
	SceneManagerSelection::Init(width, height, RM, controls);

	Config();

	this->InitShader();

	// Projection matrix : 45� Field of View, 4:3 ratio, display range : 0.1 unit <-> 1000 units
	Mtx44 perspective;
	perspective.SetToPerspective(45.0f, 4.0f / 3.0f, 0.1f, 10000.0f);
	//perspective.SetToOrtho(-80, 80, -60, 60, -1000, 1000);
	projectionStack.LoadMatrix(perspective);

	mouseMesh = resourceManager.retrieveMesh("CURSOR");
}

void SceneManager_Options::Config()
{
	SceneManagerSelection::Config("Config\\GameStateConfig\\OptionsConfig.txt");
}

void SceneManager_Options::Update(double dt)
{
	SceneManagerSelection::Update(dt);

	UpdateMouse();
	UpdateSelection();

	lights[0].power = brightness;
	glUniform1f(parameters[U_LIGHT0_POWER], lights[0].power);
}

void SceneManager_Options::UpdateOptions(void)
{
	ofstream outFile;
	outFile.open("Config//Options.txt");
	if (outFile.good())
	{
		outFile << brightness << std::endl;
		outFile << tpCamera.GetVSense() << std::endl;
		outFile << tpCamera.GetHSense() << std::endl;
		outFile.close();
	}
	else
		std::cout << "Load Options data failed" << std::endl;
}

void SceneManager_Options::Render()
{
	SceneManagerSelection::ClearScreen();

	Mtx44 perspective;
	perspective.SetToPerspective(45.0f, 4.0f / 3.0f, 0.1f, 10000.0f);
	//perspective.SetToOrtho(-80, 80, -60, 60, -1000, 1000);
	projectionStack.LoadMatrix(perspective);

	// Set up the view
	viewStack.LoadIdentity();
	viewStack.LookAt(fpCamera.getPosition().x, fpCamera.getPosition().y, fpCamera.getPosition().z,
		fpCamera.getTarget().x, fpCamera.getTarget().y, fpCamera.getTarget().z,
		fpCamera.getUp().x, fpCamera.getUp().y, fpCamera.getUp().z);

	// Model matrix : an identity matrix (model will be at the origin)
	modelStack.LoadIdentity();

	RenderBG();
	RenderStaticObject();
	RenderMobileObject();
	RenderSelection();
	RenderLight(0, 0, 1, 0);
}

void SceneManager_Options::Exit()
{
	SceneManagerSelection::Exit();
}

void SceneManager_Options::BindShaders()
{
	SceneManagerSelection::BindShaders();
}

// Other specific init, update and render classes
void SceneManager_Options::InitShader()
{
	SHADER thisShader = resourceManager.retrieveShader("Comg");
	programID = LoadShaders(thisShader.vertexShaderDirectory.c_str(), thisShader.fragmentShaderDirectory.c_str());

	this->BindShaders();
	parameters.resize(U_TOTAL);
	lights.resize(2);

	// Get a handle for our uniform
	parameters[U_MVP] = glGetUniformLocation(programID, "MVP");
	//parameters[U_MODEL] = glGetUniformLocation(programID, "M");
	//parameters[U_VIEW] = glGetUniformLocation(programID, "V");
	parameters[U_MODELVIEW] = glGetUniformLocation(programID, "MV");
	parameters[U_MODELVIEW_INVERSE_TRANSPOSE] = glGetUniformLocation(programID, "MV_inverse_transpose");
	parameters[U_MATERIAL_AMBIENT] = glGetUniformLocation(programID, "material.kAmbient");
	parameters[U_MATERIAL_DIFFUSE] = glGetUniformLocation(programID, "material.kDiffuse");
	parameters[U_MATERIAL_SPECULAR] = glGetUniformLocation(programID, "material.kSpecular");
	parameters[U_MATERIAL_SHININESS] = glGetUniformLocation(programID, "material.kShininess");
	parameters[U_LIGHTENABLED] = glGetUniformLocation(programID, "lightEnabled");
	parameters[U_NUMLIGHTS] = glGetUniformLocation(programID, "numLights");

	parameters[U_LIGHT0_TYPE] = glGetUniformLocation(programID, "lights[0].type");
	parameters[U_LIGHT0_POSITION] = glGetUniformLocation(programID, "lights[0].position_cameraspace");
	parameters[U_LIGHT0_COLOR] = glGetUniformLocation(programID, "lights[0].color");
	parameters[U_LIGHT0_POWER] = glGetUniformLocation(programID, "lights[0].power");
	parameters[U_LIGHT0_KC] = glGetUniformLocation(programID, "lights[0].kC");
	parameters[U_LIGHT0_KL] = glGetUniformLocation(programID, "lights[0].kL");
	parameters[U_LIGHT0_KQ] = glGetUniformLocation(programID, "lights[0].kQ");
	parameters[U_LIGHT0_SPOTDIRECTION] = glGetUniformLocation(programID, "lights[0].spotDirection");
	parameters[U_LIGHT0_COSCUTOFF] = glGetUniformLocation(programID, "lights[0].cosCutoff");
	parameters[U_LIGHT0_COSINNER] = glGetUniformLocation(programID, "lights[0].cosInner");
	parameters[U_LIGHT0_EXPONENT] = glGetUniformLocation(programID, "lights[0].exponent");

	parameters[U_LIGHT1_TYPE] = glGetUniformLocation(programID, "lights[1].type");
	parameters[U_LIGHT1_POSITION] = glGetUniformLocation(programID, "lights[1].position_cameraspace");
	parameters[U_LIGHT1_COLOR] = glGetUniformLocation(programID, "lights[1].color");
	parameters[U_LIGHT1_POWER] = glGetUniformLocation(programID, "lights[1].power");
	parameters[U_LIGHT1_KC] = glGetUniformLocation(programID, "lights[1].kC");
	parameters[U_LIGHT1_KL] = glGetUniformLocation(programID, "lights[1].kL");
	parameters[U_LIGHT1_KQ] = glGetUniformLocation(programID, "lights[1].kQ");
	parameters[U_LIGHT1_SPOTDIRECTION] = glGetUniformLocation(programID, "lights[1].spotDirection");
	parameters[U_LIGHT1_COSCUTOFF] = glGetUniformLocation(programID, "lights[1].cosCutoff");
	parameters[U_LIGHT1_COSINNER] = glGetUniformLocation(programID, "lights[1].cosInner");
	parameters[U_LIGHT1_EXPONENT] = glGetUniformLocation(programID, "lights[1].exponent");
	// Get a handle for our "colorTexture" uniform
	parameters[U_COLOR_TEXTURE_ENABLED] = glGetUniformLocation(programID, "colorTextureEnabled");
	parameters[U_COLOR_TEXTURE] = glGetUniformLocation(programID, "colorTexture");
	// Get a handle for our "textColor" uniform
	parameters[U_TEXT_ENABLED] = glGetUniformLocation(programID, "textEnabled");
	parameters[U_TEXT_COLOR] = glGetUniformLocation(programID, "textColor");

	// Use our shader
	glUseProgram(programID);

	lights[0].type = Light::LIGHT_DIRECTIONAL;
	lights[0].position.Set(0, 0, 1);
	lights[0].color.Set(1, 1, 1);
	lights[0].power = brightness;
	lights[0].kC = 1.f;
	lights[0].kL = 0.001f;
	lights[0].kQ = 0.0001f;
	lights[0].cosCutoff = cos(Math::DegreeToRadian(45));
	lights[0].cosInner = cos(Math::DegreeToRadian(30));
	lights[0].exponent = 3.f;
	lights[0].spotDirection.Set(0.f, 0.f, 1.f);

	lights[1].type = Light::LIGHT_POINT;
	lights[1].position.Set(0, 0, 10);
	lights[1].color.Set(1, 1, 1);
	lights[1].power = 5;
	lights[1].kC = 1.f;
	lights[1].kL = 0.001f;
	lights[1].kQ = 0.0001f;
	lights[1].cosCutoff = cos(Math::DegreeToRadian(45));
	lights[1].cosInner = cos(Math::DegreeToRadian(30));
	lights[1].exponent = 3.f;
	lights[1].spotDirection.Set(0.f, 0.f, 1.f);

	glUniform1i(parameters[U_NUMLIGHTS], 2);
	glUniform1i(parameters[U_TEXT_ENABLED], 0);

	glUniform1i(parameters[U_LIGHT0_TYPE], lights[0].type);
	glUniform3fv(parameters[U_LIGHT0_COLOR], 1, &lights[0].color.r);
	glUniform1f(parameters[U_LIGHT0_POWER], lights[0].power);
	glUniform1f(parameters[U_LIGHT0_KC], lights[0].kC);
	glUniform1f(parameters[U_LIGHT0_KL], lights[0].kL);
	glUniform1f(parameters[U_LIGHT0_KQ], lights[0].kQ);
	glUniform1f(parameters[U_LIGHT0_COSCUTOFF], lights[0].cosCutoff);
	glUniform1f(parameters[U_LIGHT0_COSINNER], lights[0].cosInner);
	glUniform1f(parameters[U_LIGHT0_EXPONENT], lights[0].exponent);

	glUniform1i(parameters[U_LIGHT1_TYPE], lights[1].type);
	glUniform3fv(parameters[U_LIGHT1_COLOR], 1, &lights[1].color.r);
	glUniform1f(parameters[U_LIGHT1_POWER], lights[1].power);
	glUniform1f(parameters[U_LIGHT1_KC], lights[1].kC);
	glUniform1f(parameters[U_LIGHT1_KL], lights[1].kL);
	glUniform1f(parameters[U_LIGHT1_KQ], lights[1].kQ);
	glUniform1f(parameters[U_LIGHT1_COSCUTOFF], lights[1].cosCutoff);
	glUniform1f(parameters[U_LIGHT1_COSINNER], lights[1].cosInner);
	glUniform1f(parameters[U_LIGHT1_EXPONENT], lights[1].exponent);
}

void SceneManager_Options::RenderLight(const float rotation, const float x, const float y, const float z)
{
	//Lights
	if (lights[0].type == Light::LIGHT_DIRECTIONAL)
	{
		Mtx44 rot;
		rot.SetToRotation(rotation, x, y, z);
		Vector3 lightDir(lights[0].position.x, lights[0].position.y, lights[0].position.z);
		Vector3 lightDirection_cameraspace = viewStack.Top() * rot * lightDir;
		glUniform3fv(parameters[U_LIGHT0_POSITION], 1, &lightDirection_cameraspace.x);
	}
	else if (lights[0].type == Light::LIGHT_SPOT)
	{
		Mtx44 rot;
		rot.SetToRotation(rotation, x, y, z);
		Position lightPosition_cameraspace = viewStack.Top() * lights[0].position;
		glUniform3fv(parameters[U_LIGHT0_POSITION], 1, &lightPosition_cameraspace.x);
		Vector3 spotDirection_cameraspace = viewStack.Top() * rot * lights[0].spotDirection;
		glUniform3fv(parameters[U_LIGHT0_SPOTDIRECTION], 1, &spotDirection_cameraspace.x);
	}
	else if (lights[0].type == Light::LIGHT_POINT)
	{
		Position lightPosition_cameraspace = viewStack.Top() * lights[0].position;
		glUniform3fv(parameters[U_LIGHT0_POSITION], 1, &lightPosition_cameraspace.x);
		Vector3 spotDirection_cameraspace = viewStack.Top() * lights[0].spotDirection;
		glUniform3fv(parameters[U_LIGHT0_SPOTDIRECTION], 1, &spotDirection_cameraspace.x);
	}
	else
	{
		Position lightPosition_cameraspace = viewStack.Top() * lights[0].position;
		glUniform3fv(parameters[U_LIGHT0_POSITION], 1, &lightPosition_cameraspace.x);
	}

	//Lights
	if (lights[1].type == Light::LIGHT_DIRECTIONAL)
	{
		Mtx44 rot;
		rot.SetToRotation(rotation, x, y, z);
		Vector3 lightDir(lights[1].position.x, lights[1].position.y, lights[1].position.z);
		Vector3 lightDirection_cameraspace = viewStack.Top() * rot * lightDir;
		glUniform3fv(parameters[U_LIGHT1_POSITION], 1, &lightDirection_cameraspace.x);
	}
	else if (lights[1].type == Light::LIGHT_SPOT)
	{
		Mtx44 rot;
		rot.SetToRotation(rotation, x, y, z);
		Position lightPosition_cameraspace = viewStack.Top() * lights[1].position;
		glUniform3fv(parameters[U_LIGHT1_POSITION], 1, &lightPosition_cameraspace.x);
		Vector3 spotDirection_cameraspace = viewStack.Top() * rot * lights[1].spotDirection;
		glUniform3fv(parameters[U_LIGHT1_SPOTDIRECTION], 1, &spotDirection_cameraspace.x);
	}
	else if (lights[1].type == Light::LIGHT_POINT)
	{
		Position lightPosition_cameraspace = viewStack.Top() * lights[1].position;
		glUniform3fv(parameters[U_LIGHT1_POSITION], 1, &lightPosition_cameraspace.x);
		Vector3 spotDirection_cameraspace = viewStack.Top() * lights[1].spotDirection;
		glUniform3fv(parameters[U_LIGHT1_SPOTDIRECTION], 1, &spotDirection_cameraspace.x);
	}
	else
	{
		Position lightPosition_cameraspace = viewStack.Top() * lights[1].position;
		glUniform3fv(parameters[U_LIGHT1_POSITION], 1, &lightPosition_cameraspace.x);
	}
}

void SceneManager_Options::RenderBG()
{
	Mesh* drawMesh;
	drawMesh = resourceManager.retrieveMesh("Background");
	drawMesh->textureID = resourceManager.retrieveTexture("OPTION_BG");
	Render2DMesh(drawMesh, true, Vector2(1920, 1080), Vector2(sceneWidth * 0.5f, sceneHeight * 0.5f));

	//Options header
	drawMesh = resourceManager.retrieveMesh("FONT");
	RenderTextOnScreen(drawMesh, "Options", Color(1, 1, 1), 200.f, 200, 800, 0.f);

	//Brightness
	drawMesh = resourceManager.retrieveMesh("FONT");
	ostringstream ss_brightness;
	ss_brightness << std::setprecision(2) << brightness;
	RenderTextOnScreen(drawMesh, "Brightness:", Color(1, 1, 1), 100.f, 200, 700, 0.f);
	RenderTextOnScreen(drawMesh, ss_brightness.str(), Color(1, 1, 1), 100.f, 325, 600, 0.f);

	//Sound
	drawMesh = resourceManager.retrieveMesh("FONT");
	RenderTextOnScreen(drawMesh, "Volume:", Color(1, 1, 1), 100.f, 200, 500, 0.f);
	RenderTextOnScreen(drawMesh, std::to_string((int)(resourceManager.getEngineVolume() * 100 + 0.5f)), Color(1, 1, 1), 100.f, 325, 400, 0.f);

	//Sensitivity
	drawMesh = resourceManager.retrieveMesh("FONT");
	RenderTextOnScreen(drawMesh, "Vertical Sensitivity", Color(1, 1, 1), 100.f, 800, 700, 0.f);
	RenderTextOnScreen(drawMesh, std::to_string(tpCamera.GetVSense()), Color(1, 1, 1), 100.f, 925, 600, 0.f);

	drawMesh = resourceManager.retrieveMesh("FONT");
	RenderTextOnScreen(drawMesh, "Horizontal Sensitivity", Color(1, 1, 1), 100.f, 800, 500, 0.f);
	RenderTextOnScreen(drawMesh, std::to_string(tpCamera.GetHSense()), Color(1, 1, 1), 100.f, 925, 400, 0.f);
}

void SceneManager_Options::RenderStaticObject()
{
}

void SceneManager_Options::RenderMobileObject()
{
}

void SceneManager_Options::RenderSelection()
{
	SceneManagerSelection::RenderSelection();

	// Render mouse
	Render2DMesh(mouseMesh, false, Vector2(25, 50), Vector2(mousePos.x, mousePos.y - 5.f), 0.f);
}

void SceneManager_Options::UpdateMouse()
{
	SceneManagerSelection::UpateMouse();
}

void SceneManager_Options::UpdateSelection()
{
	SceneManagerSelection::UpdateSelection();

	for (unsigned i = 0; i < (unsigned)interactiveButtons.size(); ++i)
	{
		if (interactiveButtons[i].getStatus() != interactiveButtons[i].getPrevStatus())
		{
			if (interactiveButtons[i].getStatus() == Button2D::BUTTON_PRESSED)
			{
				interactiveButtons[i].setColor(resourceManager.retrieveColor("Red"));
				resourceManager.retrieveSoundas2D("Button_Press");

				//Brightness
				if (interactiveButtons[i].getName() == "Brightness_Up")
				{
					if (brightness < 5)
						brightness += 0.5f;
				}
				if (interactiveButtons[i].getName() == "Brightness_Down")
				{
					if (brightness > 0)
						brightness -= 0.5f;
				}

				//Volume
				if (interactiveButtons[i].getName() == "Volume_Up")
				{
					resourceManager.IncreaseSoundEngineVolume();
				}
				if (interactiveButtons[i].getName() == "Volume_Down")
				{
					resourceManager.DecreaseSoundEngineVolume();
				}

				//Vertical sensitivity
				if (interactiveButtons[i].getName() == "VSense_Up")
				{
					if (tpCamera.GetVSense() < 0.09f)
						tpCamera.AddVSense();
				}
				if (interactiveButtons[i].getName() == "VSense_Down")
				{
					if (tpCamera.GetVSense() > 0.f)
						tpCamera.MinusVSense();
				}

				//Horizontal sensitivity
				if (interactiveButtons[i].getName() == "HSense_Up")
				{
					if (tpCamera.GetHSense() < 0.09f)
						tpCamera.AddHSense();
				}
				if (interactiveButtons[i].getName() == "HSense_Down")
				{
					if (tpCamera.GetHSense() > 0.f)
						tpCamera.MinusHSense();
				}
			}

			else if (interactiveButtons[i].getStatus() == Button2D::BUTTON_IDLE)
			{
				interactiveButtons[i].setColor(resourceManager.retrieveColor("White"));
				interactiveButtons[i].setRotation(0.f);
			}

			else if (interactiveButtons[i].getStatus() == Button2D::BUTTON_HOVER)
			{
				if (interactiveButtons[i].getName() == "Back")
				{
					interactiveButtons[i].setRotation(-10.f);
					UpdateOptions();
				}
				interactiveButtons[i].setColor(resourceManager.retrieveColor("Red"));
				resourceManager.retrieveSoundas2D("Button_Hover");
			}
		}
	}
}