#ifndef SCENE_MANAGER_OPTIONS_H
#define SCENE_MANAGER_OPTIONS_H

#include "SceneManagerSelection.h"
#include "..//Lua/LuaFileLoader.h"
#include <iomanip>

class SceneManager_Options : public SceneManagerSelection
{
public:
	SceneManager_Options();
	~SceneManager_Options();

	void Init(const int width, const int height, ResourcePool* RP, InputManager* controls);
	void Config();
	void Update(double dt);
	void Render();
	void Exit();

	void BindShaders();
	void InitShader();

	void UpdateMouse();
	void UpdateSelection();
	void RenderSelection();

	void RenderLight(const float rotation, const float x, const float y, const float z);
	void RenderBG();
	void RenderStaticObject();
	void RenderMobileObject();

	void UpdateOptions(void);

protected:
	int screenWidth;
	int screenHeight;

};

#endif