#ifndef FPS_ROTATION_CAMERA_CONTROLLER_H
#define FPS_ROTATION_CAMERA_CONTROLLER_H

#include "CameraControllers/ICameraController.h"

/**
* Camera Controller like in a First Person Shooter with rotation
*/

class FPSRotationCameraController : public ICameraController {

protected:
	static std::string controllerType;

	// Speed for the camera controller
	float speed;

	// Speed for the rotation of the camera controller
	float rotationSpeed;

public:
	FPSRotationCameraController () : speed(10.0f), rotationSpeed(1.0f) {};
	~FPSRotationCameraController () {
		OutputDebugString("Destroying FPSRotation camera controller...\n");
	};

	/**
	* Update the camera with a delta time like in a First Person Shooter
	* with rotation
	* 
	* @param camera Camera
	* @param deltaTime Delta time for update
	*/
	void update(Camera &camera, float deltaTime=(1.0f/60.0f));

	const std::string & getControllerType () const { return controllerType; };
};

#endif //FPS_ROTATION_CAMERA_CONTROLLER_H