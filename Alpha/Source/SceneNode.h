#ifndef SCENE_NODE_H
#define SCENE_NODE_H

#include "GameObject3D.h"

class SceneManager;

class SceneNode
{
public:
	enum NODE_TYPE
	{
		STATIC_NODE,
		DYNAMIC_NODE,
		MAX_NODE
	};

	SceneNode();
	~SceneNode();
	
	void SetGameObject(GameObject3D *gameObject);
	void SetParentNode(SceneNode* _parentNode);

	void AddChildNode(SceneNode *childNode);
	void AddChildToChildNode(string childName, SceneNode *childNode);

	void Draw(SceneManager *sceneManager, Mesh* debugMesh = NULL);
	void DrawChild(SceneManager *sceneManager, Mesh* debugMesh);

	void RemoveChildNode(SceneNode *childNode);
	void RemoveChildFromChild(string childName, SceneNode *childNode);

	bool ProcessCollision(SceneNode* dst);

	void SetWorldPosition(Vector3 pos);
	Vector3 GetWorldPosition(void);

	SceneNode* GetParentNode(void);

	void setActive(bool active);
	bool getActive(void);
	void SetPartitionIndex(Vector3 partitionIndex);
	Vector3 GetPartitionIndex(void);

	void CleanUp();

	bool HasChild(void);

	SceneNode* GetChildNode(string childName);
	
	GameObject3D* GetGameObject();

	vector<SceneNode*> childNodes;
	SceneNode *parentNode;
	NODE_TYPE nodeType;
protected:
	GameObject3D* gameObject3D;
	Vector3 WorldPosition;
	Vector3 currentPartition;
	bool active;
};

#endif