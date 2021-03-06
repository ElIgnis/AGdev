#include "TPCamera.h"
#include "Application.h"
#include "Mtx44.h"

#define CAM_TPS_HEIGHT_MAX 50.f
#define CAM_TPS_HEIGHT_MIN -30.f
#define CAM_SHOULDER_HEIGHT_MAX 50.f
#define CAM_SHOULDER_HEIGHT_MIN -30.f
#define CAM_ROTATION_SPEED_LIMIT 3.f
#define CAM_PITCH_SPEED_LIMIT 3.f
#define CAM_ZOOM_SPEED_LIMIT 15.f

// The default camera speed
static const float CAMERA_SPEED = 200.f;
const float m_fTPVCameraOffset = 30.00f;	   // Offset distance for the camera from the target

/********************************************************************************
Constructor
********************************************************************************/
TPCamera::TPCamera()
	: MoveVel_W(0)
	, MoveVel_S(0)
	, MoveVel_A(0)
	, MoveVel_D(0)
	, CAMERA_ACCEL(0)
	, aimMode(false)
	, normalView_Transit(true)
	, aimView_Transit(false)
	, sideView_Transit(false)
	, strafeLeft(0)
	, vSense(0.01f)
	, hSense(0.01f)
	, translateSpeed(500.f)
{
}

/********************************************************************************
Destructor
********************************************************************************/
TPCamera::~TPCamera()
{
}

/********************************************************************************
Initialise the camera
********************************************************************************/
void TPCamera::Init(const Vector3& pos, const Vector3& target, const Vector3& up)
{
	this->position = defaultPosition = pos;
	this->target = defaultTarget = target;
	Vector3 view = (target - position).Normalized();
	Vector3 right = view.Cross(up);
	right.y = 0;
	right.Normalize();
	this->up = defaultUp = right.Cross(view).Normalized();

	// Initialise the camera movement flags
	for (int i = 0; i<255; i++){
		myKeys[i] = false;
	}

	// Initialise the camera type
	sCameraType = LAND_CAM;

	// For Jump use
	m_bJumping = false;
	JumpVel = 0.0f;
	JUMPMAXSPEED = 10.0f;
	JUMPACCEL = 10.0f;
	GRAVITY = -30.0f;

	// Maximum movement speed
	CAMERA_ACCEL = 10.0f;
}

void TPCamera::Init(const Vector3& pos, const Vector3& target, const Vector3& up, bool lockPitch, bool lockYaw)
{
	this->position = defaultPosition = pos;
	this->target = defaultTarget = target;
	Vector3 view = (target - position).Normalized();
	Vector3 right = view.Cross(up);
	right.y = 0;
	right.Normalize();
	this->up = defaultUp = right.Cross(view).Normalized();

	// Initialise the camera movement flags
	for (int i = 0; i<255; i++){
		myKeys[i] = false;
	}

	// Initialise the camera type
	sCameraType = LAND_CAM;

	// For Jump use
	m_bJumping = false;
	JumpVel = 0.0f;
	JUMPMAXSPEED = 10.0f;
	JUMPACCEL = 10.0f;
	GRAVITY = -30.0f;

	// Maximum movement speed
	CAMERA_ACCEL = 10.0f;

	//Control pitch and yaw
	LockPitch = lockPitch;
	LockYaw = lockYaw;
}

/********************************************************************************
Update the camera
********************************************************************************/
void TPCamera::Update(double dt)
{
	// WASD movement
	if (myKeys['w'] == true)
	{
		Walk(dt);
	}
	else
	{
		MoveVel_W = 0.0f;
	}
	if (myKeys['s'] == true)
	{
		Walk(-dt);
	}
	else
	{
		MoveVel_S = 0.0f;
	}
	if (myKeys['a'] == true)
	{
		Strafe(-dt);
	}
	else
	{
		MoveVel_A = 0.0f;
	}
	if (myKeys['d'] == true)
	{
		Strafe(dt);
	}
	else
	{
		MoveVel_D = 0.0f;
	}

	// Rotation
	if (myKeys[VK_UP] == true)
	{
		LookUp(dt);
	}
	if (myKeys[VK_DOWN] == true)
	{
		LookUp(-dt);
	}
	if (myKeys[VK_LEFT] == true)
	{
		TurnLeft(-dt);
	}
	if (myKeys[VK_RIGHT] == true)
	{
		TurnRight(dt);
	}

	// Jump
	if (myKeys[32] == true)
	{
		Jump(dt);
		myKeys[32] = false;
	}
	UpdateJump(dt);

	//Update the camera direction based on mouse move
	// left-right rotate
	/*if (Application::camera_yaw != 0)
		Yaw(dt);
	if (Application::camera_pitch != 0)
		Pitch(dt);

	if (Application::IsKeyPressed('R'))
	{
		Reset();
	}*/
}

/********************************************************************************
Update the camera for third person view
Vector3 newPosition is the new position which the camera is to be based on
********************************************************************************/
void TPCamera::UpdatePosition(Vector3 newPos, Vector3 newDir, Vector3 ShoulderPos, double dt, bool isMoving)
{
	if (!LockPitch)
	{
		calcPitch();
	}
	if (!LockYaw)
	{
		calcRotation();
	}

	float theta = m_fTPVCameraAngle;
	float offSet_X = calcHDist() * sin(Math::DegreeToRadian(theta));
	float offSet_Z = calcHDist() * cos(Math::DegreeToRadian(theta));

	target.x = newPos.x + offSet_X * 2;
	target.y = newPos.y + 25.f - m_fTPVCameraPitch;
	target.z = newPos.z + offSet_Z * 2;

	//Update focused view
	if (aimMode)
	{
		normalView_Transit = false;
		//Set the height
		m_fTPVCameraOffsetY = 0.f;

		//Transition
		if (aimView_Transit == false)
		{
			Vector3 shoulderView;
			shoulderView.x = ShoulderPos.x - 15 * sin(Math::DegreeToRadian(theta + 35));
			shoulderView.y = ShoulderPos.y + (m_fTPVCameraPitch * 0.1f) + m_fTPVCameraOffsetY;
			shoulderView.z = ShoulderPos.z - 15 * cos(Math::DegreeToRadian(theta + 35));

			Vector3 direction = shoulderView - position;
			float distance = direction.LengthSquared();

			if ((translateSpeed * dt) * (translateSpeed * dt) < distance)
			{
				position += direction.Normalized() * (translateSpeed * dt);
			}
			else
			{
				aimView_Transit = true;
			}
		}
		else
		{
			position.x = ShoulderPos.x - 15 * sin(Math::DegreeToRadian(theta + 35));
			position.y = ShoulderPos.y + (m_fTPVCameraPitch * 0.1f) + m_fTPVCameraOffsetY;
			position.z = ShoulderPos.z - 15 * cos(Math::DegreeToRadian(theta + 35));
		}
	}
	//Update normal view
	else
	{
		aimView_Transit = false;
		//Set the height
		m_fTPVCameraOffsetY = 100.f;
		
		//Cam moves to player when not moving
		if (strafeLeft == 0)
		{
			strafe = 0;
			
			sideView_Transit = false;
			//Transition
			if (normalView_Transit == false)
			{
				Vector3 defaultView;
				defaultView.x = newPos.x - offSet_X;
				defaultView.y = newPos.y + calcVDist();
				defaultView.z = newPos.z - offSet_Z;

				Vector3 direction = defaultView - position;
				float distance = direction.LengthSquared();

				if ((translateSpeed * dt) * (translateSpeed * dt) < distance)
				{
					position += direction.Normalized() * (translateSpeed * dt);
				}
				else
				{
					normalView_Transit = true;
				}
			}
			else
			{
				position.x = newPos.x - offSet_X;
				position.y = newPos.y + calcVDist();
				position.z = newPos.z - offSet_Z;
			}
		}
		//Chracter slides towards direction
		else
		{
			if (strafeLeft == 1 && strafe < maxLimit)
				strafe += dt * 100;
			if (strafeLeft == 2 && strafe > -maxLimit)
				strafe -= dt * 100;

			normalView_Transit = false;
			//Transition
			if (sideView_Transit == false)	
			{
				Vector3 sideView;
				sideView.x = newPos.x - 45 * sin(Math::DegreeToRadian(theta + strafe));
				sideView.y = newPos.y + calcVDist();
				sideView.z = newPos.z - 45 * cos(Math::DegreeToRadian(theta + strafe));

				Vector3 direction = sideView - position;
				float distance = direction.LengthSquared();

				if ((translateSpeed * dt) * (translateSpeed * dt) < distance)
				{
					position += direction.Normalized() * (translateSpeed * dt);
					target += direction.Normalized() * (translateSpeed * dt);
				}
				else
				{
					sideView_Transit = true;
				}
			}
			else
			{
				position.x = newPos.x - 45 * sin(Math::DegreeToRadian(theta + strafe));
				position.y = newPos.y + calcVDist();
				position.z = newPos.z - 45 * cos(Math::DegreeToRadian(theta + strafe));
			}
		}
		
		if (position.y <= 0.f)
			position.y = 0.f;
	}
}

void TPCamera::TogglePitchLock(void)
{
	if (LockPitch == true)
	{
		this->LockPitch = false;
	}

	else
		this->LockPitch = true;
}
void TPCamera::ToggleYawLock(void)
{
	if (LockYaw == true)
	{
		this->LockYaw = false;
	}
	else
		this->LockYaw = true;
}

void TPCamera::calcZoom(void)
{
	float zoomLevel = Application::getMouse()->getDiffPos().y;
	
	if (zoomLevel > CAM_ZOOM_SPEED_LIMIT)
	{
		zoomLevel = CAM_ZOOM_SPEED_LIMIT;
	}
	else if (zoomLevel < -CAM_ZOOM_SPEED_LIMIT)
	{
		zoomLevel = -CAM_ZOOM_SPEED_LIMIT;
	}
	//Zoom in
	if (zoomLevel > 0.0 && m_fTPVCameraOffset > 50.f)
	{
		m_fTPVCameraOffsetX -= zoomLevel;
		m_fTPVCameraOffsetY -= zoomLevel;
	}
	//Zoom out
	else if (zoomLevel < 0.0 && m_fTPVCameraOffset < 200.f)
	{
		m_fTPVCameraOffsetX -= zoomLevel;
		m_fTPVCameraOffsetY -= zoomLevel;
	}
}

void TPCamera::calcPitch(void)
{
	//Pitch inertia
	//Speed limit
	//if (pitchChange < CAM_PITCH_SPEED_LIMIT && pitchChange > -CAM_PITCH_SPEED_LIMIT)
	pitchChange = Application::getMouse()->getDiffPos().y * vSense;
	//std::cout << pitchChange << std::endl;
	//Slowing down
	//if (pitchChange != 0.0f && Application::mouse_diff_y == 0.f)
	//	pitchChange -= pitchChange * 0.25f;

	//Prevent cam from going too low
	if (Application::getMouse()->getMousePitch() > 0.0)
	{
		m_fTPVCameraPitch += pitchChange;
		if (aimMode)
		{
			if (m_fTPVCameraPitch > CAM_SHOULDER_HEIGHT_MAX)
				m_fTPVCameraPitch = CAM_SHOULDER_HEIGHT_MAX;
		}
		else
		{

			if (m_fTPVCameraPitch > CAM_TPS_HEIGHT_MAX)
				m_fTPVCameraPitch = CAM_TPS_HEIGHT_MAX;
		}
	}
	else if (Application::getMouse()->getMousePitch() < 0.0)
	{
		m_fTPVCameraPitch += pitchChange;
		
		if (aimMode)
		{
			if (m_fTPVCameraPitch < CAM_SHOULDER_HEIGHT_MIN)
				m_fTPVCameraPitch = CAM_SHOULDER_HEIGHT_MIN;
		}
		else
		{
			if (m_fTPVCameraPitch < CAM_TPS_HEIGHT_MIN)
				m_fTPVCameraPitch = CAM_TPS_HEIGHT_MIN;
		}
	}
}

void TPCamera::calcRotation(void)
{
	//Rotation inertia
	//Speed limit
	if (angleChange < CAM_ROTATION_SPEED_LIMIT && angleChange > -CAM_ROTATION_SPEED_LIMIT)
		angleChange += Application::getMouse()->getDiffPos().x * hSense;

	//Slowing down
	if (angleChange != 0.0f && Application::getMouse()->getDiffPos().x == 0.f)
		angleChange -= angleChange * 0.75f;

	m_fTPVCameraAngle -= angleChange;
	//360/0 limiter
	if (m_fTPVCameraAngle > 360.f)
		m_fTPVCameraAngle = 0.f;
	else if (m_fTPVCameraAngle < 0.f)
		m_fTPVCameraAngle = 360.f;
}

float TPCamera::calcHDist(void)
{
	return (float)(m_fTPVCameraOffsetX * cos(Math::DegreeToRadian(m_fTPVCameraPitch)));
}

float TPCamera::calcVDist(void)
{
	return (float)(m_fTPVCameraOffsetY * sin(Math::DegreeToRadian(m_fTPVCameraPitch)));
}

float TPCamera::GetCamAngle(void)
{
	return m_fTPVCameraAngle;
}

void TPCamera::SetAimMode(bool newAimMode)
{
	this->aimMode = newAimMode;
}

bool TPCamera::GetAimMode(void)
{
	return aimMode;
}

void TPCamera::ToggleAimMode(void)
{
	
}

float TPCamera::getCamPitch(void)
{
	return m_fTPVCameraPitch;
}

void TPCamera::SetVSense(float newVSense)
{
	this->vSense = newVSense;
}
void TPCamera::AddVSense(void)
{
	this->vSense += 0.01f;
}
void TPCamera::MinusVSense(void)
{
	this->vSense -= 0.01f;
}
void TPCamera::SetHSense(float newHSense)
{
	this->hSense = newHSense;
}
void TPCamera::AddHSense(void)
{
	this->hSense += 0.01f;
}
void TPCamera::MinusHSense(void)
{
	this->hSense -= 0.01f;
}
float TPCamera::GetHSense(void)
{
	return hSense;
}
float TPCamera::GetVSense(void)
{
	return vSense;
}

void TPCamera::SetStrafeLeft(int strafe)
{
	this->strafeLeft = strafe;
}

/********************************************************************************
Update the camera status
********************************************************************************/
void TPCamera::UpdateStatus(const unsigned char key, const bool status)
{
	myKeys[key] = status;
}

/********************************************************************************
Reset the camera to default settings
********************************************************************************/
void TPCamera::Reset()
{
	position = defaultPosition;
	target = defaultTarget;
	up = defaultUp;
}

/********************************************************************************
Move the camera forward
********************************************************************************/
void TPCamera::MoveForward(const double dt)
{
	// Calculate the direction vector of the camera
	Vector3 view = (target - position).Normalized();

	// Constrain the movement to the ground if the camera type is land based
	if (sCameraType == LAND_CAM)
	{
		view.y = 0.0f;
		view = view.Normalized();
	}

	MoveVel_W = MoveVel_W + CAMERA_ACCEL * (float)dt;
	if (MoveVel_W > CAMERA_SPEED)
		MoveVel_W = CAMERA_SPEED;

	// Update the camera and target position
	position += view * MoveVel_W;
	target += view * MoveVel_W;
}
/********************************************************************************
Move the camera backward
********************************************************************************/
void TPCamera::MoveBackward(const double dt)
{
	Vector3 view = (target - position).Normalized();

	// Constrain the movement to the ground if the camera type is land based
	if (sCameraType == LAND_CAM)
	{
		view.y = 0.0f;
		view = view.Normalized();
	}

	MoveVel_S = MoveVel_S + CAMERA_ACCEL * (float)dt;
	if (MoveVel_S < -CAMERA_SPEED)
		MoveVel_S = -CAMERA_SPEED;

	// Update the camera and target position
	position -= view * MoveVel_S;
	target -= view * MoveVel_S;

}
/********************************************************************************
Move the camera left
********************************************************************************/
void TPCamera::MoveLeft(const double dt)
{
	Vector3 view = (target - position).Normalized();
	Vector3 right = view.Cross(up);
	right.y = 0;
	right.Normalize();

	MoveVel_A = MoveVel_A + CAMERA_ACCEL * (float)dt;
	if (MoveVel_A < -CAMERA_SPEED)
		MoveVel_A = -CAMERA_SPEED;

	// Update the camera and target position
	position -= right * MoveVel_A;
	target -= right * MoveVel_A;
}
/********************************************************************************
Move the camera right
********************************************************************************/
void TPCamera::MoveRight(const double dt)
{
	Vector3 view = (target - position).Normalized();
	Vector3 right = view.Cross(up);
	right.y = 0;
	right.Normalize();

	MoveVel_D = MoveVel_D + CAMERA_ACCEL * (float)dt;
	if (MoveVel_D > CAMERA_SPEED)
		MoveVel_D = CAMERA_SPEED;

	// Update the camera and target position
	position += right * MoveVel_D;
	target += right * MoveVel_D;
}

/********************************************************************************
Turn left
********************************************************************************/
void TPCamera::TurnLeft(const double dt)
{
	Vector3 view = (target - position).Normalized();
	//float yaw = (float)(-CAMERA_SPEED * Application::camera_yaw * (float)dt);
	float yaw = (float)(-CAMERA_SPEED * (float)dt);
	Mtx44 rotation;
	rotation.SetToRotation(yaw, 0, 1, 0);
	view = rotation * view;
	target = position + view;
	Vector3 right = view.Cross(up);
	right.y = 0;
	right.Normalize();
	up = right.Cross(view).Normalized();
}
/********************************************************************************
Turn right
********************************************************************************/
void TPCamera::TurnRight(const double dt)
{
	Vector3 view = (target - position).Normalized();
	//	float yaw = (float)(-CAMERA_SPEED * Application::camera_yaw * (float)dt);
	float yaw = (float)(-CAMERA_SPEED * (float)dt);
	Mtx44 rotation;
	rotation.SetToRotation(yaw, 0, 1, 0);
	view = rotation * view;
	target = position + view;
	Vector3 right = view.Cross(up);
	right.y = 0;
	right.Normalize();
	up = right.Cross(view).Normalized();
}
/********************************************************************************
LookUp
********************************************************************************/
void TPCamera::LookUp(const double dt)
{
	//float pitch = (float)(-CAMERA_SPEED * Application::camera_pitch * (float)dt);
	float pitch = (float)(CAMERA_SPEED * (float)dt);
	Vector3 view = (target - position).Normalized();
	Vector3 right = view.Cross(up);
	right.y = 0;
	right.Normalize();
	up = right.Cross(view).Normalized();
	Mtx44 rotation;
	rotation.SetToRotation(pitch, right.x, right.y, right.z);
	view = rotation * view;
	target = position + view;
}
/********************************************************************************
LookDown
********************************************************************************/
void TPCamera::LookDown(const double dt)
{
	//float pitch = (float)(-CAMERA_SPEED * Application::camera_pitch * (float)dt);
	float pitch = (float)(CAMERA_SPEED * (float)dt);
	Vector3 view = (target - position).Normalized();
	Vector3 right = view.Cross(up);
	right.y = 0;
	right.Normalize();
	up = right.Cross(view).Normalized();
	Mtx44 rotation;
	rotation.SetToRotation(pitch, right.x, right.y, right.z);
	view = rotation * view;
	target = position + view;
}
/********************************************************************************
Spin Clockwise
********************************************************************************/
void TPCamera::SpinClockWise(const double dt)
{
}
/********************************************************************************
Spin CounterClockwise
********************************************************************************/
void TPCamera::SpinCounterClockWise(const double dt)
{
}
/********************************************************************************
Set Camera Type
********************************************************************************/
void TPCamera::SetCameraType(CAM_TYPE sCameraType)
{
	this->sCameraType = sCameraType;
}
/********************************************************************************
Get Camera Type
********************************************************************************/
TPCamera::CAM_TYPE TPCamera::GetCameraType(void)
{
	return sCameraType;
}

/********************************************************************************
Pitch. You can add in a deadzone here.
********************************************************************************/
void TPCamera::Pitch(const double dt)
{
	/*if (Application::camera_pitch > 0.0)
		LookUp(-Application::camera_pitch * dt);
	else if (Application::camera_pitch < 0.0)
		LookDown(-Application::camera_pitch * dt);*/
}
/********************************************************************************
Yaw. You can add in a deadzone here.
********************************************************************************/
void TPCamera::Yaw(const double dt)
{
	/*if (Application::camera_yaw > 0.0)
		TurnRight(Application::camera_yaw * dt);
	else if (Application::camera_yaw < 0.0)
		TurnLeft(Application::camera_yaw * dt);*/


}
/********************************************************************************
Roll your camera
********************************************************************************/
void TPCamera::Roll(const double dt)
{
}
/********************************************************************************
Walk forward or backward. You can add in a deadzone here.
********************************************************************************/
void TPCamera::Walk(const double dt)
{
	if (dt > 0)
		MoveForward(dt);
	else if (dt < 0)
		MoveBackward(abs(dt));
}
/********************************************************************************
Strafe left or right. You can add in a deadzone here.
********************************************************************************/
void TPCamera::Strafe(const double dt)
{
	if (dt > 0)
		MoveRight(dt);
	else if (dt < 0)
		MoveLeft(abs(dt));
}
/********************************************************************************
Jump
********************************************************************************/
void TPCamera::Jump(const double dt)
{
	if (m_bJumping == false)
	{
		m_bJumping = true;

		// Calculate the jump velocity
		JumpVel = JUMPACCEL;// * dt;

		// Factor in maximum speed limit
		if (JumpVel > JUMPMAXSPEED)
			JumpVel = JUMPMAXSPEED;
	}
}

/********************************************************************************
Update Jump
********************************************************************************/
void TPCamera::UpdateJump(const double dt)
{
	if (m_bJumping == true)
	{
		// Factor in gravity
		JumpVel += GRAVITY * (float)dt;

		// Update the camera and target position
		position.y += JumpVel * (float)dt;
		target.y += JumpVel * (float)dt;

		// Check if the camera has reached the ground
		if (position.y <= 0)
		{
			position.y = 0;
			target.y = 0;
			JumpVel = 0.0f;
			m_bJumping = false;
		}
	}
}