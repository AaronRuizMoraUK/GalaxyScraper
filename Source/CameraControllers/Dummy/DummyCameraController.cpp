#include "DummyCameraController.h"
#include "Global/GlobalVariables.h"
#include "Managers/PlanetManager.h"

std::string DummyCameraController::controllerType = "dummy_camera_controller";

void DummyCameraController::update(Camera &camera, float deltaTime) {

	// Get the last position and target of the camera
	const D3DXVECTOR3 &prevTarget = camera.getTarget();
	const D3DXVECTOR3 &prevPosition = camera.getPosition();

	D3DXVECTOR3 newTarget = prevTarget;
	D3DXVECTOR3 newPosition = prevPosition;

	// Camera Vibration
	if( Global::cameraVibrationEnabled ) {

		//static const float amount = 0.052f;
		const std::string &planetName = PlanetManager::getPlanetManager()->getCurrentPlanetName();
		static float amount = 0.1f;
		if( planetName == "iuranus" )
			amount = 0.07f;
		else if( planetName == "boiler" )
			amount = 0.23f;
		static const float speed = 84.0f;
		float variation = amount*sin( speed*Global::time );
		newTarget += variation * ( camera.getLocalLeftVector() + camera.getLocalUpVector() +
					 camera.getLocalFrontVector() );
	}

	// Set new up vector for camera
	camera.setUpVector( camera.getLocalUpVector() );

	// Update where camera is looking at
	camera.lookAt(newPosition, newTarget);
}
