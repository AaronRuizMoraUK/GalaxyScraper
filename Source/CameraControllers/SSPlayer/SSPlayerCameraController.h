#ifndef SS_PLAYER_CAMERA_CONTROLLER_H
#define SS_PLAYER_CAMERA_CONTROLLER_H

#include "CameraControllers/ICameraController.h"

/**
* Camera Controller for a spherical scene
*/

// TODO by XML
// - targetSpeed
// - positionSpeed
// - distance

class SSPlayerCameraController : public ICameraController {

protected:
	static std::string controllerType;

	// Speeds for the camera controller 
	float targetSpeed;
	float positionSpeed;

	// Distance between camera and player
	float distance;

public:
	SSPlayerCameraController () : targetSpeed(12.0f), positionSpeed(1.8f), distance(16.0f) {};
	SSPlayerCameraController (float pTargetSpeed, float pPositionSpeed, float pDistance)
		: targetSpeed(pTargetSpeed), positionSpeed(pPositionSpeed), distance(pDistance) {};
	~SSPlayerCameraController () {
		OutputDebugString("Destroying SSPlayer camera controller...\n");
	};

	/**
	* Update the camera with a delta time like in a First Person Shooter
	* with rotation
	* 
	* @param camera Camera
	* @param deltaTime Delta time for update
	*/
	void update(Camera &camera, float deltaTime=(1.0f/60.0f));

	float getTargetSpeed() const { return targetSpeed; };
	float getPositionSpeed() const { return positionSpeed; };
	float getDistance() const { return distance; };

	void setTargetSpeed( float pTargetSpeed ) { targetSpeed = pTargetSpeed; };
	void setPositionSpeed( float pPositionSpeed ) { positionSpeed = pPositionSpeed; };
	void setDistance( float pDistance ) { distance = pDistance; };

	const std::string & getControllerType () const { return controllerType; };

};

#endif //SS_PLAYER_CAMERA_CONTROLLER_H
