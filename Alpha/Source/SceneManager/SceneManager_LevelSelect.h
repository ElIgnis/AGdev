#ifndef SCENE_MANAGER_LEVELSELECT_H
#define SCENE_MANAGER_LEVELSELECT_H

#include "SceneManagerSelection.h"
#include "..//Lua/LuaFileLoader.h"

class SceneManager_LevelSelect : public SceneManagerSelection
{
public:
	SceneManager_LevelSelect();
	~SceneManager_LevelSelect();

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

	void RenderLight(const float rotation, const float x, const float y, const float z);;
	void RenderBG();
	void RenderStaticObject();
	void RenderMobileObject();
};

#endif