#include "Enemy_Beholder.h"

CEnemy_Beholder::CEnemy_Beholder()
: Alive(true)
, currentState(IDLE)
, currentFlock(MAINTAIN)
, patrolTimer(0.f)
, idleTimer(0.f)
, PatrolDirection(RandFloatMinMax(0.f, 1.f), 0.f, RandFloatMinMax(0.f, 1.f))
{
}

CEnemy_Beholder::~CEnemy_Beholder()
{
}

void CEnemy_Beholder::Init(Vector3 currentPos, Vector3 currentDir, Mesh* newMesh)
{
	Enemy_BeholderNode = new SceneNode();
	GO3D_Object = new GameObject3D();
	Enemy_BeholderNode->SetGameObject(GO3D_Object);
	
	Enemy_BeholderNode->setActive(true);
	Enemy_BeholderNode->GetGameObject()->setPosition(currentPos);
	Enemy_BeholderNode->GetGameObject()->setMesh(newMesh);
	Enemy_BeholderNode->GetGameObject()->setName("Enemy_Beholder");
	Enemy_BeholderNode->GetGameObject()->setUpdate(true);
	Enemy_BeholderNode->GetGameObject()->setRender(true);
	Enemy_BeholderNode->GetGameObject()->setCollidable(true);

	this->moveSpeed = 100;
}

//Dir
void CEnemy_Beholder::SetDirection(Vector3 newDirection)
{
	this->direction = newDirection;
}
void CEnemy_Beholder::SetDirection_X(float newDir_X)
{
	this->direction.x = newDir_X;
}
void CEnemy_Beholder::SetDirection_Y(float newDir_Y)
{
	this->direction.y = newDir_Y;
}
void CEnemy_Beholder::SetDirection_Z(float newDir_Z)
{
	this->direction.z = newDir_Z;
}
Vector3 CEnemy_Beholder::GetDirection(void)
{
	return direction;
}

//Angle
float CEnemy_Beholder::GetAngle(void)
{
	return angle;
}
void CEnemy_Beholder::SetAngle(float newAngle)
{
	this->angle = newAngle;
}

//Physics
void CEnemy_Beholder::SetGravity(float newGravity)
{
	this->gravity = newGravity;
}
float CEnemy_Beholder::GetGravity(void)
{
	return gravity;
}
void CEnemy_Beholder::SetJumpSpeed(float newJSpeed)
{
	this->jumpSpeed = newJSpeed;
}
float CEnemy_Beholder::GetJumpSpeed(void)
{
	return jumpSpeed;
}
void CEnemy_Beholder::SetMoveSpeed(float newMSpeed)
{
	this->moveSpeed = newMSpeed;
}
float CEnemy_Beholder::GetMoveSpeed(void)
{
	return moveSpeed;
}

void CEnemy_Beholder::SetMoveSpeedMult(float newMSpeedMult)
{
	this->moveSpeed_Mult = newMSpeedMult;
}
float CEnemy_Beholder::GetMoveSpeedMult(void)
{
	return moveSpeed_Mult;
}

bool CEnemy_Beholder::GetIsMoving(void)
{
	return moving;
}

void CEnemy_Beholder::SetIsMoving(bool moving)
{
	this->moving = moving;
}
bool CEnemy_Beholder::GetInAir(void)
{
	return inAir;
}

SceneNode* CEnemy_Beholder::GetNode(void)
{
	return Enemy_BeholderNode;
}

void CEnemy_Beholder::RotateLimb(string nodeName, float angle, float rotateSpeed, bool playOnce, double dt, float axisX, float axisY, float axisZ)
{
}

void CEnemy_Beholder::RevertLimb(bool aimMode, double dt)
{
}

void CEnemy_Beholder::SetIsAlive(bool alive)
{
	this->Alive = alive;
}

bool CEnemy_Beholder::GetIsAlive(void)
{
	return Alive;
}

//Update
void CEnemy_Beholder::Update(double dt, float CamAngle)
{

}

void CEnemy_Beholder::Update(double dt, Vector3 tgtPos, vector<CEnemy_Beholder*>enemy_beholderList)
{
	//Update flocking
	if (Enemy_BeholderNode->getActive())
	{
		
	}

	//Update enemies based on state
	switch (currentState)
	{
	case IDLE:
		UpdateIdle(dt);
		break;
	case PATROL:
		UpdatePatrol(dt);
		break;
	case ATTACK:
		UpdateAttack(dt, tgtPos);
		break;
	case RUN:
		UpdateRun(dt, tgtPos);
		break;
	default:
		break;
	}

	//World Wrapping 
	if (Enemy_BeholderNode->GetGameObject()->getPosition().x < 0)
	{
		Enemy_BeholderNode->GetGameObject()->setPositionX(4000.f);
	}
	if (Enemy_BeholderNode->GetGameObject()->getPosition().x > 4000)
	{
		Enemy_BeholderNode->GetGameObject()->setPositionX(0.f);
	}

	if (Enemy_BeholderNode->GetGameObject()->getPosition().z < 0)
	{
		Enemy_BeholderNode->GetGameObject()->setPositionZ(4000.f);
	}
	if (Enemy_BeholderNode->GetGameObject()->getPosition().z > 4000)
	{
		Enemy_BeholderNode->GetGameObject()->setPositionZ(0.f);
	}

}

void CEnemy_Beholder::UpdateMovement(bool aimMode, double dt)
{
}

void CEnemy_Beholder::UpdateIdle(double dt)
{
	idleTimer += dt;

	if (idleTimer > IDLE_DELAY)
	{
		idleTimer = 0.f;
		currentState = PATROL;
	}
}

void CEnemy_Beholder::UpdatePatrol(double dt)
{
	patrolTimer += dt;

	if (patrolTimer > PATROL_DELAY)
	{
		patrolTimer = 0.f;
		currentState = IDLE;
		PatrolDirection.x = PatrolDirection.z = RandFloatMinMax(0.f, 1.f);
	}

	//Randomize a direction to move in
	this->Enemy_BeholderNode->GetGameObject()->setRotation(this->angle, 0, 1, 0);
	
	Enemy_BeholderNode->GetGameObject()->addPosition(PatrolDirection * moveSpeed * dt);
}

void CEnemy_Beholder::UpdateAttack(double dt, Vector3 tgtPos)
{
	//Update direction of beholder towards player
	direction = (tgtPos - Enemy_BeholderNode->GetGameObject()->getPosition());
	distSquared = direction.LengthSquared();
	this->Enemy_BeholderNode->GetGameObject()->setRotation(this->angle, 0, 1, 0);
	Enemy_BeholderNode->GetGameObject()->addPosition(direction.Normalized() * moveSpeed * dt);
}

void CEnemy_Beholder::UpdateRun(double dt, Vector3 tgtPos)
{   
	//Update direction of beholder towards player
	direction = (tgtPos - Enemy_BeholderNode->GetGameObject()->getPosition());
	distSquared = direction.LengthSquared();
	this->Enemy_BeholderNode->GetGameObject()->setRotation(this->angle + 180.f, 0, 1, 0);
	Enemy_BeholderNode->GetGameObject()->addPosition(direction.Normalized() * -moveSpeed * dt);
}

void CEnemy_Beholder::UpdateAngle(float dt)
{
}

//Game related
void CEnemy_Beholder::SetHealth(int newHealth)
{
	this->health = newHealth;
}
void CEnemy_Beholder::SetLives(int newLives)
{
	this->lives = newLives;
}
int CEnemy_Beholder::GetHealth(void)
{
	return health;
}
int CEnemy_Beholder::GetLives(void)
{
	return lives;
}

//FSM and flocking
CEnemy_Beholder::FSM CEnemy_Beholder::getCurrentState(void)
{
	return currentState;
}
void CEnemy_Beholder::setCurrentState(FSM changeState)
{
	this->currentState = changeState;
}
CEnemy_Beholder::FLOCKING CEnemy_Beholder::getCurrentFlock(void)
{
	return currentFlock;
}
void CEnemy_Beholder::setCurrentFlock(FLOCKING changeFlock)
{
	this->currentFlock = changeFlock;
}