#include "Demon.h"

CDemon::CDemon()
{
}


CDemon::~CDemon()
{
}

void CDemon::Init(Vector3 currentPos, Vector3 currentDir, Mesh* playerMesh)
{
	demonNode = new SceneNode();
	GO3D_Object = new GameObject3D();
	demonNode->SetGameObject(GO3D_Object);
	
	demonNode->setActive(false);
	demonNode->GetGameObject()->setPosition(currentPos);
	demonNode->GetGameObject()->setMesh(playerMesh);
	demonNode->GetGameObject()->setName("Demon");
	demonNode->GetGameObject()->setUpdate(true);
	demonNode->GetGameObject()->setRender(true);
	demonNode->GetGameObject()->setCollidable(true);
}

//Dir
void CDemon::SetDirection(Vector3 newDirection)
{
	this->direction = newDirection;
}
void CDemon::SetDirection_X(float newDir_X)
{
	this->direction.x = newDir_X;
}
void CDemon::SetDirection_Y(float newDir_Y)
{
	this->direction.y = newDir_Y;
}
void CDemon::SetDirection_Z(float newDir_Z)
{
	this->direction.z = newDir_Z;
}
Vector3 CDemon::GetDirection(void)
{
	return direction;
}

//Angle
float CDemon::GetAngle(void)
{
	return angle;
}
void CDemon::SetAngle(float newAngle)
{
	this->angle = newAngle;
}
void CDemon::AddAngle(float newAngle, float dt)
{
	this->angle += (newAngle - this->angle) * dt;
}

//Physics
void CDemon::SetGravity(float newGravity)
{
	this->gravity = newGravity;
}
float CDemon::GetGravity(void)
{
	return gravity;
}
void CDemon::SetJumpSpeed(float newJSpeed)
{
	this->jumpSpeed = newJSpeed;
}
float CDemon::GetJumpSpeed(void)
{
	return jumpSpeed;
}
void CDemon::SetMoveSpeed(float newMSpeed)
{
	this->moveSpeed = newMSpeed;
}
float CDemon::GetMoveSpeed(void)
{
	return moveSpeed;
}

void CDemon::SetMoveSpeedMult(float newMSpeedMult)
{
	this->moveSpeed_Mult = newMSpeedMult;
}
float CDemon::GetMoveSpeedMult(void)
{
	return moveSpeed_Mult;
}

bool CDemon::GetIsMoving(void)
{
	return moving;
}

void CDemon::SetIsMoving(bool moving)
{
	this->moving = moving;
}
bool CDemon::GetInAir(void)
{
	return inAir;
}

SceneNode* CDemon::GetNode(void)
{
	return demonNode;
}

void CDemon::RotateLimb(string nodeName, float angle, float rotateSpeed, bool playOnce, double dt, float axisX, float axisY, float axisZ)
{
	//Left hand joint
	if (nodeName == "Enemy_LeftHand_Joint")
	{
		this->GetNode()->GetChildNode(nodeName)->GetGameObject()->setRotation(
			LeftHand.Animate(this->GetNode()->GetChildNode(nodeName)->GetGameObject()->getRotateAngle(), angle, rotateSpeed, dt, playOnce), axisX, axisY, axisZ);
	}
	if (nodeName == "Enemy_RightHand_Joint")
	{
		this->GetNode()->GetChildNode(nodeName)->GetGameObject()->setRotation(
			RightHand.Animate(this->GetNode()->GetChildNode(nodeName)->GetGameObject()->getRotateAngle(), angle, rotateSpeed, dt, playOnce), axisX, axisY, axisZ);
	}
	if (nodeName == "Enemy_LeftLeg_Joint")
	{
		this->GetNode()->GetChildNode(nodeName)->GetGameObject()->setRotation(
			LeftLeg.Animate(this->GetNode()->GetChildNode(nodeName)->GetGameObject()->getRotateAngle(), angle, rotateSpeed, dt, playOnce), axisX, axisY, axisZ);
	}
	if (nodeName == "Enemy_RightLeg_Joint")
	{
		this->GetNode()->GetChildNode(nodeName)->GetGameObject()->setRotation(
			RightLeg.Animate(this->GetNode()->GetChildNode(nodeName)->GetGameObject()->getRotateAngle(), angle, rotateSpeed, dt, playOnce), axisX, axisY, axisZ);
	}
}

void CDemon::RevertLimb(bool aimMode, double dt)
{
}

//Update
void CDemon::Update(double dt, float CamAngle)
{
}

void CDemon::UpdateMovement(bool aimMode, double dt)
{	
}

void CDemon::UpdateChase(Vector3 tgtPos, double dt)
{

	Vector3 direction = (tgtPos - demonNode->GetGameObject()->getPosition());
	float distSquared = direction.LengthSquared();

	demonNode->GetGameObject()->addPosition(direction.Normalized() * moveSpeed * dt);

	//Used for collision response for all gameobjects
	demonNode->GetGameObject()->addPosition(this->GetNode()->GetGameObject()->getVelocity() * (float)dt);
	if (demonNode->GetChildNode("Enemy_Head") != NULL)
	{
		demonNode->GetChildNode("Enemy_Head")->SetWorldPosition(demonNode->GetGameObject()->getPosition() + demonNode->GetChildNode("Enemy_Head")->GetGameObject()->getPosition());
	}
	
	//Left Hand
	if (demonNode->GetChildNode("Enemy_LeftHand_Joint") != NULL)
	{
		RotateLimb("Enemy_LeftHand_Joint", 35, 200, false, dt, 1, 0, 0);
		demonNode->GetChildNode("Enemy_LeftHand_Joint")->SetWorldPosition(demonNode->GetGameObject()->getPosition() + demonNode->GetChildNode("Enemy_LeftHand_Joint")->GetGameObject()->getPosition());
		
		if (demonNode->GetChildNode("Enemy_LeftHand_Joint")->GetChildNode("Enemy_LeftHand") != NULL)
		{
			demonNode->GetChildNode("Enemy_LeftHand_Joint")->GetChildNode("Enemy_LeftHand")->SetWorldPosition(demonNode->GetGameObject()->getPosition() + demonNode->GetChildNode("Enemy_LeftHand_Joint")->GetChildNode("Enemy_LeftHand")->GetGameObject()->getPosition());
		}
	}
	

	//Right Hand
	if (demonNode->GetChildNode("Enemy_RightHand_Joint") != NULL)
	{
		RotateLimb("Enemy_RightHand_Joint", -35, 200, false, dt, 1, 0, 0);
		demonNode->GetChildNode("Enemy_RightHand_Joint")->SetWorldPosition(demonNode->GetGameObject()->getPosition() + demonNode->GetChildNode("Enemy_RightHand_Joint")->GetGameObject()->getPosition());
		
		if (demonNode->GetChildNode("Enemy_RightHand_Joint")->GetChildNode("Enemy_RightHand") != NULL)
		{
			demonNode->GetChildNode("Enemy_RightHand_Joint")->GetChildNode("Enemy_RightHand")->SetWorldPosition(demonNode->GetGameObject()->getPosition() + demonNode->GetChildNode("Enemy_RightHand_Joint")->GetChildNode("Enemy_RightHand")->GetGameObject()->getPosition());
		}
	}
	

	//Left Leg
	if (demonNode->GetChildNode("Enemy_LeftLeg_Joint") != NULL)
	{
		RotateLimb("Enemy_LeftLeg_Joint", -35, 200, false, dt, 1, 0, 0);
		demonNode->GetChildNode("Enemy_LeftLeg_Joint")->SetWorldPosition(demonNode->GetGameObject()->getPosition() + demonNode->GetChildNode("Enemy_LeftLeg_Joint")->GetGameObject()->getPosition());
		
		if (demonNode->GetChildNode("Enemy_LeftLeg_Joint")->GetChildNode("Enemy_LeftLeg") != NULL)
		{
			demonNode->GetChildNode("Enemy_LeftLeg_Joint")->GetChildNode("Enemy_LeftLeg")->SetWorldPosition(demonNode->GetGameObject()->getPosition() + demonNode->GetChildNode("Enemy_LeftLeg_Joint")->GetChildNode("Enemy_LeftLeg")->GetGameObject()->getPosition());
		}
	}
	

	//Right Leg
	if (demonNode->GetChildNode("Enemy_RightLeg_Joint") != NULL)
	{
		RotateLimb("Enemy_RightLeg_Joint", 35, 200, false, dt, 1, 0, 0);
		demonNode->GetChildNode("Enemy_RightLeg_Joint")->SetWorldPosition(demonNode->GetGameObject()->getPosition() + demonNode->GetChildNode("Enemy_RightLeg_Joint")->GetGameObject()->getPosition());
		
		if (demonNode->GetChildNode("Enemy_RightLeg_Joint")->GetChildNode("Enemy_RightLeg") != NULL)
		{
			demonNode->GetChildNode("Enemy_RightLeg_Joint")->GetChildNode("Enemy_RightLeg")->SetWorldPosition(demonNode->GetGameObject()->getPosition() + demonNode->GetChildNode("Enemy_RightLeg_Joint")->GetChildNode("Enemy_RightLeg")->GetGameObject()->getPosition());
		}
	}
	
}

void CDemon::UpdateAngle(float dt)
{
	orientation = Math::DegreeToRadian(angle);

	direction.x = sinf(orientation);
	direction.z = cosf(orientation);
}

//Game related
void CDemon::SetHealth(int newHealth)
{
	this->health = newHealth;
}
void CDemon::SetLives(int newLives)
{
	this->lives = newLives;
}
int CDemon::GetHealth(void)
{
	return health;
}
int CDemon::GetLives(void)
{
	return lives;
}