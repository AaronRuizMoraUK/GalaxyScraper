#include "FPSRotationCameraController.h"
#include "Global/GlobalFunctions.h"

std::string FPSRotationCameraController::controllerType = "fps_rotation_camera_controller";

/**
* Update the camera with a delta time like in a First Person Shooter
* with rotation
* 
* @param camera Camera
* @param deltaTime Delta time for update
*/
void FPSRotationCameraController::update(Camera &camera, float deltaTime) {

	// Get the last position and target of the camera
	const D3DXVECTOR3 &prevPosition = camera.getPosition();
	const D3DXVECTOR3 &prevTarget = camera.getTarget();

	// Get the last camera local front vector
	const D3DXVECTOR3 &prevLocalFrontVector = camera.getLocalFrontVector();
	D3DXVECTOR3 newLocalFrontVector = prevLocalFrontVector;

	// Delta vector
	D3DXVECTOR3 delta(0, 0, 0);

	// Check Controllers
	if (Global::isPushed('U'))
		delta += camera.getLocalFrontVector();
	if (Global::isPushed('J'))
		delta += -camera.getLocalFrontVector();
	if (Global::isPushed('H'))
		delta += camera.getLocalLeftVector();
	if (Global::isPushed('K'))
		delta += -camera.getLocalLeftVector();
	if (Global::isPushed('O'))
		delta += D3DXVECTOR3(0,1,0);
	if (Global::isPushed('L'))
		delta += D3DXVECTOR3(0,-1,0);
	if (Global::isPushed('Y')) {
		// Rotation Matrix
		D3DXMATRIX matRotation;
		// Angle in radians of the rotation
		float radians = rotationSpeed * deltaTime;
		// Get the matrix rotation and applies to the previous local front vector
		D3DXMatrixRotationY(&matRotation, radians);
		D3DXVec3TransformNormal(&newLocalFrontVector, &newLocalFrontVector, &matRotation);
	}
	if (Global::isPushed('I')) {
		// Rotation Matrix
		D3DXMATRIX matRotation;
		// Angle in radians of the rotation
		float radians = -(rotationSpeed * deltaTime);
		// Get the matrix rotation and applies to the previous local front vector
		D3DXMatrixRotationY(&matRotation, radians);
		D3DXVec3TransformNormal(&newLocalFrontVector, &newLocalFrontVector, &matRotation);
	}
	if (Global::isPushed('N')) {
		float prevYaw, prevPitch;
		Global::vectorToAngles(newLocalFrontVector, prevYaw, prevPitch);

		if(prevPitch > -D3DX_PI/2+0.01) {
			float radians = -(rotationSpeed * deltaTime);
			float newPitch = prevPitch+radians;
			Global::anglesToVector(prevYaw, newPitch, newLocalFrontVector);
		}
	}
	if (Global::isPushed('M')) {
		float prevYaw, prevPitch;
		Global::vectorToAngles(newLocalFrontVector, prevYaw, prevPitch);

		if(prevPitch < D3DX_PI/2-0.01) {
			float radians = rotationSpeed * deltaTime;
			float newPitch = prevPitch+radians;
			Global::anglesToVector(prevYaw, newPitch, newLocalFrontVector);
		}
	}

	// Update delta vector
	delta *= speed*deltaTime;

	// Set the up vector of the camera
	camera.setUpVector(D3DXVECTOR3(0,1,0));

	// Update where camera is looking at
	// NOTE: prevTarget is equal to prevPosition+prevLocalFrontVector
	camera.lookAt(prevPosition+delta, prevPosition+newLocalFrontVector+delta);
}
