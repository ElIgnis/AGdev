#pragma once

#include "Entity.h"
#include "Free3DMovement.h"
#include "Weapon.h"

class CJoker : public Entity
{
public:
	CJoker();
	~CJoker();

	//Init
	virtual void Init(Vector3 currentPos, Vector3 currentDir, Mesh* playerMesh);

	//Dir
	virtual void SetDirection(Vector3 newDirection);
	virtual void SetDirection_X(float newDir_X);
	virtual void SetDirection_Y(float newDir_Y);
	virtual void SetDirection_Z(float newDir_Z);
	virtual Vector3 GetDirection(void);

	//Angle
	virtual float GetAngle(void);
	virtual void SetAngle(float newAngle);
	//Physics
	virtual void SetGravity(float newGravity);
	virtual float GetGravity(void);
	//Jump Speed
	virtual void SetJumpSpeed(float newJSpeed);
	virtual float GetJumpSpeed(void);
	//Move Speed
	virtual void SetMoveSpeed(float newMSpeed);
	virtual float GetMoveSpeed(void);
	//Move Speed Mult
	virtual void SetMoveSpeedMult(float newMSpeedMult);
	virtual float GetMoveSpeedMult(void);
	//Update Angle
	virtual void UpdateAngle(float dt);
	virtual void RotateLimb(string nodeName, float angle, float rotateSpeed, bool playOnce, double dt, float axisX, float axisY, float axisZ);
	virtual void RevertLimb(bool aimMode, double dt);

	virtual bool GetInAir(void);

	virtual SceneNode* GetNode(void);

	//Update
	virtual void Update(double dt, float CamAngle);
	virtual void UpdateMovement(bool aimMode, double dt);

	//Game related
	virtual void SetHealth(int newHealth);
	virtual void SetLives(int newLives);
	virtual int GetHealth(void);
	virtual int GetLives(void);

	Weapon* GetWeapon(void);

private:
	GameObject3D* GO3D_Object;
	SceneNode* jokerNode;
	Free3DMovement LeftHand, RightHand, LeftLeg, RightLeg;
	Weapon* Pistol;
};

