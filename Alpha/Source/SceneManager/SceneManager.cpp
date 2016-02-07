#include "SceneManager.h"
#include "GL\glew.h"
#include "..\shader.hpp"
#include "..\Application.h"

lua_State *Lua_Init;

SceneManager::SceneManager()
: pulseAmt(0.f)
, pulse(false)
{
	parameters.clear();
	lights.clear();
	fontSize = specialFontSize = 1.f;
	debugInfo = false;
}

SceneManager::~SceneManager()
{
}

void SceneManager::Init(const int width, const int height, ResourcePool* RM, InputManager* controls)
{
	//Monospacing
	TextCount = 0;
	std::string data = " ";

	//File reading
	ifstream inFile;
	inFile.open("Config//TextSpacing.txt");
	if (inFile.good())
	{
		while (getline(inFile, data))
		{
			textWidth[TextCount] = stoi(data);
			TextCount++;
		}
		inFile.close();
	}

	Lua_Init = lua_open();

	//Load the libs
	luaL_openlibs(Lua_Init);

	//Initialise engine with values from Lua file
	luaL_dofile(Lua_Init, "Lua/OptionsSettings.Lua");

	if (luaL_loadfile(Lua_Init, "Lua/OptionsSettings.Lua") || lua_pcall(Lua_Init, 0, 0, 0))
	{
		printf("error: %s", lua_tostring(Lua_Init, -1));
	}

	int NumOptions = GetNumOptions();

	for (int i = 1; i <= NumOptions; ++i)
	{
		string options_data = InitOptionSettings(i);

		switch (i)
		{
		case 1:
			brightness = stoi(options_data);
			break;
		case 2:
			tpCamera.SetVSense(stof(options_data));
			break;
		case 3:
			tpCamera.SetHSense(stof(options_data));
			break;
		default:
			break;
		}
	}

	//Close the lua file
	lua_close(Lua_Init);

	this->sceneWidth = (float)width;
	this->sceneHeight = (float)height;
	this->resourceManager.Init(RM);
	this->inputManager = controls;

	glClearColor(0.f, 0.f, 0.f, 1.0f);
	// Enable depth test
	glEnable(GL_DEPTH_TEST);
	// Accept fragment if it closer to the camera than the former one
	glDepthFunc(GL_LESS);

	glEnable(GL_CULL_FACE);

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	Math::InitRNG();
}

string SceneManager::InitOptionSettings(int numOfOption)
{
	lua_getglobal(Lua_Init, "readData");

	lua_pushstring(Lua_Init, "Config\\Options.txt");
	lua_pushnumber(Lua_Init, numOfOption);
	if (lua_pcall(Lua_Init, 2, 1, 0) != 0)
		printf("error running function `f': %s", lua_tostring(Lua_Init, -1));

	return (string)lua_tostring(Lua_Init, -1);
}

int SceneManager::GetNumOptions(void)
{
	lua_getglobal(Lua_Init, "readDataSize");

	lua_pushstring(Lua_Init, "Config\\Options.txt");
	if (lua_pcall(Lua_Init, 1, 1, 0) != 0)
		printf("error running function `f': %s", lua_tostring(Lua_Init, -1));

	return (int)lua_tonumber(Lua_Init, -1);
}

void SceneManager::Config(string directory)
{
}

void SceneManager::InitShader()
{
}

void SceneManager::BindShaders()
{
	glGenVertexArrays(1, &vertexArrayID);
	glBindVertexArray(vertexArrayID);
}

void SceneManager::UpdateMouse()
{
	// updating mouse pos in the scene via a handler from the engine
	double x, y;
	x = Application::getMouse()->getCurrentPosX();
	y = Application::getMouse()->getCurrentPosY();

	int w = Application::getWindowWidth();
	int h = Application::getWindowHeight();

	mousePos.Set((float)x * sceneWidth / w, (h - (float)y) * sceneHeight / h);
}

void SceneManager::RenderLight(const float rotation, const float x, const float y, const float z)
{
}

void SceneManager::Update(double dt)
{
	fps = (float)(1.f / dt);

	this->inputManager->Update();

	if (inputManager->getKey("ToggleWireFrame"))
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}

	else if (inputManager->getKey("ToggleFill"))
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}

	if (inputManager->getKey("EnableDebugInfo"))
	{
		this->debugInfo = true;
	}

	else if (inputManager->getKey("DisableDebugInfo"))
	{
		this->debugInfo = false;
	}

}

void SceneManager::ClearScreen()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void SceneManager::Render()
{
	Mesh* drawMesh;
	drawMesh = resourceManager.retrieveMesh("FONT");
	RenderTextOnScreen(drawMesh, "test", Color(), 100, 100, 100);
	
}

void SceneManager::PreRender(bool enableLight)
{
	Mtx44 MVP, modelView, modelView_inverse_transpose;

	MVP = projectionStack.Top() * viewStack.Top() * modelStack.Top();
	glUniformMatrix4fv(parameters[U_MVP], 1, GL_FALSE, &MVP.a[0]);
	if (enableLight)
	{
		glUniform1i(parameters[U_LIGHTENABLED], 1);
		modelView = viewStack.Top() * modelStack.Top();
		glUniformMatrix4fv(parameters[U_MODELVIEW], 1, GL_FALSE, &modelView.a[0]);
		modelView_inverse_transpose = modelView.GetInverse().GetTranspose();
		glUniformMatrix4fv(parameters[U_MODELVIEW_INVERSE_TRANSPOSE], 1, GL_FALSE, &modelView.a[0]);
	}
	else
	{
		glUniform1i(parameters[U_LIGHTENABLED], 0);
	}
}

void SceneManager::RenderPush(Mtx44 properties)
{
	modelStack.PushMatrix();
	modelStack.MultMatrix(properties);
}

void SceneManager::RenderPop()
{
	modelStack.PopMatrix();
}

/********************************************************************************
Render text onto the screen with reference position in the middle of the image
********************************************************************************/
void SceneManager::RenderText(Mesh* mesh, std::string text, Color color)
{
	if (!mesh)
	{
		std::cout << "Unable to render text!" << std::endl;
		return;
	}

	glDisable(GL_DEPTH_TEST);
	glUniform1i(parameters[U_TEXT_ENABLED], 1);
	glUniform3fv(parameters[U_TEXT_COLOR], 1, &color.r);
	glUniform1i(parameters[U_LIGHTENABLED], 0);
	glUniform1i(parameters[U_COLOR_TEXTURE_ENABLED], 1);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, mesh->textureID);
	glUniform1i(parameters[U_COLOR_TEXTURE], 0);
	float temp = 0;
	float widthDivide = 0.01f;
	for (unsigned i = 0; i < text.length(); ++i)
	{
		Mtx44 characterSpacing;
		characterSpacing.SetToTranslation(temp * 1.5f + 0.5f, 0.5f, 0); //1.0f is the spacing of each character, you may change this value
		Mtx44 MVP = projectionStack.Top() * viewStack.Top() * modelStack.Top() * characterSpacing;
		glUniformMatrix4fv(parameters[U_MVP], 1, GL_FALSE, &MVP.a[0]);
		temp += textWidth[text[i]] * widthDivide;
		mesh->Render((unsigned)text[i] * 6, 6);
	}
	glBindTexture(GL_TEXTURE_2D, 0);
	glUniform1i(parameters[U_TEXT_ENABLED], 0);
	glEnable(GL_DEPTH_TEST);

}

/********************************************************************************
Render text onto the screen
********************************************************************************/
void SceneManager::RenderTextOnScreen(Mesh* mesh, std::string text, Color color, float size, float x, float y, float rotation)
{
	if (!mesh)
	{
		std::cout << "Unable to render text on screen!" << std::endl;
		return;
	}

	glDisable(GL_DEPTH_TEST);
	Mtx44 ortho;
	ortho.SetToOrtho(0, double(sceneWidth), 0, double(sceneHeight), -10, 10);
	projectionStack.PushMatrix();
	projectionStack.LoadMatrix(ortho);
	viewStack.PushMatrix();
	viewStack.LoadIdentity();
	modelStack.PushMatrix();
	modelStack.LoadIdentity();
	modelStack.Translate(x, y, 0);
	if (rotation != 0.f)
	{
		modelStack.Rotate(rotation, 0, 0, 1);
	}
	modelStack.Scale(size, size, size);

	glUniform1i(parameters[U_TEXT_ENABLED], 1);
	glUniform3fv(parameters[U_TEXT_COLOR], 1, &color.r);
	glUniform1i(parameters[U_LIGHTENABLED], 0);
	glUniform1i(parameters[U_COLOR_TEXTURE_ENABLED], 1);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, mesh->textureID);
	glUniform1i(parameters[U_COLOR_TEXTURE], 0);
	float temp = 0;
	float widthDivide = 0.01f;
	for (unsigned i = 0; i < text.length(); ++i)
	{
		Mtx44 characterSpacing;
		characterSpacing.SetToTranslation(temp * 1.5f + 0.5f, 0.5f, 0); //1.0f is the spacing of each character, you may change this value
		Mtx44 MVP = projectionStack.Top() * viewStack.Top() * modelStack.Top() * characterSpacing;
		glUniformMatrix4fv(parameters[U_MVP], 1, GL_FALSE, &MVP.a[0]);
		temp += textWidth[text[i]] * widthDivide;
		mesh->Render((unsigned)text[i] * 6, 6);
	}
	glBindTexture(GL_TEXTURE_2D, 0);
	glUniform1i(parameters[U_TEXT_ENABLED], 0);
	modelStack.PopMatrix();
	viewStack.PopMatrix();
	projectionStack.PopMatrix();
	glEnable(GL_DEPTH_TEST);
}

/********************************************************************************
Render a mesh in 3D
********************************************************************************/
void SceneManager::RenderMesh(Mesh* mesh, bool enableLight)
{
	if (!mesh)
	{
		//std::cout << "Unable to render 3D mesh!" << std::endl;
		return;
	}

	Mtx44 MVP, modelView, modelView_inverse_transpose;

	MVP = projectionStack.Top() * viewStack.Top() * modelStack.Top();
	glUniformMatrix4fv(parameters[U_MVP], 1, GL_FALSE, &MVP.a[0]);

	modelView = viewStack.Top() * modelStack.Top();
	glUniformMatrix4fv(parameters[U_MODELVIEW], 1, GL_FALSE, &modelView.a[0]);

	if (enableLight)
	{
		glUniform1i(parameters[U_LIGHTENABLED], 1);
		modelView_inverse_transpose = modelView.GetInverse().GetTranspose();
		glUniformMatrix4fv(parameters[U_MODELVIEW_INVERSE_TRANSPOSE], 1, GL_FALSE, &modelView_inverse_transpose.a[0]);

		//load material
		glUniform3fv(parameters[U_MATERIAL_AMBIENT], 1, &mesh->material.kAmbient.r);
		glUniform3fv(parameters[U_MATERIAL_DIFFUSE], 1, &mesh->material.kDiffuse.r);
		glUniform3fv(parameters[U_MATERIAL_SPECULAR], 1, &mesh->material.kSpecular.r);
		glUniform1f(parameters[U_MATERIAL_SHININESS], mesh->material.kShininess);
	}

	else
	{
		glUniform1i(parameters[U_LIGHTENABLED], 0);
	}

	if (mesh->textureID != NULL)
	{
		glUniform1i(parameters[U_COLOR_TEXTURE_ENABLED], 1);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, mesh->textureID);
		glUniform1i(parameters[U_COLOR_TEXTURE], 0);
	}
	else
	{
		glUniform1i(parameters[U_COLOR_TEXTURE_ENABLED], 0);
	}
	mesh->Render();
}

/********************************************************************************
Render a mesh in 2D
********************************************************************************/
void SceneManager::Render2DMesh(Mesh *mesh, const bool enableLight, const Vector2 scale, const Vector2 pos, const float rotation)
{
	if (!mesh)
	{
		std::cout << "Unable to render 2D mesh!" << std::endl;
		return;
	}
	glDisable(GL_DEPTH_TEST);
	Mtx44 ortho;
	ortho.SetToOrtho(0, double(sceneWidth), 0, double(sceneHeight), -100, 100);
	projectionStack.PushMatrix();
	projectionStack.LoadMatrix(ortho);
	viewStack.PushMatrix();
	viewStack.LoadIdentity();
	modelStack.PushMatrix();
	modelStack.LoadIdentity();
	modelStack.Translate(pos.x, pos.y, 0);
	modelStack.Scale(scale.x, scale.y, 1);
	if (rotation != 0.f)
		modelStack.Rotate(rotation, 0, 0, 1);

	Mtx44 MVP, modelView, modelView_inverse_transpose;

	MVP = projectionStack.Top() * viewStack.Top() * modelStack.Top();
	glUniformMatrix4fv(parameters[U_MVP], 1, GL_FALSE, &MVP.a[0]);

	if (mesh->textureID != NULL)
	{
		glUniform1i(parameters[U_COLOR_TEXTURE_ENABLED], 1);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, mesh->textureID);
		glUniform1i(parameters[U_COLOR_TEXTURE], 0);
	}
	else
	{
		glUniform1i(parameters[U_COLOR_TEXTURE_ENABLED], 0);
	}
	//mesh->Render();
	RenderMesh(mesh, enableLight);
	if (mesh->textureID > 0)
	{
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	modelStack.PopMatrix();
	viewStack.PopMatrix();
	projectionStack.PopMatrix();
	glEnable(GL_DEPTH_TEST);
}

void SceneManager::Exit()
{
	projectileManager.CleanUp();

	glDeleteProgram(programID);
	glDeleteVertexArrays(1, &vertexArrayID);
}