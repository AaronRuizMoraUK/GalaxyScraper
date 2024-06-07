#include "IuranusEnemyManager.h"
#include "Global/GlobalFunctions.h"
#include "Global/GlobalVariables.h"
#include "Factories/MeshFactory.h"
#include "AIControllers/ArachnidBehaviour/ArachnidBehaviour.h"
#include "Object/CollisionableObject/AnimatedObject/Player/Player.h"
#include "Managers/PlanetManager.h"
#include "Managers/LogicManager/LogicManager.h"
#include "Managers/SoundManager.h"
#include "Object/Planet/Planet.h"
#include "Util/SpatialPartitioner/PolarAnglesPartitioner.h"
#include "Factories/CalCoreModelFactory.h"
#include "Factories/AnimationStateGraphFactory.h"
#include "Factories/CollisionShapeFactory.h"
#include "Managers/DataLocatorManager.h"
#include "SOLID.h"
#include <fstream>
#include <cassert>
#include "AnimationControllers/EnemyAnimation/StatesDefines.h"
#include "Managers/ParticlesManager.h"
#include "Managers/IuranusMAnagers/IuranusShadowManager.h"

IuranusEnemyManager * IuranusEnemyManager::iuranusEnemyManager = NULL;

IuranusEnemyManager * IuranusEnemyManager::getIuranusEnemyManager() {
	if(iuranusEnemyManager == NULL)
		iuranusEnemyManager = new IuranusEnemyManager();

	return iuranusEnemyManager;
}

IuranusEnemyManager::IuranusEnemyManager() 
: currentEnemyType("")
{
	init();
};

void IuranusEnemyManager::init()
{
	DataLocatorManager *dataLocatorManager = DataLocatorManager::getDataLocatorManager();
	const std::string &pathToXml = dataLocatorManager->getPath("xml");

	std::string xmlFile = pathToXml + "/iuranus/iuranus_enemies.xml";
	bool ok = xmlParseFile(xmlFile);
	assert(ok);
}

void IuranusEnemyManager::onStartElement (const std::string &elem, MKeyValue &atts) 
{

	if( elem == "enemy" ) 
	{
		currentEnemyType = atts["type"];
		enemyTypes[currentEnemyType] = new IntelligentObject();
	}
	else if( elem == "mesh" )
	{
		std::string coreMeshName = atts["name"];
		std::string typeMesh = atts["type"];
		std::string graphName = atts["graph"];
		std::string initialCycleName = atts["initial_cycle"];
		std::string initialActionName = atts["initial_action"];

		float speedMultiplier = atts.getFloat( "speed_multiplier", 0.0f );

		enemyTypes[currentEnemyType]->setSpeedMultiplier(speedMultiplier);
		enemyTypes[currentEnemyType]->setDefaultSpeedMultiplier(speedMultiplier);

		if (typeMesh == "custom")
		{
			IMesh * mesh = MeshFactory::getMeshFactory()->createMesh(coreMeshName);
			assert(mesh);
			enemyTypes[currentEnemyType]->setMesh(mesh);
		}
		else if(typeMesh == "cal3d")
		{
			CalAnimModel *calAnimModel = NULL;
			CalAnimCoreModel *calAnimCoreModel = 
				CalCoreModelFactory::getCalCoreModelFactory()->createCalAnimCoreModel(coreMeshName);
			assert(calAnimCoreModel);

			calAnimModel = new CalAnimModel();
			calAnimModel->meshId = calAnimCoreModel->meshId;
			calAnimModel->idAnimMap = calAnimCoreModel->idAnimMap;
			calAnimModel->calModel = new CalModel(calAnimCoreModel->calCoreModel);
			calAnimModel->calModel->attachMesh(calAnimModel->meshId);

			CalCoreMesh *calCoreMesh = calAnimModel->calModel->getMesh(calAnimModel->meshId)->getCoreMesh();

			IMesh * mesh = MeshFactory::getMeshFactory()->createMesh(coreMeshName, calCoreMesh);
			assert(mesh);
			enemyTypes[currentEnemyType]->setMesh(mesh);

			// Set the calAnimModel to the enemy
			enemyTypes[currentEnemyType]->setCalAnimModel(calAnimModel);
		}

		// The state animation graph always has to be initialized
		AnimationStates *animationStates = AnimationStateGraphFactory::getAnimationStateGraphFactory()->createAnimationStateGraph(graphName);
		assert(animationStates);
		if(animationStates!=NULL) {
			AnimationState *initialState = animationStates->findState(initialCycleName, initialActionName);
			assert(initialState);
			if(initialState!=NULL)
				enemyTypes[currentEnemyType]->setCurrentAnimationState(initialState);
		}

		// Now mesh exists, create collision object
		ShapeHandle *collisionShapeHandle = 
			CollisionShapeFactory::getCollisionShapeFactory()->createCollisionShape("arachnid"
			, enemyTypes[currentEnemyType]->getMesh(), ARACHNID_SHAPE, true);
		assert(collisionShapeHandle);
		DT_ObjectHandle collisionObjectHandle = DT_CreateObject(enemyTypes[currentEnemyType], collisionShapeHandle->shapeHandle);
		enemyTypes[currentEnemyType]->setCollisionShape(collisionShapeHandle);
		enemyTypes[currentEnemyType]->setCollisionObject(collisionObjectHandle);
		// This is for update the position in world of collisionable object too
		enemyTypes[currentEnemyType]->setTransformMatrix(enemyTypes[currentEnemyType]->getTransformMatrix());
	}
	else if( elem == "material" )
	{
		std::string texture = atts["texture"];
		std::string shader = atts["shader"];
		std::string technique = atts["technique"];
		enemyTypes[currentEnemyType]->SETMATERIAL(texture, shader, technique);
	}
	else if( elem == "ai_controller" )
	{
		IAIController * aiController;
		std::string aiControllerType = atts["type"];

		if( aiControllerType == "arachnid_behaviour" )
		{
			D3DXVECTOR3 localSlotPosition(0,0,0);	
			sscanf_s( atts["local_slot"].c_str(), "%f %f %f", &localSlotPosition.x, &localSlotPosition.y, &localSlotPosition.z );

			float mouthIdleDistance = atts.getFloat( "mouth_idle_distance", 0.0f );
			float mouthSlotableDistance = atts.getFloat( "mouth_slotable_distance", 0.0f );
			float assIdleDistance = atts.getFloat( "ass_idle_distance", 0.0f );
			float assSlotableDistance = atts.getFloat( "ass_slotable_distance", 0.0f );
			float attackDistance = atts.getFloat( "attack_distance", 0.0f );
			float attackDirection = atts.getFloat( "attack_direction", 0.0f );
			float groupmentDistance = atts.getFloat( "groupment_distance", 0.0f );
			float foolishedTime = atts.getFloat( "foolished_time", 0.0f );
			float walkSpeed = atts.getFloat( "walk_speed", 0.0f );
			float hitSpeed = atts.getFloat( "hit_speed", 0.0f );
			float prepareAttackTime = atts.getFloat( "prepare_attack_time", 0.0f );
			float attackSpeed = atts.getFloat( "attack_speed", 0.0f );
			float runamaySpeed = atts.getFloat( "runaway_speed", 0.0f );
			float visionDistance = atts.getFloat( "vision_distance", 0.0f );
			float visionAngle = atts.getFloat( "vision_angle", 0.0f );
			visionAngle = visionAngle * D3DX_PI / 360;
			float visionRadius = atts.getFloat( "vision_radius", 0.0f );
			unsigned int maxPlayerAttacants = atts.getInt( "max_player_attacants", 0 );
			float stopFollowingPlayerDistance = atts.getFloat( "stop_following_player_distance", 0.0f );

			aiController = new ArachnidBehaviour(localSlotPosition,
				mouthIdleDistance,
				mouthSlotableDistance,
				assIdleDistance,
				assSlotableDistance,
				attackDistance,
				attackDirection,
				groupmentDistance,
				foolishedTime,
				walkSpeed,
				hitSpeed,
				prepareAttackTime,
				attackSpeed,
				runamaySpeed,
				visionDistance,
				visionAngle,
				visionRadius,
				maxPlayerAttacants,
				stopFollowingPlayerDistance);
		}
		else {
			assert( !"Not type of AI controller allowed in IuranusEnemyManager" );
			return;
		}

		if ( aiController )
			enemyTypes[currentEnemyType]->setAIController(aiController);
	}
	else if (elem=="animation_controller") 
	{

		IAnimationController *iAnimationController = NULL;	

		std::string animController=atts["type"];

		if( animController == "enemy_animation" ) {
			float rotationSpeed = atts.getFloat( "rotation_speed", 0.0f );
			float jumpHeight = atts.getFloat( "jump_height", 0.0f );

			iAnimationController = new EnemyAnimation( rotationSpeed, jumpHeight );
		}
		else {
			assert( !"Not type of animation controller allowed in IuranusEnemyManager" );
			return;
		}

		if ( iAnimationController )
			enemyTypes[currentEnemyType]->setAnimationController(iAnimationController);
	}

}

void IuranusEnemyManager::createEnemy(const D3DXVECTOR3 &createWhere, const std::string &enemyType)
{
	EnemyTypes::iterator it = enemyTypes.find(enemyType);
	if(it == enemyTypes.end()) {
		assert( !"Type of enemy not supported in IuranusEnemyManager" );
		return;
	}

	IntelligentObject *newEnemy = new IntelligentObject( *(it->second) );

	// If this enemy works by cal3d
	if( newEnemy->getCalModel() ) {
		// Name of current state in animation graph
		const std::string &currentCycle = newEnemy->getCurrentCycle();
		const std::string &currentAction = newEnemy->getCurrentAction();

		// Look for the id of currentCycle to make the blend in the mixer
		if (!currentCycle.empty())
		{
			const int idCycle = newEnemy->getCalIdAnim(currentCycle);

			if (idCycle > -1)
				newEnemy->getCalModel()->getMixer()->blendCycle(idCycle, 1, 0.5f);
		}


		if (!currentAction.empty())
		{
			const int idAction = newEnemy->getCalIdAnim(currentAction);

			if (idAction > -1)
				newEnemy->getCalModel()->getMixer()->executeAction(idAction, 0.2f, 0.2f, 1.0f, false);
		}
	}

	// calculate new transformated matrix for the new enemy
	const Planet *planet = PlanetManager::getPlanetManager()->getCurrentPlanet();
	const D3DXVECTOR3 &center = planet->getCenter();

	D3DXMATRIX transformMatrix;
	D3DXMatrixIdentity(&transformMatrix);

	assert( createWhere != center );
	D3DXVECTOR3 localUp = createWhere - center;
	Global::normalizeVector3(localUp, localUp);

	D3DXVECTOR3 localLeft;
	assert( localUp != D3DXVECTOR3(0,1,0) );
	D3DXVec3Cross(&localLeft, &localUp, &D3DXVECTOR3(0,1,0));
	Global::normalizeVector3(localLeft, localLeft);

	D3DXVECTOR3 localFront;
	D3DXVec3Cross(&localFront, &localLeft, &localUp);

	// Rotation
	transformMatrix(0,0) = localLeft.x;
	transformMatrix(0,1) = localLeft.y;
	transformMatrix(0,2) = localLeft.z;
	transformMatrix(1,0) = localUp.x;
	transformMatrix(1,1) = localUp.y;
	transformMatrix(1,2) = localUp.z;
	transformMatrix(2,0) = localFront.x;
	transformMatrix(2,1) = localFront.y;
	transformMatrix(2,2) = localFront.z;

	// Random rotation
	float angleRotation = Global::obtainRandom(0.0f ,2.0f*D3DX_PI);
	D3DXMATRIX rotateAngle;
	D3DXMatrixIdentity(&rotateAngle);
	D3DXMatrixRotationAxis(&rotateAngle, &localUp, angleRotation);
	D3DXMatrixMultiply(&transformMatrix, &transformMatrix, &rotateAngle);

	// position

	// The point of birth must to be over the surface of planet
	// Calculate the final position with height
	float alpha = 0.0f;
	float delta = 0.0f;
	planet->getPolarAngles(createWhere, alpha, delta);
	newEnemy->setAlpha(alpha);
	newEnemy->setDelta(delta);
	float height = planet->getHeight(alpha, delta);
	if(height==0.0f)
		height=planet->getRadius();
	D3DXVECTOR3 finalCreateWhere = center + height*localUp;
	//newEnemy->setPosition( finalPosition );

	// make finalPosition be under the planet
	const float enemyHeight = 1.0f;
	D3DXVECTOR3 initialPosition = finalCreateWhere - localUp * enemyHeight;

	transformMatrix(3,0) = initialPosition.x;
	transformMatrix(3,1) = initialPosition.y;
	transformMatrix(3,2) = initialPosition.z;

	newEnemy->setTransformMatrix(transformMatrix);

	push_back(newEnemy);

	// Add enemy to space
	PolarAnglesPartitioner *spatialPartitioner = PolarAnglesPartitioner::getPolarAnglesPartitioner();
	bool ok = spatialPartitioner->addMovableObject( newEnemy );
	assert(ok);
}

IuranusEnemyManager::iterator IuranusEnemyManager::killEnemy(IuranusEnemyManager::iterator &enemyIterator)
{
	IntelligentObject *enemy =(*enemyIterator);

	// Extract enemy from space
	PolarAnglesPartitioner *spatialPartitioner = PolarAnglesPartitioner::getPolarAnglesPartitioner();
	spatialPartitioner->removeMovableObject( enemy );

	

	if(enemy)
	{
		ParticlesManager *pm = ParticlesManager::getParticlesManager();
		pm->removeParticles(enemy);
		delete enemy, enemy=NULL;
	}
	return erase(enemyIterator);
}

void IuranusEnemyManager::render() const {
	IuranusShadowManager *iuranusShadowManager = IuranusShadowManager::getIuranusShadowManager();
	int index = 0;

	const_iterator i = begin();

#ifdef DEBUG
	int enemiesDrawn = 0;
	int backEnemiesDrawn = 0;
#endif

	while(i!=end()) {
		Object *obj = (*i);

		const IMesh * mesh = obj->getMesh();
		const D3DXVECTOR3 &center = mesh->getBoundingSphereCenter();
		float radius = mesh->getBoundingSphereRadius();

		D3DXVECTOR3 centerInstance = center + obj->getPosition();
		if( Global::frustum.isBSphereVisible(centerInstance, radius) ) 
		{
#ifdef DEBUG
			enemiesDrawn++;
#endif
			if( Global::backSideFrustum.isBSphereVisible(centerInstance, radius) )
			{
#ifdef DEBUG
				backEnemiesDrawn++;
#endif
				obj->setDrawn(true);
				obj->render();

				// Update Shadows position
				iuranusShadowManager->updateEnemyShadow( *i, index );
				++index;
			}
			else
			{
				obj->setDrawn(false);
			}
		}
		else {
			obj->setDrawn(false);
		}
		++i;
	}

#ifdef DEBUG
	Global::print(0,20, 0xffff0000, "Frustum Enemies drawn: %d", enemiesDrawn);
	Global::print(0,30, 0xffff0000, "BackSideFrustum enemies drawn: %d", backEnemiesDrawn);
#endif
}

void IuranusEnemyManager::changeStatusEnemiesKicked() 
{
	Player * player = Player::getPlayer();
	if( !player->isPlayerKicking() )
		return;

	// Get enemies close to player
	PolarAnglesPartitioner *spatialPartitioner = PolarAnglesPartitioner::getPolarAnglesPartitioner();
	std::set<IntelligentObject *> enemiesClosePlayer;
	spatialPartitioner->getAdjacentMovableObjects(player->getSpatialGrid(), enemiesClosePlayer);

	if( enemiesClosePlayer.empty() )
		return;

	const D3DXVECTOR3 &playerPosition = player->getPosition();
	const D3DXVECTOR3 &playerFront = player->getLocalFrontVector();
	const float frontKickDistanceEffect = player->getFrontKickDistanceEffect();
	const float frontKickAngleEffect = player->getFrontKickAngleEffect();
	const float minimunRadius = 0.5f;

	std::set<IntelligentObject *>::iterator it = enemiesClosePlayer.begin();
	while( it != enemiesClosePlayer.end() ) {

		// Check if any enemy is kicked and change its state to HIT
		const int state = ((ArachnidBehaviour *)(*it)->getAIController())->getCurrentAIState();
		const std::string &currentCycle = (*it)->getCurrentCycle( );
		//if( state != HIT && state != EMERGE && currentCycle!=ANIM_FLY_UPWARDS )
		if( state != EMERGE && currentCycle != ANIM_FLY_UPWARDS && currentCycle != ANIM_FLY_DOWNWARDS )
		{
			const D3DXVECTOR3 &enemyPosition = (*it)->getPosition();
			float enemyDistance = Global::distanceSqrt(playerPosition, enemyPosition);
			if( enemyDistance < frontKickDistanceEffect )
			{
				D3DXVECTOR3 enemyVector = enemyPosition - playerPosition;
				Global::normalizeVector3( enemyVector, enemyVector );
				float cosinus = D3DXVec3Dot( &playerFront, &enemyVector );
				float alpha = acos(cosinus);

				// Enemy is in angle vision or enemy is too close
				if( alpha < frontKickAngleEffect || 
					( enemyDistance<minimunRadius && alpha<D3DX_PI/2.0f ) )
				{
					if( Global::samplesEnabled )
					{
						SoundManager *soundManager = SoundManager::getSoundManager();

						int randNum = floor(Global::obtainRandom( 1.0f, 3.5f ));
						randNum = Global::clamp( randNum, 1, 3 );

						char sound[ 128 ];
						sprintf_s(sound, sizeof(sound), "enemy_hit_received_%d", randNum);
			
						soundManager->playSample(sound);
					}
				
					ParticlesManager *pm = ParticlesManager::getParticlesManager();
					pm->addParticle("enemyHit", (*it), false);


					IAIController *aiController = (*it)->getAIController();
					aiController->setCurrentAIState(HIT);
				}
			}
		}

		++it;
	}
}

void IuranusEnemyManager::updateAnimation(float deltaTime)
{
	iterator it = begin();
	while (it != end() ) 
	{
		(*it)->updateAnimation(deltaTime);

		++it;
	}
}

void IuranusEnemyManager::updateAI(float deltaTime) 
{
	changeStatusEnemiesKicked();

	iterator it = begin();
	while (it != end() ) 
	{
		(*it)->refreshPreviousTransformMatrix(); // Set previous transform matrix to transform matrix
		(*it)->updateAI(deltaTime);
		++it;
	}
}

IntelligentObject * IuranusEnemyManager::getGroupablePartner(const IntelligentObject &partnerSearcher) const
{
	/*
	// With spatial partitioner

	// Get enemies close
	PolarAnglesPartitioner *spatialPartitioner = PolarAnglesPartitioner::getPolarAnglesPartitioner();
	std::set<IntelligentObject *> enemiesClose;
	//spatialPartitioner->getMovableObjectsInGridIndex(partnerSearcher.getSpatialGrid(), enemiesClose);
	spatialPartitioner->getAdjacentMovableObjects(partnerSearcher.getSpatialGrid(), enemiesClose);

	// PartnerSearcher is in the grid, so, must be more than 1 element
	if( enemiesClose.size() <= 1 )
		return NULL;

	std::set<IntelligentObject *>::const_iterator it = enemiesClose.begin();

	// obtain all the arachnids with a free slot, except partnerSearcher
	// but only if they are in IDLE_OUT || RANDOM_WALK states
	while( it != enemiesClose.end() ) {
		if( (*it) != &partnerSearcher ) {
			ArachnidBehaviour *partner = (ArachnidBehaviour*)(*it)->getAIController();
			if( partner->freeSlot() ) {
				int state = partner->getCurrentAIState();
				if( state == IDLE_OUT || state == RANDOM_WALK )
					return (*it);
			}
		}
		++it;
	}

	return NULL;
	*/

	// With distances

	IntelligentObject *partnerFound = NULL;
	const D3DXVECTOR3 &partnerSearcherPos = partnerSearcher.getPosition();
	float minPartnerDistance = FLT_MAX;

	const_iterator it = begin();

	// obtain all the arachnids with a free slot, except partnerSearcher
	// but only if they are in IDLE_OUT || RANDOM_WALK states
	while( it != end() )
	{
		if( (*it) != &partnerSearcher )
		{
			ArachnidBehaviour *a = (ArachnidBehaviour*)(*it)->getAIController();
			bool partnerSlotFree = a->freeSlot();
			if( partnerSlotFree )
			{
				int state = a->getCurrentAIState();
				if( state == IDLE_OUT || state == RANDOM_WALK )
				{
					const D3DXVECTOR3 &freePartnerPos = (*it)->getPosition();
					float currentPartnerDistance = Global::distance(partnerSearcherPos, freePartnerPos);
					if(currentPartnerDistance < minPartnerDistance) 
					{
						minPartnerDistance = currentPartnerDistance;
						partnerFound = (*it);	
					}
				}
			}
		}
		++it;
	}

	return partnerFound;
}

void IuranusEnemyManager::cleanAll() 
{
	EnemyTypes::iterator it = enemyTypes.begin();
	while(it != enemyTypes.end())
	{	
		IntelligentObject *intObj = (it->second);
		if(intObj)
			delete intObj, intObj=NULL;
		++it;
	}
	enemyTypes.clear();

	clean();
}

void IuranusEnemyManager::clean() 
{
	foolisheds.clear();
	enemiesEaten.clear();

	// Extract enemy from space and delete it
	PolarAnglesPartitioner *spatialPartitioner = PolarAnglesPartitioner::getPolarAnglesPartitioner();
	while(!empty()) 
	{
		IntelligentObject * intelligentObject= back();
		spatialPartitioner->removeMovableObject( intelligentObject );
		if(intelligentObject)
			delete intelligentObject, intelligentObject=NULL;
		pop_back();
	}
}

void IuranusEnemyManager::addFoolished(AnimatedObject *foolished)
{
	assert( foolished );
	foolisheds.push_back(foolished);
}

void IuranusEnemyManager::removeFoolished(AnimatedObject *foolished)
{
	assert( foolished );
	foolisheds.remove(foolished);
}

void IuranusEnemyManager::addEnemyEaten(IntelligentObject *eaten)
{
	assert( eaten );
	foolisheds.remove(eaten);
	assert( eaten );

	// change enemy IA state to "SENTENCED_TO_DEATH"
	eaten->getAIController()->setCurrentAIState(SENTENCED_TO_DEATH);

	enemiesEaten.push_back(eaten);
}

void IuranusEnemyManager::killEatenEnemies()
{
	EnemiesEaten::iterator it = enemiesEaten.begin();

	while( it != enemiesEaten.end() ) 
	{
		// If an enemiesEaten is not in SENTENCED_TO_DEATH then do not delete
		// because particles failed, put it in SENTENCED_TO_DEATH
		if( (*it)->getAIController()->getCurrentAIState() != SENTENCED_TO_DEATH ) {
			(*it)->getAIController()->setCurrentAIState( SENTENCED_TO_DEATH );

			++it;
			continue;
		}

		iterator it0 = begin();
		iterator it0_end = end();
		while( it0 != it0_end ) 
		{
			if( (*it0) == (*it) ) 
			{
				killEnemy(it0);
				it0 = it0_end = end();
			}
			else
				++it0;
		}

		++it;
	}
	enemiesEaten.clear();
}


bool IuranusEnemyManager::enemyAlreadyEaten( const IntelligentObject *object ) const
{
	EnemiesEaten::const_iterator it = enemiesEaten.begin();
	while( it != enemiesEaten.end() ) 
	{
		if( *it == object )
			return true;
		++it;
	}
	return false;
}


bool IuranusEnemyManager::someNearFoolished(const AnimatedObject &nonFoolished, float visionDistance) const
{
	const D3DXVECTOR3 &nonFoolishedPosition = nonFoolished.getPosition();
	Foolisheds::const_iterator it = foolisheds.begin();

	while( it != foolisheds.end() )
	{
		const D3DXVECTOR3 &foolishedPosition = (*it)->getPosition();
		float distanceToFoolished = Global::distanceSqrt( nonFoolishedPosition, foolishedPosition );
		if( distanceToFoolished < visionDistance ) 
			return true;
		++it;
	}

	return false;
}

void IuranusEnemyManager::checkForEnemiesInMouthExpelRadius() const
{
	const LogicVolumeManager &iuranusLogicVolumeManager = LogicManager::getLogicManager()->getIuranusLogicVolumeManager();		
	const LogicVolume *volume = iuranusLogicVolumeManager.getElement("expel_mouth");

	const_iterator it = begin();
	while (it != end() ) 
	{
		IAIController *aiController = (*it)->getAIController();
		const std::string &currentCycle = (*it)->getCurrentCycle( );
		if( currentCycle != ANIM_FLY_UPWARDS && currentCycle != ANIM_FLY_DOWNWARDS )
		{
			bool enemyInside = volume->checkObjectInsideVolume( *(*it) );
			if( enemyInside )
				aiController->setCurrentAIState( MOUTH_EXPEL );
		}
		++it;
	}
}

void IuranusEnemyManager::checkForEnemiesInAssExpelRadius() const
{
	const LogicVolumeManager &iuranusLogicVolumeManager = LogicManager::getLogicManager()->getIuranusLogicVolumeManager();
	const LogicVolume *volume = iuranusLogicVolumeManager.getElement("expel_ass");

	const_iterator it = begin();
	while( it != end() )
	{
		IAIController *aiController = (*it)->getAIController();
		const std::string &currentCycle = (*it)->getCurrentCycle( );
		if( currentCycle != ANIM_FLY_UPWARDS && currentCycle != ANIM_FLY_DOWNWARDS )
		{
			bool enemyInside = volume->checkObjectInsideVolume( *(*it) );
			if( enemyInside )
				aiController->setCurrentAIState( ASS_EXPEL );
		}
		++it;
	}
}

void IuranusEnemyManager::changeEnemiesStateToVictory() const
{
	const_iterator it = begin();
	while( it != end() )
	{
		IAIController *aiController = (*it)->getAIController();
		aiController->setCurrentAIState( PREPARE_PLAYER_EATEN );
		++it;
	}
}

void IuranusEnemyManager::resetEnemiesStates()
{
	const_iterator it = begin();
	while( it != end() )
	{
		IAIController *aiController = (*it)->getAIController();
		aiController->setCurrentAIState( EMERGE );
		++it;
	}
}

void IuranusEnemyManager::reloadShader()
{
	Foolisheds::iterator itF = foolisheds.begin();
	while( itF != foolisheds.end() ) 
	{
		(*itF)->reloadShader();
		++itF;
	}

	EnemyTypes::iterator itET = enemyTypes.begin();
	while( itET != enemyTypes.end() ) 
	{
		(itET->second)->reloadShader();
		++itET;
	}

	EnemiesEaten::iterator itEE = enemiesEaten.begin();
	while( itEE != enemiesEaten.end() ) 
	{
		(*itEE)->reloadShader();
		++itEE;
	}

	iterator it = begin();
	while( it != end() ) 
	{
		(*it)->reloadShader();
		++it;
	}
}
