/******************************************************************************/
/*!
\file	Free3DMovement.h
\author Wei Liang Lee
\par	email: Intervention\.408\@live.com
\brief
3D Movement for 2 joints per hands/legs FOR HUMANOIDS ONLY
Note: Hierarchical must be set up properly(Per limb tagged to per joint)
Short forms: 
LH = Left Hand
RH = Right Hand
LL = Left Leg
RL = Right Leg
UJ = Upper Joint
LJ = Lower Joint
*/
/******************************************************************************/
#ifndef _FREE3DMOVEMENT_H
#define _FREE3DMOVEMENT_H

/******************************************************************************/
/*!
		Class Free3DMovement:
\brief	3D Movement for 2 joints
*/
/******************************************************************************/
class Free3DMovement
{
public:

	float turnAmount;
	bool canTurn;
	bool canChangeAlti;

	float Animate(float startAngle, float maxAngle, float speed, double dt, bool stopAtMaxMovement);

	Free3DMovement();
	~Free3DMovement();
};
	
#endif