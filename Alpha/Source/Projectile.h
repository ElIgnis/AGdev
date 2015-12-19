#ifndef CPROJECTILE_H
#define CPROJECTILE_H

#include "Vector3.h"
#include "GameObject3D.h"

class CProjectile : public GameObject3D
{
public:
	CProjectile(void);
	~CProjectile(void);

	void Update(const double dt, const float worldLength, const float worldWidth);	//Update status

	void Init(GameObject3D object, Vector3 newDirection, bool active, float speed);	//Initialize

	GameObject3D* getGameObject(void);	//Get this game object

	Vector3 GetDirection(void);	//Get direction of projectile

	void SetSpeed(float speed);
	float GetSpeed(void);

private:
	Vector3 m_v2Direction;	// The direction of the projectile

	float m_fSpeed;	// Speed
};

#endif