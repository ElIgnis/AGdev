#ifndef SCENE_MANAGER_PLAY_L1_H
#define SCENE_MANAGER_PLAY_L1_H

#include "SceneManagerGameplay.h"
#include "..//Lua/LuaFileLoader.h"
#include "..\Player.h"
#include "..\Enemy_Beholder.h"
#include "..\StaticTree.h"

#define Enemy_BeholderSpawnInterval 10.f
#define AmmoSpawnInterval 30.f

class SceneManager_Play_L1 : public SceneManagerGameplay
{
private:
	vector<SceneNode*> nodeList;
	SpatialPartitionManager* spatialPartitionManager;
	static SceneNode* sceneGraph;
	static SceneNode* staticSceneGraph;
	static SceneNode* dynamicSceneGraph;
	// MiniMap* miniMap;

	GameObject3D playerbullet;
	GameObject3D theLaser;

	Mesh* textMesh;
public:
	SceneManager_Play_L1();
	~SceneManager_Play_L1();

	void Init(const int width, const int height, ResourcePool* RP, InputManager* controls);
	void InitLuaFunc(void);
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
	void InitDynamicNodes();
	void InitPlayer();
	void RespawnEnemy(void);
	void DeleteEnemy(SceneNode* node);
	void UpdateMouse();

	//Functions for lua
	static int SpawnEnemy_Beholder(lua_State *L);
	static int SpawnStaticTrees(lua_State *L);

	void CollisionResponse(SceneNode* first, SceneNode* second, double dt);
	bool CheckSelfCollide(SceneNode* first, SceneNode* second);
	//bool ProcessCollision(SceneNode* first, SceneNode* second);

	void RenderLight(const float rotation, const float x, const float y, const float z);;
	void RenderBG();
	void RenderStaticObject();
	void RenderMobileObject();
	void RenderGUI();

	void LoadHighScore(void);
	void CompareHighScore(void);
	void WriteHighScore(void);

	SceneNode* getNode(void);
	CPlayer* player;
	static vector<CEnemy_Beholder*> enemyList_Enemy_Beholder;
	static vector<CStaticTree*> treeList_StaticTree;

	int score;
	int spawnNumber;
	float ammoTimer;
	float spawnTimer;
	int nearbyEnemies;

	bool newRecord;
	int ScoreList[10];
	string data;
};

#endif