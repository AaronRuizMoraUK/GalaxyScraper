#ifndef FOLLOW_PLAYER_CAMERA_CONTROLLER_H
#define FOLLOW_PLAYER_CAMERA_CONTROLLER_H

#include "CameraControllers/ICameraController.h"

/**
* Camera Controller for a spherical scene
*/

// TODO by XML
// - targetSpeed
// - positionSpeed
// - distance

class FollowPlayerCameraController : public ICameraController {

protected:
	static std::string controllerType;

	// Speeds for the camera controller 
	float targetSpeed;
	float positionSpeed;

	// Distance between camera and player
	float distance;

public:
	FollowPlayerCameraController () : targetSpeed(35.0f), positionSpeed(35.0f), distance(15.0f) {};
	~FollowPlayerCameraController () {
		OutputDebugString("Destroying FollowPlayer camera controller...\n");
	};

	void update(Camera &camera, float deltaTime=(1.0f/60.0f));

	float getTargetSpeed() const { return targetSpeed; };
	float getPositionSpeed() const { return positionSpeed; };
	float getDistance() const { return distance; };

	void setTargetSpeed( float pTargetSpeed ) { targetSpeed = pTargetSpeed; };
	void setPositionSpeed( float pPositionSpeed ) { positionSpeed = pPositionSpeed; };
	void setDistance( float pDistance ) { distance = pDistance; };

	const std::string & getControllerType () const { return controllerType; };
};

#endif //FOLLOW_PLAYER_CAMERA_CONTROLLER_H
