#ifndef SCENE_MANAGER_HIGHSCORE_H
#define SCENE_MANAGER_HIGHSCORE_H

#include "SceneManagerSelection.h"

class SceneManager_HighScore : public SceneManagerSelection
{
public:
	SceneManager_HighScore();
	~SceneManager_HighScore();

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
};

#endif