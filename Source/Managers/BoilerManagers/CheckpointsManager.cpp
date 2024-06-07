#include "CheckpointsManager.h"
#include "Managers/DataLocatorManager.h"
#include "Managers/PlanetManager.h"
#include "Managers/LogicManager/LogicManager.h"
#include "Global/GlobalFunctions.h"
#include "Global/GlobalVariables.h"
#include "CameraControllers/SSPlayer/SSPlayerCameraController.h"
#include "Object/CollisionableObject/AnimatedObject/Player/Player.h"
#include "PlayerControllers/Sphere/SpherePlayerController.h"
#include "AnimationControllers/PlayerAnimation/PlayerAnimation.h"
#include <cassert>

CheckpointsManager * CheckpointsManager::checkpointsManager = NULL;

CheckpointsManager * CheckpointsManager::getCheckpointsManager() 
{
	if(checkpointsManager == NULL)
		checkpointsManager = new CheckpointsManager();

	return checkpointsManager;
}

CheckpointsManager::CheckpointsManager()
: currentParsed(0,0,0)
, currentParsedElementId(0)
, lastCheckPointAchievedId(0)
, initialCheckpoint(0)
{
	init();
}

void CheckpointsManager::init()
{
	DataLocatorManager *dataLocatorManager = DataLocatorManager::getDataLocatorManager();
	const std::string &pathToXml = dataLocatorManager->getPath("xml");

	std::string xmlFile = pathToXml + "/boiler/boiler_checkpoints.xml";
	bool ok = xmlParseFile(xmlFile);
	assert(ok);
}

void CheckpointsManager::onEndElement (const std::string &elem)
{
	if( elem == "check_point" )
	{
		(*this)[currentParsedElementId] = currentParsed;
	}
}

void CheckpointsManager::onStartElement (const std::string &elem, MKeyValue &atts) 
{
	if( elem == "check_point" )
	{
		const Planet *planet = PlanetManager::getPlanetManager()->getPlanet("boiler");
		const D3DXVECTOR3 &planetPos = planet->getPosition();

		currentParsedElementId = atts.getInt("id", 0);
		std::string position = atts.getString("position", "");
		float x = 0.0f;
		float y = 0.0f;
		float z = 0.0f;
		sscanf_s( position.c_str(),"%f %f %f", &x, &y, &z );

		D3DXVECTOR3 tempVector(x,y,z);
		tempVector = tempVector + planetPos;

		// position
		// The point of birth must to be over the surface of planet
		// Calculate the final position with height
		float alpha = 0.0f;
		float delta = 0.0f;
		planet->getPolarAngles(tempVector, alpha, delta);
		float height = planet->getHeight(alpha, delta);
		if(height==0.0f)
			height=planet->getRadius();

		assert( tempVector != planetPos );
		D3DXVECTOR3 localUp = tempVector - planetPos;
		Global::normalizeVector3(localUp, localUp);

		currentParsed = planetPos + height*localUp;
	}
}

void CheckpointsManager::playerCheckpointAchieved(int checkpointId)
{
	lastCheckPointAchievedId = checkpointId;
}

void CheckpointsManager::placePlayerLastCheckpoint()
{
	D3DXVECTOR3 checkpointPosition; 
	getCheckpointPos( checkpointPosition, lastCheckPointAchievedId );

	const Planet *planet = PlanetManager::getPlanetManager()->getPlanet("boiler");
	const D3DXVECTOR3 &center = planet->getCenter();

	assert( checkpointPosition != center );
	D3DXVECTOR3 playerUp = checkpointPosition - center;
	Global::normalizeVector3(playerUp, playerUp);

	D3DXVECTOR3 cameraPosition = checkpointPosition + playerUp * 27.0f;
	D3DXVECTOR3 cameraTarget = cameraPosition - playerUp * 4.0f;
	Global::camera->setUpVector( D3DXVECTOR3(0,1,0) );
	Global::camera->lookAt( cameraPosition, cameraTarget );

	D3DXVECTOR3 playerLeft;
	D3DXVECTOR3 fakeUp(0,1,0);
	D3DXVec3Cross(&playerLeft, &playerUp, &fakeUp);
	Global::normalizeVector3(playerLeft, playerLeft);

	D3DXVECTOR3 playerFront;
	D3DXVec3Cross(&playerFront, &playerLeft, &playerUp);

	D3DXMATRIX transformMatrix;
	D3DXMatrixIdentity( &transformMatrix );

	transformMatrix(0,0) = playerLeft.x;
	transformMatrix(0,1) = playerLeft.y;
	transformMatrix(0,2) = playerLeft.z;
	transformMatrix(1,0) = playerUp.x;
	transformMatrix(1,1) = playerUp.y;
	transformMatrix(1,2) = playerUp.z;
	transformMatrix(2,0) = playerFront.x;
	transformMatrix(2,1) = playerFront.y;
	transformMatrix(2,2) = playerFront.z;

	Player *player = Player::getPlayer();
	player->setTransformMatrix( transformMatrix );
	player->setPosition( checkpointPosition );

	// update alpha/delta
	float alpha = 0.0f;
	float delta = 0.0f;
	planet->getPolarAngles(checkpointPosition, alpha, delta);
	player->setAlpha( alpha );
	player->setDelta( delta );

	SpherePlayerController * playerController = new SpherePlayerController( D3DX_PI / 2 );
	player->setPlayerController( playerController );

	player->setRelationToState(CYCLE, "idle", true, true);
	player->updateAnimation();
	player->setRelationToState(ACTION, "burn", true, true);

	// Reset jump values
	IAnimationController *animationController = player->getAnimationController();
	if( animationController->getControllerType( ) == "player_animation" )
		((PlayerAnimation *)animationController)->resetJumpVariables( );
}

void CheckpointsManager::placePlayerInitialCheckpoint()
{
	lastCheckPointAchievedId = initialCheckpoint;
	placePlayerLastCheckpoint();
}

void CheckpointsManager::getCheckpointPos( D3DXVECTOR3 &out, int checkpointId )
{
	iterator it = find( checkpointId );
	if(it == end()) {
		assert( !"[getCheckpointPos]::Checkpoint Id not existing in CheckpointsManager" );
		return;
	}

	out = it->second;
}

void CheckpointsManager::reset()
{
	lastCheckPointAchievedId = 0;
}

void CheckpointsManager::cleanUp() 
{
	clear();	
}
