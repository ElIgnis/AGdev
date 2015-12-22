#ifndef TP_CAMERA_H
#define TP_CAMERA_H

#include "Camera.h"

class TPCamera : public Camera
{
public:
	Vector3 defaultPosition;
	Vector3 defaultTarget;
	Vector3 defaultUp;

	enum CAM_TYPE {
		LAND_CAM,
		AIR_CAM,
		NUM_CAM_TYPE
	};

	TPCamera();
	~TPCamera();
	virtual void Init(const Vector3& pos, const Vector3& target, const Vector3& up);
	void Init(const Vector3& pos, const Vector3& target, const Vector3& up, const bool lockPitch, const bool lockYaw);
	virtual void Update(double dt);
	// For third person camera
	
	virtual void UpdatePosition(Vector3 position, Vector3 newDirection, Vector3 ShoulderPos, double dt, bool isMoving);
	// Update Camera status
	virtual void UpdateStatus(const unsigned char key, const bool status = true);
	virtual void Reset();

	// Basic methods
	virtual void MoveForward(const double dt);
	virtual void MoveBackward(const double dt);
	virtual void MoveLeft(const double dt);
	virtual void MoveRight(const double dt);
	virtual void TurnLeft(const double dt);
	virtual void TurnRight(const double dt);
	virtual void LookUp(const double dt);
	virtual void LookDown(const double dt);
	virtual void SpinClockWise(const double dt);
	virtual void SpinCounterClockWise(const double dt);

	virtual void SetCameraType(CAM_TYPE sCameraType);
	virtual CAM_TYPE GetCameraType(void);

	// Applied methods
	virtual void Pitch(const double dt);
	virtual void Yaw(const double dt);
	virtual void Roll(const double dt);
	virtual void Walk(const double dt);
	virtual void Strafe(const double dt);
	virtual void Jump(const double dt);
	virtual void UpdateJump(const double dt);
	float MoveVel_W;
	float MoveVel_S;
	float MoveVel_A;
	float MoveVel_D;

	void calcZoom(void);
	void calcPitch(void);
	void calcZoomPitch(void);
	void calcRotation(void);

	float calcHDist(void);
	float calcVDist(void);
	float getCamPitch(void);
	float GetCamAngle(void);

	void TogglePitchLock(void);
	void ToggleYawLock(void);

	void SetAimMode(bool newAimMode);
	bool GetAimMode(void);
	void ToggleAimMode(void);

	void AddVSense(void);
	void MinusVSense(void);
	void SetVSense(float newVSense);

	void AddHSense(void);
	void MinusHSense(void);
	void SetHSense(float newHSense);

	float GetHSense(void);
	float GetVSense(void);

	void SetStrafeLeft(int strafe);

private:
	bool myKeys[255];

	CAM_TYPE sCameraType;

	// For Jump use
	bool m_bJumping;
	float GRAVITY;
	float JumpVel;
	float JUMPMAXSPEED, JUMPACCEL;

	// Maximum movement speed and acceleration
	float CAMERA_ACCEL;

	//Offset distance for the camera from the target
	float m_fTPVCameraOffsetX = 50.0f;
	float m_fTPVCameraOffsetY = 50.0f;
	float m_fTPVCameraAngle = 0.f;
	float m_fTPVCameraPitch = 45.f;
	
	float translateSpeed;
	float vSense;
	float hSense;
	float pitchChange;
	float angleChange;

	bool LockPitch;
	bool LockYaw;
	bool aimMode;
	bool aimView_Transit;
	bool sideView_Transit;
	bool normalView_Transit;
	int strafeLeft;
	float maxLimit = 35;
	float strafe;
};

#endif