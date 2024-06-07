#include "FollowPlayerCameraController.h"
#include "Object/CollisionableObject/AnimatedObject/Player/Player.h"
//#include "Managers/PlanetManager.h"
//#include "Object/Planet/Planet.h"

std::string FollowPlayerCameraController::controllerType = "follow_player_camera_controller";

/**
* Update the camera with a delta time like in a First Person Shooter
* with rotation
* 
* @param camera Camera
* @param deltaTime Delta time for update
*/
void FollowPlayerCameraController::update(Camera &camera, float deltaTime) {

	// Get the last position and target of the camera
	const D3DXVECTOR3 &prevTarget = camera.getTarget();
	const D3DXVECTOR3 &prevPosition = camera.getPosition();

	// Obtain Player position
	Player *player = Player::getPlayer();
	const D3DXVECTOR3 &playerPosition = player->getPosition();
	//const D3DXVECTOR3 &playerLocalFrontVector = player->getLocalFrontVector();

	/*
	static float fara = 0.0f;
	fara += deltaTime * 2.0f;
	float fara2 = distance + 5.0f * sin( fara );
	*/

	// Obtain new target and position with inertia
	D3DXVECTOR3 deltaTarget = playerPosition-prevTarget;
	//D3DXVECTOR3 deltaPosition = (playerPosition-distance*playerLocalFrontVector)-prevPosition;

	D3DXVECTOR3 newTarget = prevTarget + deltaTarget*deltaTime*targetSpeed;
	//D3DXVECTOR3 newPosition = prevPosition + deltaPosition*deltaTime*positionSpeed;

	// Set new up vector for camera
	//camera.setUpVector( camera.getLocalUpVector() );
	camera.setUpVector( D3DXVECTOR3( 0.0f, 1.0f, 0.0f )  );

	// Update where camera is looking at
	camera.lookAt(camera.getPosition(), newTarget);

	/*
	const D3DXMATRIX &matView = camera.getMatView( );
	static float rotation = 0.0f;
	rotation += deltaTime * 0.5f;
	D3DXMATRIX newMatView;
	D3DXMatrixRotationAxis( &newMatView, &playerLocalFrontVector, rotation );
	D3DXMatrixMultiply(&newMatView, &matView, &newMatView);
	camera.setMatView( newMatView );
	*/
}
