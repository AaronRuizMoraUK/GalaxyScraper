#include "SSPlayerCameraController.h"
#include "Object/CollisionableObject/AnimatedObject/Player/Player.h"
#include "Managers/PlanetManager.h"
//#include "Object/Planet/Planet.h"

std::string SSPlayerCameraController::controllerType = "ss_player_camera_controller";

/**
* Update the camera with a delta time like in a First Person Shooter
* with rotation
* 
* @param camera Camera
* @param deltaTime Delta time for update
*/
void SSPlayerCameraController::update(Camera &camera, float deltaTime) {

	// Get the last position and target of the camera
	const D3DXVECTOR3 &prevTarget = camera.getTarget();
	const D3DXVECTOR3 &prevPosition = camera.getPosition();

	// Obtain Player position
	Player *player = Player::getPlayer();
	const D3DXVECTOR3 &playerPosition = player->getPosition();
	const D3DXVECTOR3 &playerLocalUpVector = player->getLocalUpVector();

	// Obtain new target and position with inertia
	D3DXVECTOR3 deltaTarget = playerPosition-prevTarget;
	D3DXVECTOR3 deltaPosition = (playerPosition+distance*playerLocalUpVector)-prevPosition;


	// Intersection point between line and sphere
	/*const Planet * planet = PlanetManager::getPlanetManager()->getCurrentPlanet();
	const D3DXVECTOR3 &center = planet->getCenter();
	float radius = planet->getRadius();
	float t = 0;
	float denominator = pow(playerPosition.x-center.x, 2) + pow(playerPosition.y-center.y, 2) + pow(playerPosition.z-center.z, 2);
	if (denominator!=0)
		t = radius / sqrt(denominator);
	// New position (point in the sphere)
	D3DXVECTOR3 newPosition2 = playerPosition;
	newPosition2.x = center.x + t*(newPosition2.x-center.x);
	newPosition2.y = center.y + t*(newPosition2.y-center.y);
	newPosition2.z = center.z + t*(newPosition2.z-center.z);
	D3DXVECTOR3 deltaTarget = newPosition2-prevTarget;
	D3DXVECTOR3 deltaPosition = (newPosition2+(distance+1)*playerLocalUpVector)-prevPosition;*/


	D3DXVECTOR3 newTarget = prevTarget + deltaTarget*deltaTime*targetSpeed;
	D3DXVECTOR3 newPosition = prevPosition + deltaPosition*deltaTime*positionSpeed;

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

		/*
		if( Global::isPushed( '1' ) ) {
			static float amount = 0.2f;
			static float speed = 10.0f;

			if( Global::isPushed( '2' ) )
				amount += 0.002f;
			if( Global::isPushed( '3' ) )
				amount -= 0.002f;
			if( Global::isPushed( '4' ) )
				speed += 0.2f;
			if( Global::isPushed( '5' ) )
				speed -= 0.2f;

			if( amount < 0.0f )
				amount = 0.0f;
			if( speed < 0.0f )
				speed = 0.0f;

			float variation = amount*sin( speed*Global::time );
			newTarget += variation * ( camera.getLocalLeftVector() + camera.getLocalUpVector() + camera.getLocalFrontVector() );

			if( Global::isPushed( 'Q' ) ) {
				char msg[ 128 ];
				sprintf_s(msg, sizeof(msg), "amount %f - speed %f\n", amount, speed);
				OutputDebugString(msg);
			}
		}

		if( Global::isPushed( '6' ) ) {
			static float amount = 0.2f;
			static float speed = 10.0f;

			if( amount < 0.0f )
				amount = 0.0f;
			if( speed < 0.0f )
				speed = 0.0f;

			if( Global::isPushed( '7' ) )
				amount += 0.002f;
			if( Global::isPushed( '8' ) )
				amount -= 0.002f;
			if( Global::isPushed( '9' ) )
				speed += 0.1f;
			if( Global::isPushed( '0' ) )
				speed -= 0.1f;

			float variation = amount*sin( speed*Global::time );
			newPosition += camera.getLocalLeftVector()*variation;
		}
		*/
	}

	// Set new up vector for camera
	camera.setUpVector( camera.getLocalUpVector() );

	// Update where camera is looking at
	camera.lookAt(newPosition, newTarget);
}
