#include "FPSCameraController.h"
#include "Global/GlobalFunctions.h"

std::string FPSCameraController::controllerType = "fps_camera_controller";

/**
* Update the camera with a delta time like in a First Person Shooter.
* 
* @param camera Camera
* @param deltaTime Delta time for update
*/
void FPSCameraController::update(Camera &camera, float deltaTime) {

	// Get the last position and target of the camera
	const D3DXVECTOR3 &prevPosition = camera.getPosition();
	const D3DXVECTOR3 &prevTarget = camera.getTarget();

	// Delta vector
	D3DXVECTOR3 delta(0, 0, 0);

	// Check Controllers
	if (Global::isPushed('W'))
		delta += camera.getLocalFrontVector();
	if (Global::isPushed('S'))
		delta += -camera.getLocalFrontVector();
	if (Global::isPushed('A'))
		delta += camera.getLocalLeftVector();
	if (Global::isPushed('D'))
		delta += -camera.getLocalLeftVector();

	// Update delta vector
	delta *= speed*deltaTime;

	// Update where camera is looking at
	camera.lookAt(prevPosition+delta, prevTarget+delta);
}
