#include "Player.h"

CPlayer* CPlayer::instance = 0;

CPlayer* CPlayer::GetInstance()
{
	if (instance == 0)
		instance = new CPlayer();

	return instance;
}

CPlayer::CPlayer()
{
}


CPlayer::~CPlayer()
{
}

void CPlayer::Init(Vector3 currentPos, Vector3 currentDir, Mesh* playerMesh)
{
	playerNode = new SceneNode();
	GO3D_Object = new GameObject3D();
	playerNode->SetGameObject(GO3D_Object);
	
	playerNode->setActive(true);
	playerNode->GetGameObject()->setPosition(currentPos);
	playerNode->GetGameObject()->setMesh(playerMesh);
	playerNode->GetGameObject()->setName("Player");
	playerNode->GetGameObject()->setUpdate(true);
	playerNode->GetGameObject()->setRender(true);
	playerNode->GetGameObject()->setCollidable(true);

	Pistol = new Weapon();
}

//Dir
void CPlayer::SetDirection(Vector3 newDirection)
{
	this->direction = newDirection;
}
void CPlayer::SetDirection_X(float newDir_X)
{
	this->direction.x = newDir_X;
}
void CPlayer::SetDirection_Y(float newDir_Y)
{
	this->direction.y = newDir_Y;
}
void CPlayer::SetDirection_Z(float newDir_Z)
{
	this->direction.z = newDir_Z;
}
Vector3 CPlayer::GetDirection(void)
{
	return direction;
}

//Angle
float CPlayer::GetAngle(void)
{
	return angle;
}
void CPlayer::SetAngle(float newAngle)
{
	this->angle = newAngle;
}

//Physics
void CPlayer::SetGravity(float newGravity)
{
	this->gravity = newGravity;
}
float CPlayer::GetGravity(void)
{
	return gravity;
}
void CPlayer::SetJumpSpeed(float newJSpeed)
{
	this->jumpSpeed = newJSpeed;
}
float CPlayer::GetJumpSpeed(void)
{
	return jumpSpeed;
}
void CPlayer::SetMoveSpeed(float newMSpeed)
{
	this->moveSpeed = newMSpeed;
}
float CPlayer::GetMoveSpeed(void)
{
	return moveSpeed;
}

void CPlayer::SetMoveSpeedMult(float newMSpeedMult)
{
	this->moveSpeed_Mult = newMSpeedMult;
}
float CPlayer::GetMoveSpeedMult(void)
{
	return moveSpeed_Mult;
}

bool CPlayer::GetInAir(void)
{
	return inAir;
}

SceneNode* CPlayer::GetNode(void)
{
	return playerNode;
}

void CPlayer::RotateLimb(string nodeName, float angle, float rotateSpeed, bool playOnce, double dt, float axisX, float axisY, float axisZ)
{
	//Left hand joint
	if (nodeName == "LeftHand_Joint")
	{
		this->GetNode()->GetChildNode(nodeName)->GetGameObject()->setRotation(
			LeftHand.Animate(this->GetNode()->GetChildNode(nodeName)->GetGameObject()->getRotateAngle(), angle, rotateSpeed, dt, playOnce), axisX, axisY, axisZ);
	}
	if (nodeName == "RightHand_Joint")
	{
		this->GetNode()->GetChildNode(nodeName)->GetGameObject()->setRotation(
			RightHand.Animate(this->GetNode()->GetChildNode(nodeName)->GetGameObject()->getRotateAngle(), angle, rotateSpeed, dt, playOnce), axisX, axisY, axisZ);
	}
	if (nodeName == "LeftLeg_Joint")
	{
		this->GetNode()->GetChildNode(nodeName)->GetGameObject()->setRotation(
			LeftLeg.Animate(this->GetNode()->GetChildNode(nodeName)->GetGameObject()->getRotateAngle(), angle, rotateSpeed, dt, playOnce), axisX, axisY, axisZ);
	}
	if (nodeName == "RightLeg_Joint")
	{
		this->GetNode()->GetChildNode(nodeName)->GetGameObject()->setRotation(
			RightLeg.Animate(this->GetNode()->GetChildNode(nodeName)->GetGameObject()->getRotateAngle(), angle, rotateSpeed, dt, playOnce), axisX, axisY, axisZ);
	}
}

void CPlayer::RevertLimb(bool aimMode, double dt)
{
	if (aimMode)
	{
		RotateLimb("LeftHand_Joint", 0, 100, false, dt, 1, 0, 0);
		RotateLimb("LeftLeg_Joint", 0, 100, false, dt, 1, 0, 0);
		RotateLimb("RightLeg_Joint", 0, 100, false, dt, 1, 0, 0);
	}
	else
	{
		RotateLimb("LeftHand_Joint", 0, 100, false, dt, 1, 0, 0);
		RotateLimb("RightHand_Joint", 0, 100, false, dt, 1, 0, 0);
		RotateLimb("LeftLeg_Joint", 0, 100, false, dt, 1, 0, 0);
		RotateLimb("RightLeg_Joint", 0, 100, false, dt, 1, 0, 0);
	}
}

//Update
void CPlayer::Update(double dt, float CamAngle)
{
	playerNode->GetChildNode("Head")->SetWorldPosition(playerNode->GetGameObject()->getPosition() + playerNode->GetChildNode("Head")->GetGameObject()->getPosition());

	force.x = moveSpeed * moveSpeed_Mult;
	force.z = moveSpeed * moveSpeed_Mult;

	orientation = Math::DegreeToRadian(CamAngle);

	direction.x = sinf(orientation);
	direction.z = cosf(orientation);

	Vector3 acceleration = force * (1.f / mass);

	velocity.x = sinf(orientation) * acceleration.x;
	velocity.z = cosf(orientation) * acceleration.z;
}

void CPlayer::UpdateMovement(bool aimMode, double dt)
{
	//Update animation
	if (!velocity.IsZero())
	{
		if (!aimMode)
			RotateLimb("RightHand_Joint", -35, 100, false, dt, 1, 0, 0);
		RotateLimb("LeftHand_Joint", 35, 100, false, dt, 1, 0, 0);
		RotateLimb("LeftLeg_Joint", -35, 100, false, dt, 1, 0, 0);
		RotateLimb("RightLeg_Joint", 35, 100, false, dt, 1, 0, 0);
	}

	playerNode->GetGameObject()->addPosition(velocity * (float)dt);
	playerNode->GetChildNode("Head")->SetWorldPosition(playerNode->GetGameObject()->getPosition() + playerNode->GetChildNode("Head")->GetGameObject()->getPosition());
	
	playerNode->GetChildNode("LeftHand_Joint")->SetWorldPosition(playerNode->GetGameObject()->getPosition() + playerNode->GetChildNode("LeftHand_Joint")->GetGameObject()->getPosition());
	playerNode->GetChildNode("LeftHand_Joint")->GetChildNode("LeftHand")->SetWorldPosition(playerNode->GetGameObject()->getPosition() + playerNode->GetChildNode("LeftHand_Joint")->GetChildNode("LeftHand")->GetGameObject()->getPosition());
	
	playerNode->GetChildNode("RightHand_Joint")->SetWorldPosition(playerNode->GetGameObject()->getPosition() + playerNode->GetChildNode("RightHand_Joint")->GetGameObject()->getPosition());
	playerNode->GetChildNode("RightHand_Joint")->GetChildNode("RightHand")->SetWorldPosition(playerNode->GetGameObject()->getPosition() + playerNode->GetChildNode("RightHand_Joint")->GetChildNode("RightHand")->GetGameObject()->getPosition());
	playerNode->GetChildNode("RightHand_Joint")->GetChildNode("RightHand")->GetChildNode("Weapon")->SetWorldPosition(playerNode->GetGameObject()->getPosition() + playerNode->GetChildNode("RightHand_Joint")->GetChildNode("RightHand")->GetChildNode("Weapon")->GetGameObject()->getPosition());
	
	playerNode->GetChildNode("LeftLeg_Joint")->SetWorldPosition(playerNode->GetGameObject()->getPosition() + playerNode->GetChildNode("LeftLeg_Joint")->GetGameObject()->getPosition());
	playerNode->GetChildNode("LeftLeg_Joint")->GetChildNode("LeftLeg")->SetWorldPosition(playerNode->GetGameObject()->getPosition() + playerNode->GetChildNode("LeftLeg_Joint")->GetChildNode("LeftLeg")->GetGameObject()->getPosition());
	
	playerNode->GetChildNode("RightLeg_Joint")->SetWorldPosition(playerNode->GetGameObject()->getPosition() + playerNode->GetChildNode("RightLeg_Joint")->GetGameObject()->getPosition());
	playerNode->GetChildNode("RightLeg_Joint")->GetChildNode("RightLeg")->SetWorldPosition(playerNode->GetGameObject()->getPosition() + playerNode->GetChildNode("RightLeg_Joint")->GetChildNode("RightLeg")->GetGameObject()->getPosition());	
}

void CPlayer::UpdateAngle(float dt)
{
	orientation = Math::DegreeToRadian(angle);

	direction.x = sinf(orientation);
	direction.z = cosf(orientation);
}

Vector3 CPlayer::GetVelocity(void)
{
	return velocity;
}

void CPlayer::SetVelocity(Vector3 newVelocity)
{
	this->velocity = newVelocity;
}

//Game related
void CPlayer::SetHealth(int newHealth)
{
	this->health = newHealth;
}
void CPlayer::SetLives(int newLives)
{
	this->lives = newLives;
}
int CPlayer::GetHealth(void)
{
	return health;
}
int CPlayer::GetLives(void)
{
	return lives;
}
Weapon* CPlayer::GetWeapon(void)
{
	return Pistol;
}
