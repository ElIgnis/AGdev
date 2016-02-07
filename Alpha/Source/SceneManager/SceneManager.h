#ifndef SCENE_MANAGER_H
#define SCENE_MANAGER_H

#include <sstream>

#include "..\Scene.h"
#include "GL\glew.h"
#include "..\shader.hpp"
#include "Mtx44.h"
#include "..\FPCamera.h"
#include "..\TPCamera.h"
#include "..\MeshBuilder.h"
#include "MatrixStack.h"
#include "..\Light.h"
#include "..\Fog.h"
#include "..\Button2D.h"
#include "..\Button3D.h"
#include "..\SceneNode.h"
#include "..\SpatialPartitionManager.h"
#include "..\Utility.h"

using std::ifstream;
using std::ofstream;
using std::stoi;

class SceneManager : public Scene
{
public:
	enum UNIFORM_TYPE
	{
		U_MVP = 0,
		U_MODELVIEW,
		U_MODELVIEW_INVERSE_TRANSPOSE,
		U_MATERIAL_AMBIENT,
		U_MATERIAL_DIFFUSE,
		U_MATERIAL_SPECULAR,
		U_MATERIAL_SHININESS,
		U_LIGHTENABLED,
		U_NUMLIGHTS,

		U_LIGHT0_TYPE,
		U_LIGHT0_POSITION,
		U_LIGHT0_COLOR,
		U_LIGHT0_POWER,
		U_LIGHT0_KC,
		U_LIGHT0_KL,
		U_LIGHT0_KQ,
		U_LIGHT0_SPOTDIRECTION,
		U_LIGHT0_COSCUTOFF,
		U_LIGHT0_COSINNER,
		U_LIGHT0_EXPONENT,

		U_LIGHT1_TYPE,
		U_LIGHT1_POSITION,
		U_LIGHT1_COLOR,
		U_LIGHT1_POWER,
		U_LIGHT1_KC,
		U_LIGHT1_KL,
		U_LIGHT1_KQ,
		U_LIGHT1_SPOTDIRECTION,
		U_LIGHT1_COSCUTOFF,
		U_LIGHT1_COSINNER,
		U_LIGHT1_EXPONENT,

		U_COLOR_TEXTURE_ENABLED,
		U_COLOR_TEXTURE,
		U_TEXT_ENABLED,
		U_TEXT_COLOR,
		U_TOTAL,
	};

	SceneManager();
	~SceneManager();

	virtual void Init(const int width, const int height, ResourcePool* RP, InputManager* controls);
	string InitOptionSettings(int numOfOption);
	int GetNumOptions(void);
	virtual void Config(string directory);
	virtual void Update(double dt);
	virtual void Render();
	virtual void ClearScreen();
	virtual void Exit();

	virtual void InitShader();
	virtual void BindShaders();
	virtual void UpdateMouse();
	virtual void RenderLight(const float rotation, const float x, const float y, const float z);

	void PreRender(bool enableLight);
	void RenderPush(Mtx44 properties);
	void RenderPop();
	void RenderText(Mesh* mesh, std::string text, Color color);
	void RenderTextOnScreen(Mesh* mesh, std::string text, Color color, float size, float x, float y, float rotation = 0.f);
	void RenderMesh(Mesh *mesh, bool enableLight);
	void Render2DMesh(Mesh *mesh, const bool enableLight, const Vector2 size, const Vector2 pos, const float rotation = 0.f);

	InputManager* inputManager;
protected:
	float sceneWidth;
	float sceneHeight;

	unsigned vertexArrayID;
	unsigned programID;
	vector<unsigned> parameters;

	FPCamera fpCamera;
	TPCamera tpCamera;

	MS modelStack;
	MS viewStack;
	MS projectionStack;

	vector<Light> lights;

	ResourceManager resourceManager;
	ProjectileManager projectileManager;

	bool lightEnabled;
	float fps;
	float fontSize;
	float specialFontSize;
	bool debugInfo;
	Vector2 mousePos;

	int textWidth[256];
	int TextCount;

	Branch sceneBranch;

	bool pulse;
	float pulseAmt;
	float brightness;
	float volume;
	Vector2 screenSize;
};

#endif