#ifndef FPS_CAMERA_CONTROLLER_H
#define FPS_CAMERA_CONTROLLER_H

#include "CameraControllers/ICameraController.h"

/**
* Camera Controller like in a First Person Shooter
*/

class FPSCameraController : public ICameraController {

protected:
	static std::string controllerType;

	// Speed for the camera controller
	float speed;

public:
	FPSCameraController () : speed(3.0f) {};
	~FPSCameraController () {
		OutputDebugString("Destroying FPS camera controller...\n");
	};

	/**
	* Update the camera with a delta time like in a First Person Shooter
	* 
	* @param camera Camera
	* @param deltaTime Delta time for update
	*/
	void update(Camera &camera, float deltaTime=(1.0f/60.0f));

	const std::string & getControllerType () const { return controllerType; };

};

#endif //FPS_CAMERA_CONTROLLER_H
