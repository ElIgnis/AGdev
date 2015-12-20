#ifndef SCENE_MANAGER_SPLASH_H
#define SCENE_MANAGER_SPLASH_H

#include "SceneManagerTransition.h"

class SceneManager_Splash : public SceneManagerTransition
{
private:
	Mesh* handPrintTexture;
	float handPrintTimer;

	bool startTransition;
	float delayTimer;
	float scale = 0;
	float transX = 0;

	////Light debugging
	//float angle = 0;
	//float x = 0;
	//float y = 1;
	//float z = 0;
	//float a = 0;
	//float b = 0;
	//float c = 0;
	
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

	void RenderLight(const float rotation, const float x, const float y, const float z);
	void RenderTransition();
};

#endif