#ifndef SCENE_MANAGER_GAMEOVER_H
#define SCENE_MANAGER_GAMEOVER_H

#include "SceneManagerSelection.h"

class SceneManager_GameOver : public SceneManagerSelection
{
public:
	SceneManager_GameOver();
	~SceneManager_GameOver();

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