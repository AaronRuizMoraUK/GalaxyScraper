#include "IuranusCollisionManager.h"
#include "Util/SpatialPartitioner/PolarAnglesPartitioner.h"
#include "Object/CollisionableObject/AnimatedObject/Player/Player.h"
#include "Global/GlobalFunctions.h"
#include "AnimationControllers/EnemyAnimation/StatesDefines.h"

IuranusCollisionManager * IuranusCollisionManager::iuranusCollisionManager = NULL;

IuranusCollisionManager * IuranusCollisionManager::getIuranusCollisionManager() 
{
	if(iuranusCollisionManager == NULL)
		iuranusCollisionManager = new IuranusCollisionManager();

	return iuranusCollisionManager;
}

IuranusCollisionManager::IuranusCollisionManager()
: CollisionManager()
{
}

void IuranusCollisionManager::detectCollisions() {
	detectCollisionsPlayerEnemies();
	detectCollisionsBetweenEnemies();
}

void IuranusCollisionManager::detectCollisionsPlayerEnemies() {
	// Get Polar angles partitiones
	PolarAnglesPartitioner *polarAnglesPartitioner = PolarAnglesPartitioner::getPolarAnglesPartitioner();

	// Get Player
	Player *player = Player::getPlayer();
	const PAGridIndex & gridIndex= player->getSpatialGrid();

	// Obtain the player's adjacent movable objects
	std::set< IntelligentObject * > adjacentEnemies;
	polarAnglesPartitioner->getAdjacentMovableObjects(gridIndex, adjacentEnemies);

	// Obtain the player's adjacent movable objects that are in contact with player
	std::set< IntelligentObject * >::const_iterator it = adjacentEnemies.begin();
	while( it != adjacentEnemies.end() ) {
		IntelligentObject * intelligentObject = *it;

		DT_Vector3 point1;
		DT_Vector3 point2;
		DT_Bool existCollision = DT_GetPenDepth( player->getCollisionObject(), intelligentObject->getCollisionObject(), point1, point2 );

		if( existCollision ) {
			/*
			static int proof = 0;
			char msg[ 128 ];
			sprintf_s(msg, sizeof(msg), "Collision Player-Enemy %d: (%f,%f,%f) (%f,%f,%f)\n",++proof, point1[0], point1[1], point1[2], point2[0], point2[1], point2[2]);
			OutputDebugString(msg);
			*/

			// Insert collision to player-enemy vector
			playerEnemyCollisionVector.emplace_back(intelligentObject, D3DXVECTOR3(point1), D3DXVECTOR3(point2));
		}

		++it;
	}
}

void IuranusCollisionManager::detectCollisionsBetweenEnemies() {
	// Get Polar angles partitiones
	PolarAnglesPartitioner *polarAnglesPartitioner = PolarAnglesPartitioner::getPolarAnglesPartitioner();

	// Get Player
	Player *player = Player::getPlayer();
	const PAGridIndex & gridIndexPlayer = player->getSpatialGrid();

	// Get the visible grids index
	std::vector< PAGridIndex > visibleGridIndex;
	polarAnglesPartitioner->getVisibleGridsIndex(gridIndexPlayer, visibleGridIndex);

	/*
	char msg[ 128 ];
	sprintf_s(msg, sizeof(msg), "Current grid: %d,%d\n", gridIndexPlayer[0],gridIndexPlayer[1]);
	OutputDebugString(msg);
	for(unsigned int i=0; i<visibleGridIndex.size(); ++i) {
		const PAGridIndex &gridIndex = visibleGridIndex[i];

		char msg[ 128 ];
		sprintf_s(msg, sizeof(msg), "%02d,%02d    ", gridIndex[0],gridIndex[1]);
		OutputDebugString(msg);
		if(i%8==0 && i!=0)
			OutputDebugString("\n");
	}
	OutputDebugString("\n");
	*/

	// Check collisions in visible grids index
	for(unsigned int i=0; i<visibleGridIndex.size(); ++i) {
		const PAGridIndex &gridIndex = visibleGridIndex[i];

		std::set< IntelligentObject * > enemies;
		polarAnglesPartitioner->getMovableObjectsInGridIndex(gridIndex, enemies);

		if( enemies.size() > 1 ) {
			std::set< IntelligentObject * >::const_iterator it1 = enemies.begin();
			while( it1 != --enemies.end() ) {

				// If enemy is in EMERGE do not detect collision
				const std::string &currentCycleIt1 = (*it1)->getCurrentCycle();
				if( currentCycleIt1 == ANIM_EMERGE ) {
					++it1;
					continue;
				}
				
				std::set< IntelligentObject * >::const_iterator it2 = ++it1;
				--it1;
				while( it2 != enemies.end() ) {

					// If enemy who is collision is in EMERGE do not detect collision
					const std::string &currentCycleIt2 = (*it2)->getCurrentCycle();
					if( currentCycleIt2 == ANIM_EMERGE ) {
						++it2;
						continue;
					}

					DT_Vector3 point1;
					DT_Vector3 point2;
					DT_Bool existCollision = DT_GetPenDepth( (*it1)->getCollisionObject(), (*it2)->getCollisionObject(), point1, point2 );

					if( existCollision ) {
						/*
						static int proof = 0;
						char msg[ 128 ];
						sprintf_s(msg, sizeof(msg), "Collision Enemy-Enemy %d\n",++proof);
						OutputDebugString(msg);
						*/

						// Insert collision to beween enemies vector
						betweenEnemiesCollisionVector.emplace_back(*it1, *it2, D3DXVECTOR3(point1), D3DXVECTOR3(point2));
					}

					++it2;
				}

				++it1;
			}
		}
	}

}

void IuranusCollisionManager::treatCollisions() {

	// Treat between enemies collisions
	for(unsigned int i=0; i<betweenEnemiesCollisionVector.size(); ++i) {
		BetweenEnemiesCollision &betweenEnemiesCollision = betweenEnemiesCollisionVector[i];

		D3DXVECTOR3 normal = betweenEnemiesCollision.point2 - betweenEnemiesCollision.point1;
		Global::normalizeVector3(normal, normal);
		betweenEnemiesCollision.enemy1->getAnimationController()->collisionDetected(*betweenEnemiesCollision.enemy1, normal);
		betweenEnemiesCollision.enemy2->getAnimationController()->collisionDetected(*betweenEnemiesCollision.enemy2, -normal);
	}

	// Treat player-enemy collisions
	Player *player = Player::getPlayer();
	for(unsigned int i=0; i<playerEnemyCollisionVector.size(); ++i) {
		PlayerEnemyCollision &playerEnemyCollision = playerEnemyCollisionVector[i];

		D3DXVECTOR3 normal = playerEnemyCollision.point2 - playerEnemyCollision.point1;
		Global::normalizeVector3(normal, normal);

		D3DXVECTOR3 additionalMovement(0,0,0);
		player->getAnimationController()->collisionDetected(*player, normal, additionalMovement);
		playerEnemyCollision.enemy->getAnimationController()->collisionDetected(*playerEnemyCollision.enemy, -normal, additionalMovement);

		playerEnemyCollision.enemy->getAIController()->collisionDetected( *playerEnemyCollision.enemy );
	}

	// Clear collision vectors for next frame
	clean();
}

void IuranusCollisionManager::clean() {
	betweenEnemiesCollisionVector.clear();
	playerEnemyCollisionVector.clear();
}