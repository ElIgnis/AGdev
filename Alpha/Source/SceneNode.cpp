#include "SceneNode.h"
#include "MatrixStack.h"
#include "..\Source\SceneManager\SceneManager.h"
#include "Player.h"

SceneNode::SceneNode() : gameObject3D(NULL), parentNode(NULL), nodeType(STATIC_NODE), active(false)
{
}

SceneNode::~SceneNode()
{
}

void SceneNode::SetGameObject(GameObject3D *gameObject)
{
	this->gameObject3D = gameObject;
}

void SceneNode::SetParentNode(SceneNode* _parentNode)
{
	this->parentNode = _parentNode;
}

SceneNode* SceneNode::GetParentNode(void)
{
	return this->parentNode;
}

void SceneNode::setActive(bool active)
{
	this->active = active;

	this->gameObject3D->setRender(active);
	this->gameObject3D->setUpdate(active);
}

bool SceneNode::getActive(void)
{
	return active;
}

void SceneNode::AddChildNode(SceneNode *childNode)
{
	childNode->SetParentNode(this);

	this->childNodes.push_back(childNode);
}

void SceneNode::AddChildToChildNode(string childName, SceneNode *childNode)
{
	this->GetChildNode(childName)->AddChildNode(childNode);
}

void SceneNode::RemoveChildNode(SceneNode *childNode)
{
	// do a recursive down to remove child
	if (childNode->HasChild())
	{
		for (vector<SceneNode*>::iterator i = childNode->childNodes.begin(); i != childNode->childNodes.end();)
		{
			SceneNode* temp = (*i);
			RemoveChildNode(temp);
			i = childNode->childNodes.begin();
		}
	}

	childNode->setActive(false);

	// there is a parent
	if (childNode->parentNode != NULL)
	{
		vector<SceneNode*> tempParentChild = childNode->parentNode->childNodes;

		for (unsigned i = 0; i < tempParentChild.size(); ++i)
		{
			if (tempParentChild[i] == childNode)
			{
				tempParentChild.erase(tempParentChild.begin() + i);
				break;
			}
		}

		childNode->parentNode->childNodes = tempParentChild;
	}
}

void SceneNode::RemoveChildFromChild(string childName, SceneNode *childNode)
{

}

void SceneNode::Draw(SceneManager *sceneManager, Mesh* debugMesh)
{
	sceneManager->PreRender(true);

	if (gameObject3D != NULL)
	{
		sceneManager->RenderPush(gameObject3D->getProperties().modelProperties);

#if _DEBUG
		if (debugMesh != NULL)
		{
			Properties trs;
			Mtx44 overall;
			overall.SetToIdentity();
			trs.scale.SetToScale(Vector3(gameObject3D->getHitbox().getLength(), gameObject3D->getHitbox().getHeight(), gameObject3D->getHitbox().getDepth()));
			trs.modelProperties = overall * trs.translation * trs.rotation * trs.scale;
			sceneManager->RenderPush(trs.modelProperties);
			sceneManager->RenderMesh(debugMesh, false);
			sceneManager->RenderPop();
		}
#endif
		sceneManager->RenderMesh(gameObject3D->getMesh(), gameObject3D->getReflectLight());
	}

	for (unsigned i = 0; i < childNodes.size(); ++i)
	{
		this->childNodes[i]->DrawChild(sceneManager, debugMesh);
	}

	if (gameObject3D != NULL)
	{
		sceneManager->RenderPop();
	}
}

void SceneNode::DrawChild(SceneManager *sceneManager, Mesh* debugMesh)
{
	sceneManager->RenderPush(gameObject3D->getProperties().modelProperties);

#if _DEBUG
	if (debugMesh != NULL)
	{
		Properties trs;
		Mtx44 overall;
		overall.SetToIdentity();
		trs.scale.SetToScale(Vector3(gameObject3D->getHitbox().getLength(), gameObject3D->getHitbox().getHeight(), gameObject3D->getHitbox().getDepth()));
		trs.modelProperties = overall * trs.translation * trs.rotation * trs.scale;
		sceneManager->RenderPush(trs.modelProperties);
		sceneManager->RenderMesh(debugMesh, false);
		sceneManager->RenderPop();
	}
#endif
	//Only draw active Enemy_Beholder object
	if (this->GetGameObject()->getName() == "Enemy_Beholder")
	{
		if (this->active)
			sceneManager->RenderMesh(gameObject3D->getMesh(), true);
	}
	else
	{
		sceneManager->RenderMesh(gameObject3D->getMesh(), true);
	}

	for (unsigned i = 0; i < childNodes.size(); ++i)
	{
		this->childNodes[i]->DrawChild(sceneManager, debugMesh);
	}

	sceneManager->RenderPop();
}

void SceneNode::SetWorldPosition(Vector3 pos)
{
	this->WorldPosition = pos;
}

bool SceneNode::ProcessCollision(SceneNode* dst)
{
	bool checkCollide = false;
	string boxName = "";

	checkCollide = check3DCollision(this->GetGameObject()->getHitbox(), dst->GetGameObject()->getHitbox(), boxName);
	
	//if (this->GetGameObject()->getName() == "Player")
	//{
	//	Vector3 checkWithVel = this->GetGameObject()->getHitbox().getMidPoint() + Player->GetVelocity();
	//	
	//}
	//else if (dst->GetGameObject()->getName() == "Player")
	//{
	//	checkCollide = check3DCollision(this->GetGameObject()->getHitbox(), dst->GetGameObject()->getHitbox(), boxName);
	//}
	
	return checkCollide;
}

Vector3 SceneNode::GetWorldPosition(void)
{
	return this->WorldPosition;
}

SceneNode* SceneNode::GetChildNode(string childName)
{
	static SceneNode *resultNode = NULL;

	for (unsigned i = 0; i < childNodes.size(); ++i)
	{
		if (childNodes[i]->GetGameObject()->getName() == childName)
		{
			return childNodes[i];
		}

		else if (childNodes[i]->childNodes.size() > 0)
		{
			resultNode = childNodes[i]->GetChildNode(childName);

			if (resultNode != NULL)
			{
				return resultNode;
			}
		}
	}

	return NULL;
}

GameObject3D* SceneNode::GetGameObject()
{
	return this->gameObject3D;
}

void SceneNode::CleanUp()
{
	for (unsigned i = 0; i < childNodes.size(); ++i)
	{
		childNodes[i]->CleanUp();
	}

	if (this->gameObject3D != NULL)
	{
		delete gameObject3D;
		gameObject3D = NULL;
	}
}

bool SceneNode::HasChild(void)
{
	if (this->childNodes.size() > 0)
		return true;

	return false;
}

void SceneNode::SetPartitionIndex(Vector3 partitionIndex)
{
	this->currentPartition = partitionIndex;
}
Vector3 SceneNode::GetPartitionIndex(void)
{
	return currentPartition;
}