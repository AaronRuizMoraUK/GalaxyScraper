#ifndef RAILXZ_CAMERA_CONTROLLER_H
#define RAILXZ_CAMERA_CONTROLLER_H

#include "CameraControllers/ICameraController.h"

/**
* Camera Controller like a rail in XZ Axis
*/

class RailXZCameraController : public ICameraController {

protected:
	static std::string controllerType;

	// Speed for the camera controller 
	float speed;

public:
	RailXZCameraController () : speed(3.0f) {};
	~RailXZCameraController () {
		OutputDebugString("Destroying RailXZ camera controller...\n");
	};

	/**
	* Update the camera with a delta time like a rail in XZ Axis.
	* 
	* @param camera Camera
	* @param deltaTime Delta time for update
	*/
	void update(Camera &camera, float deltaTime=(1.0f/60.0f));

	const std::string & getControllerType () const { return controllerType; };
};

#endif //RAILXZ_CAMERA_CONTROLLER_H
