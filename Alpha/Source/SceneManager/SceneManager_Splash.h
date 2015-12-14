#ifndef SCENE_MANAGER_SPLASH_H
#define SCENE_MANAGER_SPLASH_H

#include "SceneManagerTransition.h"

class SceneManager_Splash : public SceneManagerTransition
{
private:
	Mesh* splashTexture;
	Vector2 textureScale;
	float pauseTimer;
	bool startTransition;
	float splashTimer;
	
public:
	SceneManager_Splash();
	~SceneManager_Splash();

	void Init(const int width, const int height, ResourcePool* RP, InputManager* controls);
	void Config();
	void Update(double dt);
	void Render();
	void Exit();

	void InitShader();
	void BindShaders();

	void UpdateMouse();
	void UpdateTransition(double dt);

	void RenderLight();
	void RenderTransition();
};

#endif