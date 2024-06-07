#ifndef DUMMY_CAMERA_CONTROLLER_H
#define DUMMY_CAMERA_CONTROLLER_H

#include "CameraControllers/ICameraController.h"

/**
* Dummy Camera Controller
*/
class DummyCameraController : public ICameraController {
protected:
	static std::string controllerType;

public:
	DummyCameraController () {};
	~DummyCameraController () {
		OutputDebugString("Destroying DummyCameraController camera controller...\n");
	};

	void update(Camera &camera, float deltaTime=(1.0f/60.0f));

	const std::string & getControllerType () const { return controllerType; };
};

#endif //DUMMY_CAMERA_CONTROLLER_H
