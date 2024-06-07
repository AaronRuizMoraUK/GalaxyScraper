#include "BoilerCollisionManager.h"
#include "Object/CollisionableObject/AnimatedObject/Player/Player.h"
#include "Managers/BoilerManagers/BoilerFireballManager.h"
#include "Managers/BoilerManagers/LeversManager.h"
#include "Managers/BoilerManagers/CanonsManager.h"
#include "Managers/BoilerManagers/BoilerStaticCollionsManager.h"
#include "Managers/BoilerManagers/BoilerAnimatedObjectManager.h"
#include "Managers/StaticObjectManager.h"

BoilerCollisionManager * BoilerCollisionManager::boilerCollisionManager = NULL;

BoilerCollisionManager * BoilerCollisionManager::getBoilerCollisionManager() 
{
	if(boilerCollisionManager == NULL)
		boilerCollisionManager = new BoilerCollisionManager();

	return boilerCollisionManager;
}

BoilerCollisionManager::BoilerCollisionManager()
: CollisionManager()
, fireballCollisionDetected( false )
{
}

void BoilerCollisionManager::detectCollisions() {
	detectCollisionsPlayerFireballs();

	if( !fireballCollisionDetected )
		detectCollisionsPlayerSolids();
}

void BoilerCollisionManager::detectCollisionsPlayerFireballs() {

	const FireballVector &fireballVector = BoilerFireballManager::getBoilerFireballManager()->getCollisionableBalls();

	FireballVector::const_iterator it = fireballVector.begin();
	while( it != fireballVector.end() ) {
		const CollisionableObject *object = *it;

		D3DXVECTOR3 point;
		DT_Bool existCollision = DT_GetCommonPoint( Player::getPlayer()->getCollisionObject(), object->getCollisionObject(), point );

		if( existCollision ) {
			/*
			char msg[ 128 ];
			sprintf_s(msg, sizeof(msg), "Collision Player-Fireball: (%f,%f,%f) \n", point.x, point.y, point.z);
			OutputDebugString(msg);

			Global::printCollisionableObjectPosition( object );
			Global::printCollisionableObjectPosition( Player::getPlayer() );
			*/

			fireballCollisionDetected = true;
			return;
		}

		++it;
	}
}

void BoilerCollisionManager::detectCollisionsPlayerSolids() {

	// Get all solid collision objects and insert them in a vector
	std::vector<CollisionableObject *> collisionableObject;

	// Get the levers
	LeversManager *leversManager = LeversManager::getLeversManager();
	LeversManager::const_iterator itLevers = leversManager->begin();
	while( itLevers != leversManager->end() ) {
		collisionableObject.push_back( itLevers->second );
		++itLevers;
	}

	/*
	// Get the canons
	CanonsManager *canonsManager = CanonsManager::getCanonsManager();
	CanonsManager::const_iterator itCanons = canonsManager->begin();
	while( itCanons != canonsManager->end() ) {
		collisionableObject.push_back( itCanons->second );
		++itCanons;
	}
	*/

	// Get the static collision objects
	BoilerStaticCollisionsManager *boilerStaticCollisionsManager = BoilerStaticCollisionsManager::getBoilerStaticCollisionsManager();
	BoilerStaticCollisionsManager::const_iterator itStatObj = boilerStaticCollisionsManager->begin();
	while( itStatObj != boilerStaticCollisionsManager->end() ) {
		collisionableObject.push_back( *itStatObj );
		++itStatObj;
	}

	// Door
	AnimatedObject *door = BoilerAnimatedObjectManager::getBoilerAnimatedObjectManager()->getAnimatedObject( "door" );
	collisionableObject.push_back( door );

	// Boiler's boiler
	CollisionableObject *boiler_boiler = StaticObjectManager::getStaticObjectManager()->boiler_boiler;
	if( boiler_boiler != NULL)
		collisionableObject.push_back( boiler_boiler );

	std::vector<CollisionableObject *>::const_iterator itColl = collisionableObject.begin();
	while( itColl != collisionableObject.end() ) {
		const CollisionableObject *object = *itColl;

		D3DXVECTOR3 point1;
		D3DXVECTOR3 point2;
		DT_Bool existCollision = DT_GetPenDepth( Player::getPlayer()->getCollisionObject(), object->getCollisionObject(), point1, point2 );

		if( existCollision ) {
			//static int proof = 0;
			//char msg[ 128 ];
			//sprintf_s(msg, sizeof(msg), "Collision Player-Solid %d: (%f,%f,%f) (%f,%f,%f)\n",++proof, point1.x, point1.y, point1.z, point2.x, point2.y, point2.z);
			//OutputDebugString(msg);

			//Global::printCollisionableObjectPosition( object );
			//Global::printCollisionableObjectPosition( Player::getPlayer() );

			// Insert collision to player-solid vector
			PlayerSolidCollision playerSolidCollision(point1, point2);
			playerSolidCollisionVector.push_back(playerSolidCollision);
		}

		++itColl;
	}
}

void BoilerCollisionManager::treatCollisions() {

	Player *player = Player::getPlayer();

	// If fireball collision detected player dies, so not treat other collisions
	if( fireballCollisionDetected )
		player->getAnimationController()->fireballCollisionDetected( *player );
	else {

		for(unsigned int i=0; i<playerSolidCollisionVector.size(); ++i) {
			PlayerSolidCollision &playerSolidCollision = playerSolidCollisionVector[i];

			D3DXVECTOR3 normal = playerSolidCollision.point2 - playerSolidCollision.point1;
			Global::normalizeVector3(normal, normal);

			player->getAnimationController()->solidCollisionDetected(*player, normal);
		}

	}

	// Clear collision vectors for next frame
	clean();
}

void BoilerCollisionManager::clean() {
	fireballCollisionDetected = false;
	playerSolidCollisionVector.clear( );
}
