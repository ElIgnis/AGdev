#ifndef SCENE_MANAGER_PLAY_H
#define SCENE_MANAGER_PLAY_H

#include "SceneManagerGameplay.h"
#include "..\Player.h"
#include "..\Joker.h"

class SceneManager_Play : public SceneManagerGameplay
{
private:
	vector<SceneNode*> nodeList;
	SpatialPartitionManager* spatialPartitionManager;
	SceneNode* sceneGraph;
	SceneNode* staticSceneGraph;
	SceneNode* dynamicSceneGraph;
	// MiniMap* miniMap;

	GameObject3D playerbullet;

	Mesh* textMesh;
public:
	SceneManager_Play();
	~SceneManager_Play();

	void Init(const int width, const int height, ResourcePool* RP, InputManager* controls);
	void Config();
	void Config(string directory);
	void Update(double dt);
	void UpdatePlayer(double dt);
	void Render();
	void Exit();

	void InitShader();
	void BindShaders();

	void InitSceneGraph();
	void InitStaticNodes();
	void InitEnvironmentNodes();
	void InitDynamicNodes();
	void InitPlayer();
	void InitAmmo();
	
	void SpawnEnemy();
	void RespawnEnemy();

	void UpdateMouse();

	bool CheckSelfCollide(SceneNode* first, SceneNode* second);
	//bool ProcessCollision(SceneNode* first, SceneNode* second);

	void RenderLight();
	void RenderBG();
	void RenderStaticObject();
	void RenderMobileObject();
	void RenderGUI();

	SceneNode* getNode(void);
	CPlayer* player;
	vector<CJoker*> enemyList_Joker;
};

#endif