#ifndef SCENE_MANAGER_PLAY_L2_H
#define SCENE_MANAGER_PLAY_L2_H

#include "SceneManagerGameplay.h"
#include "..\Player.h"
#include "..\Death.h"

#define DeathSpawnInterval 10.f
#define AmmoSpawnInterval 30.f

class SceneManager_Play_L2 : public SceneManagerGameplay
{
private:
	vector<SceneNode*> nodeList;
	SpatialPartitionManager* spatialPartitionManager;
	SceneNode* sceneGraph;
	SceneNode* staticSceneGraph;
	SceneNode* dynamicSceneGraph;
	// MiniMap* miniMap;

	GameObject3D playerbullet;
	GameObject3D theLaser;

	Mesh* textMesh;
public:
	SceneManager_Play_L2();
	~SceneManager_Play_L2();

	void Init(const int width, const int height, ResourcePool* RP, InputManager* controls);
	void Config();
	void Config(string directory);
	void Update(double dt);
	void UpdateSP(double dt);
	void UpdatePlayer(double dt);
	void UpdateEnemy(double dt);
	void Render();
	void Exit();

	void InitShader();
	void BindShaders();

	void InitSceneGraph();
	void InitStaticNodes();
	void InitEnvironmentNodes();
	void InitDynamicNodes();
	void InitDeath();
	void InitPlayer();
	void SpawnEnemy(void);
	void DeleteEnemy(SceneNode* node);
	void UpdateMouse();

	void CollisionResponse(SceneNode* first, SceneNode* second, double dt);
	bool CheckSelfCollide(SceneNode* first, SceneNode* second);
	//bool ProcessCollision(SceneNode* first, SceneNode* second);
	bool GetGameOver();
	void RenderLight(const float rotation, const float x, const float y, const float z);;
	void RenderBG();
	void RenderStaticObject();
	void RenderMobileObject();
	void RenderGUI();

	void LoadHighScore(void);
	void CompareHighScore(void);

	SceneNode* getNode(void);
	CPlayer* player;
	vector<CDeath*> enemyList_Death;
	//Score as time
	float timer_sec;
	float timer_min;

	int spawnNumber;
	float ammoTimer;
	float spawnTimer;
	bool GameOver;
};

#endif