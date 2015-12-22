#include "Death.h"

CDeath::CDeath()
:	Alive(true)
{
}

CDeath::~CDeath()
{
}

void CDeath::Init(Vector3 currentPos, Vector3 currentDir, Mesh* playerMesh)
{
	deathNode = new SceneNode();
	GO3D_Object = new GameObject3D();
	deathNode->SetGameObject(GO3D_Object);
	
	deathNode->setActive(true);
	deathNode->GetGameObject()->setPosition(currentPos);
	deathNode->GetGameObject()->setMesh(playerMesh);
	deathNode->GetGameObject()->setName("Death");
	deathNode->GetGameObject()->setUpdate(true);
	deathNode->GetGameObject()->setRender(true);
	deathNode->GetGameObject()->setCollidable(true);

	this->moveSpeed = 100;
}

//Dir
void CDeath::SetDirection(Vector3 newDirection)
{
	this->direction = newDirection;
}
void CDeath::SetDirection_X(float newDir_X)
{
	this->direction.x = newDir_X;
}
void CDeath::SetDirection_Y(float newDir_Y)
{
	this->direction.y = newDir_Y;
}
void CDeath::SetDirection_Z(float newDir_Z)
{
	this->direction.z = newDir_Z;
}
Vector3 CDeath::GetDirection(void)
{
	return direction;
}

//Angle
float CDeath::GetAngle(void)
{
	return angle;
}
void CDeath::SetAngle(float newAngle)
{
	this->angle = newAngle;
}

//Physics
void CDeath::SetGravity(float newGravity)
{
	this->gravity = newGravity;
}
float CDeath::GetGravity(void)
{
	return gravity;
}
void CDeath::SetJumpSpeed(float newJSpeed)
{
	this->jumpSpeed = newJSpeed;
}
float CDeath::GetJumpSpeed(void)
{
	return jumpSpeed;
}
void CDeath::SetMoveSpeed(float newMSpeed)
{
	this->moveSpeed = newMSpeed;
}
float CDeath::GetMoveSpeed(void)
{
	return moveSpeed;
}

void CDeath::SetMoveSpeedMult(float newMSpeedMult)
{
	this->moveSpeed_Mult = newMSpeedMult;
}
float CDeath::GetMoveSpeedMult(void)
{
	return moveSpeed_Mult;
}

bool CDeath::GetIsMoving(void)
{
	return moving;
}

void CDeath::SetIsMoving(bool moving)
{
	this->moving = moving;
}
bool CDeath::GetInAir(void)
{
	return inAir;
}

SceneNode* CDeath::GetNode(void)
{
	return deathNode;
}

void CDeath::RotateLimb(string nodeName, float angle, float rotateSpeed, bool playOnce, double dt, float axisX, float axisY, float axisZ)
{
}

void CDeath::RevertLimb(bool aimMode, double dt)
{
}

void CDeath::SetIsAlive(bool alive)
{
	this->Alive = alive;
}

bool CDeath::GetIsAlive(void)
{
	return Alive;
}

//Update
void CDeath::Update(double dt, float CamAngle)
{
}

void CDeath::UpdateMovement(bool aimMode, double dt)
{
}

void CDeath::UpdateChase(Vector3 tgtPos, bool isMoving , double dt)
{
	this->deathNode->GetGameObject()->setRotation(this->angle, 0, 1, 0);

	//Only start chasing player if player is not moving 
	if (!isMoving)
	{
		deathNode->setActive(true);
		Vector3 direction = (tgtPos - deathNode->GetGameObject()->getPosition());
		float distSquared = direction.LengthSquared();

		deathNode->GetGameObject()->addPosition(direction.Normalized() * moveSpeed * dt);
	}
	else
	{
		deathNode->setActive(false);
	}
		
}

void CDeath::UpdateAngle(float dt)
{
}

//Game related
void CDeath::SetHealth(int newHealth)
{
	this->health = newHealth;
}
void CDeath::SetLives(int newLives)
{
	this->lives = newLives;
}
int CDeath::GetHealth(void)
{
	return health;
}
int CDeath::GetLives(void)
{
	return lives;
}