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
#include "Free3DMovement.h"
/******************************************************************************/
/*!
\brief
Default Constructor.
*/
/******************************************************************************/
Free3DMovement::Free3DMovement()
: turnAmount(0.f)
, canTurn(true)
, canChangeAlti(true)
{
}
Free3DMovement::~Free3DMovement()
{
}

float Free3DMovement::Animate(float startAngle, float maxAngle, float speed, double dt, bool stopAtMaxMovement)
{
	if (maxAngle > 0)
	{
		//Clockwise
		if (startAngle <= maxAngle && canTurn == true)
		{
			startAngle += (float)(speed * dt);
		}
		else
		{
			canTurn = false;
		}
		//Anti Clockwise
		if (canTurn == false && stopAtMaxMovement == false)
		{
			startAngle -= (float)(speed * dt);
			if (startAngle <= -maxAngle)
			{
				canTurn = true;
			}
		}
	}
	else if (maxAngle < 0)
	{
		//Clockwise
		if (startAngle >= maxAngle && canTurn == true)
		{
			startAngle -= (float)(speed * dt);
		}
		else
		{
			canTurn = false;
		}
		//Anti Clockwise
		if (canTurn == false && stopAtMaxMovement == false)
		{
			startAngle += (float)(speed * dt);
			if (startAngle >= -maxAngle)
			{
				canTurn = true;
			}
		}
	}
	else if (maxAngle == 0)
	{
		canTurn = true;
		if (startAngle < 0)
		{
			//Zero rotation/revert
			if (startAngle + (float)(speed * dt) >= 0)
			{
				return 0.f;
			}
			else
			{
				startAngle += (float)(speed * dt);
			}
		}
		else if (startAngle > 0)
		{
			//Zero rotation/revert
			if (startAngle - (float)(speed * dt) <= 0)
			{
				return 0.f;
			}
			else
			{
				startAngle -= (float)(speed * dt);
			}
		}
		else
		{
			return 0.f;
		}
	}
	return startAngle;
}