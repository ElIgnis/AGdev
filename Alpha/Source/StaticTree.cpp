#include "StaticTree.h"

CStaticTree::CStaticTree()
{
}

CStaticTree::~CStaticTree()
{
}

void CStaticTree::Init(Vector3 currentPos, Vector3 currentDir, Mesh* newMesh)
{
	StaticTreeNode = new SceneNode();
	GO3D_Object = new GameObject3D();
	StaticTreeNode->SetGameObject(GO3D_Object);
	
	StaticTreeNode->setActive(true);
	StaticTreeNode->GetGameObject()->setPosition(currentPos);
	StaticTreeNode->GetGameObject()->setMesh(newMesh);
	StaticTreeNode->GetGameObject()->setName("StaticTree");
	StaticTreeNode->GetGameObject()->setUpdate(true);
	StaticTreeNode->GetGameObject()->setRender(true);
	StaticTreeNode->GetGameObject()->setCollidable(true);

	this->moveSpeed = 100;
}

//Dir
void CStaticTree::SetDirection(Vector3 newDirection)
{
	this->direction = newDirection;
}
void CStaticTree::SetDirection_X(float newDir_X)
{
	this->direction.x = newDir_X;
}
void CStaticTree::SetDirection_Y(float newDir_Y)
{
	this->direction.y = newDir_Y;
}
void CStaticTree::SetDirection_Z(float newDir_Z)
{
	this->direction.z = newDir_Z;
}
Vector3 CStaticTree::GetDirection(void)
{
	return direction;
}

//Angle
float CStaticTree::GetAngle(void)
{
	return angle;
}
void CStaticTree::SetAngle(float newAngle)
{
	this->angle = newAngle;
}

//Physics
void CStaticTree::SetGravity(float newGravity)
{
	this->gravity = newGravity;
}
float CStaticTree::GetGravity(void)
{
	return gravity;
}
void CStaticTree::SetJumpSpeed(float newJSpeed)
{
	this->jumpSpeed = newJSpeed;
}
float CStaticTree::GetJumpSpeed(void)
{
	return jumpSpeed;
}
void CStaticTree::SetMoveSpeed(float newMSpeed)
{
	this->moveSpeed = newMSpeed;
}
float CStaticTree::GetMoveSpeed(void)
{
	return moveSpeed;
}

void CStaticTree::SetMoveSpeedMult(float newMSpeedMult)
{
	this->moveSpeed_Mult = newMSpeedMult;
}
float CStaticTree::GetMoveSpeedMult(void)
{
	return moveSpeed_Mult;
}

bool CStaticTree::GetIsMoving(void)
{
	return moving;
}

void CStaticTree::SetIsMoving(bool moving)
{
	this->moving = moving;
}
bool CStaticTree::GetInAir(void)
{
	return inAir;
}

SceneNode* CStaticTree::GetNode(void)
{
	return StaticTreeNode;
}

void CStaticTree::RotateLimb(string nodeName, float angle, float rotateSpeed, bool playOnce, double dt, float axisX, float axisY, float axisZ)
{
}

void CStaticTree::RevertLimb(bool aimMode, double dt)
{
}

//Update
void CStaticTree::Update(double dt, float CamAngle)
{

}

void CStaticTree::Update(double dt, Vector3 tgtPos, vector<CStaticTree*>StaticTreeList)
{
}

void CStaticTree::UpdateMovement(bool aimMode, double dt)
{
}

void CStaticTree::UpdateIdle(double dt)
{
}

void CStaticTree::UpdatePatrol(double dt)
{
}

void CStaticTree::UpdateAttack(double dt, Vector3 tgtPos)
{
}

void CStaticTree::UpdateRun(double dt, Vector3 tgtPos)
{   
}

void CStaticTree::UpdateAngle(float dt)
{
}

//Game related
void CStaticTree::SetHealth(int newHealth)
{
	this->health = newHealth;
}
void CStaticTree::SetLives(int newLives)
{
	this->lives = newLives;
}
int CStaticTree::GetHealth(void)
{
	return health;
}
int CStaticTree::GetLives(void)
{
	return lives;
}