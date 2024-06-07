#include "RailXZCameraController.h"
#include "Global/GlobalFunctions.h"

std::string RailXZCameraController::controllerType = "rail_xz_camera_controller";

/**
* Update the camera with a delta time like a rail in XZ Axis.
* 
* @param camera Camera
* @param deltaTime Delta time for update
*/
void RailXZCameraController::update(Camera &camera, float deltaTime) {

	// Get the last position and target of the camera
	const D3DXVECTOR3 &prevPosition = camera.getPosition();
	const D3DXVECTOR3 &prevTarget = camera.getTarget();

	// Delta vector
	D3DXVECTOR3 delta(0, 0, 0);

	// Check Controllers
	if (Global::isPushed('W'))
		delta += D3DXVECTOR3(-1,0,0);
	if (Global::isPushed('S'))
		delta += D3DXVECTOR3(1,0,0);
	if (Global::isPushed('A'))
		delta += D3DXVECTOR3(0,0,1);
	if (Global::isPushed('D'))
		delta += D3DXVECTOR3(0,0,-1);

	// Update delta vector
	delta *= speed*deltaTime;

	// Update where camera is looking at
	camera.lookAt(prevPosition+delta, prevTarget+delta);
}
