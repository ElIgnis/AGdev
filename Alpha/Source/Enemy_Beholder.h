#pragma once

#include "Entity.h"
#include "Free3DMovement.h"
#include "Weapon.h"

#define PATROL_DELAY 15.f
#define IDLE_DELAY 5.f

class CEnemy_Beholder : public Entity
{
public:
	enum FSM{
		IDLE = 0,
		PATROL,
		ATTACK,
		RUN,
		NUM_FSM,
	};

	enum FLOCKING{
		SEPARATION,
		COHESION,
		MAINTAIN,
		NUM_FLOCKING,
	};


	CEnemy_Beholder();
	~CEnemy_Beholder();

	//Init
	virtual void Init(Vector3 currentPos, Vector3 currentDir, Mesh* newMesh);	

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

	virtual bool GetIsMoving(void);
	virtual void SetIsMoving(bool moving);
	virtual bool GetInAir(void);

	virtual SceneNode* GetNode(void);

	//Update
	virtual void Update(double dt, float CamAngle);
	void Update(double dt, Vector3 tgtPos, vector<CEnemy_Beholder*>enemy_beholderList);
	virtual void UpdateMovement(bool aimMode, double dt);
	void UpdateIdle(double dt);
	void UpdatePatrol(double dt);
	void UpdateAttack(double dt, Vector3 tgtPos);
	void UpdateRun(double dt, Vector3 tgtPos);
	
	//Game related
	virtual void SetHealth(int newHealth);
	virtual void SetLives(int newLives);
	virtual int GetHealth(void);
	virtual int GetLives(void);
	void SetIsAlive(bool alive);
	bool GetIsAlive(void);

	FSM getCurrentState(void);
	void setCurrentState(FSM changeState);
	FLOCKING getCurrentFlock(void);
	void setCurrentFlock(FLOCKING changeFlock);
private:
	GameObject3D* GO3D_Object;
	SceneNode* Enemy_BeholderNode;
	bool Alive;
	FSM currentState;
	FLOCKING currentFlock;
	Vector3 direction;
	float distSquared;

	float patrolTimer;
	float idleTimer;
	Vector3 PatrolDirection;
};

