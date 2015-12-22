#include "DemonSpawner.h"

CDemonSpawner::CDemonSpawner()
{
}


CDemonSpawner::~CDemonSpawner()
{
}

void CDemonSpawner::Init(Vector3 currentPos, Vector3 currentDir, Mesh* playerMesh)
{
	demonSpawnerNode = new SceneNode();
	GO3D_Object = new GameObject3D();
	demonSpawnerNode->SetGameObject(GO3D_Object);
	
	demonSpawnerNode->setActive(true);
	demonSpawnerNode->GetGameObject()->setPosition(currentPos);
	demonSpawnerNode->GetGameObject()->setMesh(playerMesh);
	demonSpawnerNode->GetGameObject()->setName("DemonSpawner");
	demonSpawnerNode->GetGameObject()->setUpdate(true);
	demonSpawnerNode->GetGameObject()->setRender(true);
	demonSpawnerNode->GetGameObject()->setCollidable(true);
}

//Dir
void CDemonSpawner::SetDirection(Vector3 newDirection)
{
	this->direction = newDirection;
}
void CDemonSpawner::SetDirection_X(float newDir_X)
{
	this->direction.x = newDir_X;
}
void CDemonSpawner::SetDirection_Y(float newDir_Y)
{
	this->direction.y = newDir_Y;
}
void CDemonSpawner::SetDirection_Z(float newDir_Z)
{
	this->direction.z = newDir_Z;
}
Vector3 CDemonSpawner::GetDirection(void)
{
	return direction;
}

//Angle
float CDemonSpawner::GetAngle(void)
{
	return angle;
}
void CDemonSpawner::SetAngle(float newAngle)
{
	this->angle = newAngle;
}
void CDemonSpawner::AddAngle(float newAngle, float dt)
{
	this->angle += (newAngle - this->angle) * dt;
}

//Physics
void CDemonSpawner::SetGravity(float newGravity)
{
	this->gravity = newGravity;
}
float CDemonSpawner::GetGravity(void)
{
	return gravity;
}
void CDemonSpawner::SetJumpSpeed(float newJSpeed)
{
	this->jumpSpeed = newJSpeed;
}
float CDemonSpawner::GetJumpSpeed(void)
{
	return jumpSpeed;
}
void CDemonSpawner::SetMoveSpeed(float newMSpeed)
{
	this->moveSpeed = newMSpeed;
}
float CDemonSpawner::GetMoveSpeed(void)
{
	return moveSpeed;
}

void CDemonSpawner::SetMoveSpeedMult(float newMSpeedMult)
{
	this->moveSpeed_Mult = newMSpeedMult;
}
float CDemonSpawner::GetMoveSpeedMult(void)
{
	return moveSpeed_Mult;
}

bool CDemonSpawner::GetIsMoving(void)
{
	return moving;
}

void CDemonSpawner::SetIsMoving(bool moving)
{
	this->moving = moving;
}
bool CDemonSpawner::GetInAir(void)
{
	return inAir;
}

SceneNode* CDemonSpawner::GetNode(void)
{
	return demonSpawnerNode;
}

void CDemonSpawner::RotateLimb(string nodeName, float angle, float rotateSpeed, bool playOnce, double dt, float axisX, float axisY, float axisZ)
{
}

void CDemonSpawner::RevertLimb(bool aimMode, double dt)
{
}

//Update
void CDemonSpawner::Update(double dt, float CamAngle)
{
	spawnTimer += dt;

	//Turns on detected at fixed intervals with probability
	if (spawnTimer > SpawnDelay)
	{
		SpawnRNG();
	}
	if (canSpawn == true && spawnTimer > SpawnDuration)
	{
		canSpawn = false;
	}
}

void CDemonSpawner::Update(double dt)
{
	spawnTimer += dt;

	//Turns on detected at fixed intervals with probability
	if (spawnTimer > SpawnDelay)
	{
		SpawnRNG();
	}
	if (canSpawn == true && spawnTimer > SpawnDuration)
	{
		canSpawn = false;
	}
}

void CDemonSpawner::UpdateMovement(bool aimMode, double dt)
{	
}

void CDemonSpawner::SpawnEnemy(Vector3 playerPos, double dt)
{
	//Only check dist if able to spawn
	if (canSpawn)
	{
		float distSquared = (playerPos - demonSpawnerNode->GetGameObject()->getPosition()).LengthSquared();

		if (distSquared < SpawnRange && canSpawn)
		{
			spawnDemon = true;
		}
		else
		{
			spawnDemon = false;
		}
	}
}

void CDemonSpawner::SpawnRNG(void)
{
	spawnTimer = 0.f;
	float RNG = rand() % 100 + 1;
	if (RNG >= Chance)
		canSpawn = true;
	else
		canSpawn = false;
}

void CDemonSpawner::SetCanSpawn(bool spawn)
{
	this->canSpawn = spawn;
}

void CDemonSpawner::UpdateAngle(float dt)
{
	orientation = Math::DegreeToRadian(angle);

	direction.x = sinf(orientation);
	direction.z = cosf(orientation);
}

//Game related
void CDemonSpawner::SetHealth(int newHealth)
{
	this->health = newHealth;
}
void CDemonSpawner::SetLives(int newLives)
{
	this->lives = newLives;
}
int CDemonSpawner::GetHealth(void)
{
	return health;
}
int CDemonSpawner::GetLives(void)
{
	return lives;
}