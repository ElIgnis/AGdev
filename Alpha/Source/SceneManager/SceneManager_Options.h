#ifndef SCENE_MANAGER_OPTIONS_H
#define SCENE_MANAGER_OPTIONS_H

#include "SceneManagerSelection.h"

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

	void RenderLight();
	void RenderBG();
	void RenderStaticObject();
	void RenderMobileObject();

private:
	SpriteAnimation* fireball;
};

#endif