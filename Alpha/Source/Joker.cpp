#include "Joker.h"

CJoker::CJoker()
{
}


CJoker::~CJoker()
{
}

void CJoker::Init(Vector3 currentPos, Vector3 currentDir, Mesh* playerMesh)
{
	jokerNode = new SceneNode();
	GO3D_Object = new GameObject3D();
	jokerNode->SetGameObject(GO3D_Object);
	
	jokerNode->setActive(true);
	jokerNode->GetGameObject()->setPosition(currentPos);
	jokerNode->GetGameObject()->setMesh(playerMesh);
	jokerNode->GetGameObject()->setName("Joker");
	jokerNode->GetGameObject()->setUpdate(true);
	jokerNode->GetGameObject()->setRender(true);
	jokerNode->GetGameObject()->setCollidable(true);

	Pistol = new Weapon();
}

//Dir
void CJoker::SetDirection(Vector3 newDirection)
{
	this->direction = newDirection;
}
void CJoker::SetDirection_X(float newDir_X)
{
	this->direction.x = newDir_X;
}
void CJoker::SetDirection_Y(float newDir_Y)
{
	this->direction.y = newDir_Y;
}
void CJoker::SetDirection_Z(float newDir_Z)
{
	this->direction.z = newDir_Z;
}
Vector3 CJoker::GetDirection(void)
{
	return direction;
}

//Angle
float CJoker::GetAngle(void)
{
	return angle;
}
void CJoker::SetAngle(float newAngle)
{
	this->angle = newAngle;
}

//Physics
void CJoker::SetGravity(float newGravity)
{
	this->gravity = newGravity;
}
float CJoker::GetGravity(void)
{
	return gravity;
}
void CJoker::SetJumpSpeed(float newJSpeed)
{
	this->jumpSpeed = newJSpeed;
}
float CJoker::GetJumpSpeed(void)
{
	return jumpSpeed;
}
void CJoker::SetMoveSpeed(float newMSpeed)
{
	this->moveSpeed = newMSpeed;
}
float CJoker::GetMoveSpeed(void)
{
	return moveSpeed;
}

void CJoker::SetMoveSpeedMult(float newMSpeedMult)
{
	this->moveSpeed_Mult = newMSpeedMult;
}
float CJoker::GetMoveSpeedMult(void)
{
	return moveSpeed_Mult;
}

bool CJoker::GetInAir(void)
{
	return inAir;
}

SceneNode* CJoker::GetNode(void)
{
	return jokerNode;
}

void CJoker::RotateLimb(string nodeName, float angle, float rotateSpeed, bool playOnce, double dt, float axisX, float axisY, float axisZ)
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

void CJoker::RevertLimb(bool aimMode, double dt)
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
void CJoker::Update(double dt, float CamAngle)
{
	//jokerNode->GetChildNode("Head")->SetWorldPosition(jokerNode->GetGameObject()->getPosition() + jokerNode->GetChildNode("Head")->GetGameObject()->getPosition());
}

void CJoker::UpdateMovement(bool aimMode, double dt)
{
	//Update animation
	RotateLimb("LeftHand_Joint", 35, 100, false, dt, 1, 0, 0);
	RotateLimb("RightHand_Joint", -35, 100, false, dt, 1, 0, 0);
	RotateLimb("LeftLeg_Joint", -35, 100, false, dt, 1, 0, 0);
	RotateLimb("RightLeg_Joint", 35, 100, false, dt, 1, 0, 0);
	
	moving = true;
	force.x = moveSpeed * moveSpeed_Mult;
	force.z = moveSpeed * moveSpeed_Mult;

	orientation = Math::DegreeToRadian(angle);
	
	direction.x = sinf(orientation);
	direction.z = cosf(orientation);

	Vector3 acceleration = force * (1.f / mass);

	velocity.x = sinf(orientation) * acceleration.x;
	velocity.z = cosf(orientation) * acceleration.z;

	jokerNode->GetGameObject()->addPosition(velocity * (float)dt);
	jokerNode->GetChildNode("Head")->SetWorldPosition(jokerNode->GetGameObject()->getPosition() + jokerNode->GetChildNode("Head")->GetGameObject()->getPosition());
	
	jokerNode->GetChildNode("LeftHand_Joint")->SetWorldPosition(jokerNode->GetGameObject()->getPosition() + jokerNode->GetChildNode("LeftHand_Joint")->GetGameObject()->getPosition());
	jokerNode->GetChildNode("LeftHand_Joint")->GetChildNode("LeftHand")->SetWorldPosition(jokerNode->GetGameObject()->getPosition() + jokerNode->GetChildNode("LeftHand_Joint")->GetChildNode("LeftHand")->GetGameObject()->getPosition());
	
	jokerNode->GetChildNode("RightHand_Joint")->SetWorldPosition(jokerNode->GetGameObject()->getPosition() + jokerNode->GetChildNode("RightHand_Joint")->GetGameObject()->getPosition());
	jokerNode->GetChildNode("RightHand_Joint")->GetChildNode("RightHand")->SetWorldPosition(jokerNode->GetGameObject()->getPosition() + jokerNode->GetChildNode("RightHand_Joint")->GetChildNode("RightHand")->GetGameObject()->getPosition());
	jokerNode->GetChildNode("RightHand_Joint")->GetChildNode("RightHand")->GetChildNode("Weapon")->SetWorldPosition(jokerNode->GetGameObject()->getPosition() + jokerNode->GetChildNode("RightHand_Joint")->GetChildNode("RightHand")->GetChildNode("Weapon")->GetGameObject()->getPosition());
	
	jokerNode->GetChildNode("LeftLeg_Joint")->SetWorldPosition(jokerNode->GetGameObject()->getPosition() + jokerNode->GetChildNode("LeftLeg_Joint")->GetGameObject()->getPosition());
	jokerNode->GetChildNode("LeftLeg_Joint")->GetChildNode("LeftLeg")->SetWorldPosition(jokerNode->GetGameObject()->getPosition() + jokerNode->GetChildNode("LeftLeg_Joint")->GetChildNode("LeftLeg")->GetGameObject()->getPosition());
	
	jokerNode->GetChildNode("RightLeg_Joint")->SetWorldPosition(jokerNode->GetGameObject()->getPosition() + jokerNode->GetChildNode("RightLeg_Joint")->GetGameObject()->getPosition());
	jokerNode->GetChildNode("RightLeg_Joint")->GetChildNode("RightLeg")->SetWorldPosition(jokerNode->GetGameObject()->getPosition() + jokerNode->GetChildNode("RightLeg_Joint")->GetChildNode("RightLeg")->GetGameObject()->getPosition());
	
}

void CJoker::UpdateAngle(float dt)
{
	orientation = Math::DegreeToRadian(angle);

	direction.x = sinf(orientation);
	direction.z = cosf(orientation);
}

Vector3 CJoker::GetVelocity(void)
{
	return velocity;
}

void CJoker::SetVelocity(Vector3 newVelocity)
{
	this->velocity = newVelocity;
}

//Game related
void CJoker::SetHealth(int newHealth)
{
	this->health = newHealth;
}
void CJoker::SetLives(int newLives)
{
	this->lives = newLives;
}
int CJoker::GetHealth(void)
{
	return health;
}
int CJoker::GetLives(void)
{
	return lives;
}
Weapon* CJoker::GetWeapon(void)
{
	return Pistol;
}
