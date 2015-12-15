#include "SceneManager_Splash.h"

SceneManager_Splash::SceneManager_Splash()
{
}

SceneManager_Splash::~SceneManager_Splash()
{
}

void SceneManager_Splash::Init(const int width, const int height, ResourcePool *RM, InputManager* controls)
{
	SceneManagerTransition::Init(width, height, RM, controls);

	Config();

	this->InitShader();

	// Projection matrix : 45� Field of View, 4:3 ratio, display range : 0.1 unit <-> 1000 units
	Mtx44 perspective;
	perspective.SetToPerspective(45.0f, 4.0f / 3.0f, 0.1f, 10000.0f);
	//perspective.SetToOrtho(-80, 80, -60, 60, -1000, 1000);
	projectionStack.LoadMatrix(perspective);

	handPrintTexture = resourceManager.retrieveMesh("HANDPRINT");
	handPrintTimer = 1.5f;
	
	startTransition = false;
	delayTimer = 1.5f;
}

void SceneManager_Splash::Config()
{
	SceneManagerTransition::Config("Config\\GameStateConfig\\SplashConfig.txt");
}

void SceneManager_Splash::Update(double dt)
{
	SceneManagerTransition::Update(dt);

	UpdateMouse();
	UpdateTransition(dt);
}

void SceneManager_Splash::Render()
{
	SceneManagerTransition::Render();

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

	RenderLight();
	RenderTransition();
}

void SceneManager_Splash::Exit()
{
	SceneManagerTransition::Exit();
}

void SceneManager_Splash::BindShaders()
{
	SceneManagerTransition::BindShaders();
}

// Other specific init, update and render classes
void SceneManager_Splash::InitShader()
{
	SHADER thisShader = resourceManager.retrieveShader("Comg");
	programID = LoadShaders(thisShader.vertexShaderDirectory.c_str(), thisShader.fragmentShaderDirectory.c_str());

	this->BindShaders();
	parameters.resize(U_TOTAL);
	lights.resize(1);

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
	// Get a handle for our "colorTexture" uniform
	parameters[U_COLOR_TEXTURE_ENABLED] = glGetUniformLocation(programID, "colorTextureEnabled");
	parameters[U_COLOR_TEXTURE] = glGetUniformLocation(programID, "colorTexture");
	// Get a handle for our "textColor" uniform
	parameters[U_TEXT_ENABLED] = glGetUniformLocation(programID, "textEnabled");
	parameters[U_TEXT_COLOR] = glGetUniformLocation(programID, "textColor");

	// Use our shader
	glUseProgram(programID);

	lights[0].type = Light::LIGHT_DIRECTIONAL;
	lights[0].position.Set(0, 0, 10);
	lights[0].color.Set(1, 1, 1);
	lights[0].power = 1;
	lights[0].kC = 1.f;
	lights[0].kL = 0.01f;
	lights[0].kQ = 0.001f;
	lights[0].cosCutoff = cos(Math::DegreeToRadian(45));
	lights[0].cosInner = cos(Math::DegreeToRadian(30));
	lights[0].exponent = 3.f;
	lights[0].spotDirection.Set(0.f, 0.f, 1.f);

	glUniform1i(parameters[U_NUMLIGHTS], 1);
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
}

void SceneManager_Splash::RenderLight()
{

}

void SceneManager_Splash::RenderTransition()
{
	//BG
	Mesh* drawMesh;
	drawMesh = resourceManager.retrieveMesh("Background");
	drawMesh->textureID = resourceManager.retrieveTexture("SPLASH_BG");
	Render2DMesh(drawMesh, false, Vector2(1920, 1080), Vector2(sceneWidth * 0.5f, sceneHeight * 0.5f));

	//HandPrint
	if (handPrintTimer <= 0)
	{
		Render2DMesh(handPrintTexture, false, Vector2(sceneHeight * 0.75f, sceneHeight * 0.75f), Vector2(sceneWidth * 0.45f, sceneHeight * 0.5f));	
	}
	if (handPrintTimer <= 0)
	{
		drawMesh = resourceManager.retrieveMesh("FONT");
		drawMesh->textureID = resourceManager.retrieveTexture("FONT");
		RenderTextOnScreen(drawMesh, "Ignis Productions", Color(1, 1, 1), 200, sceneWidth * 0.25f, sceneHeight * 0.4f);
	}
}

void SceneManager_Splash::UpdateMouse()
{
	SceneManagerTransition::UpateMouse();
}

void SceneManager_Splash::UpdateTransition(double dt)
{
	SceneManagerTransition::UpdateTransition(dt);

	// update
	if (!complete)
	{
		if (handPrintTimer > 0)
		{
			handPrintTimer -= dt;
			resourceManager.retrieveSoundas2D("Splash_Opening", false, false);
		}

		else
		{
			startTransition = true;
		}

		if (startTransition)
		{
			delayTimer -= (float)dt;

			if (delayTimer <= 0)
				complete = true;
		}
	}

	else
	{
		++runCount;
	}
}