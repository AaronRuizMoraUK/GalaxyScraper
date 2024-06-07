#include "Player.h"
#include "PlayerControllers/RailXZ/RailXZPlayerController.h"
#include "PlayerControllers/Sphere/SpherePlayerController.h"
#include "AnimationControllers/OscilatingPosition/OscilatingPositionROA.h"
#include "AnimationControllers/PlayerAnimation/PlayerAnimation.h"
#include "AnimationControllers/AssAnimation/AssAnimation.h"
#include "AnimationControllers/EnemyAnimation/EnemyAnimation.h"
#include "Global/GlobalFunctions.h"
#include "Factories/MeshFactory.h"
#include "Factories/CalCoreModelFactory.h"
#include "Factories/AnimationStateGraphFactory.h"
#include "Factories/CollisionShapeFactory.h"
#include "Managers/PlanetManager.h"
#include "Object/Planet/Planet.h"
#include "Managers/DataLocatorManager.h"
#include <cassert>

Player * Player::player = NULL;

Player * Player::getPlayer() {
	if(player == NULL)
		player = new Player();

	return player;
}

Player::Player() 
: AnimatedObject()
, playerController(NULL)
, frontKickAngleEffect(D3DX_PI/8)
, frontKickDistanceEffect(1.0f)
{
	// Init grid in space
	spaceGrid.push_back(0);
	spaceGrid.push_back(0);

	DataLocatorManager *dataLocatorManager = DataLocatorManager::getDataLocatorManager();
	const std::string &pathToXml = dataLocatorManager->getPath("xml");

	std::string xmlFile = pathToXml + "player.xml";
	bool ok = xmlParseFile(xmlFile);
	assert( ok );
}

Player::~Player()
{
	OutputDebugString("Destroying Player...\n");

	// Destroy controller
	if(playerController)
		delete playerController, playerController=NULL;

	// Automaticaly slots destroies itself
}

void Player::update(float deltaTime){
	playerController->update(*this, deltaTime);
}

void Player::hitRecieved(const D3DXVECTOR3 &newFront)
{
	PlayerAnimation *playerAnimation = (PlayerAnimation *) getAnimationController();
	playerAnimation->hitRecieved(*this, newFront);
}

bool Player::isPlayerKicking()
{
	assert((PlayerAnimation *)animationController);
	return ((PlayerAnimation *) animationController)->isPlayerKicking();
}

void Player::setPlayerToInitPosition()
{
	setTransformMatrix(initialTransformMatrix);

	currentSpeed = 0.0f;
	// TODO Falta poner al player en idle

	setPlayerController( controllerFactory("sphere_player") ); // TODO
}

void Player::onStartElement (const std::string &elem, MKeyValue &atts) {
	if (elem=="localization") 
	{
		std::string pos=atts["transform"];
		D3DXMATRIX matrix;
		D3DXMatrixIdentity(&matrix);
		sscanf_s(pos.c_str(),"%f %f %f %f %f %f %f %f %f %f %f %f",
			&matrix(0,0), &matrix(0,1), &matrix(0,2),
			&matrix(1,0), &matrix(1,1), &matrix(1,2),
			&matrix(2,0), &matrix(2,1), &matrix(2,2),
			&matrix(3,0), &matrix(3,1), &matrix(3,2));
		setTransformMatrix(matrix);

		// Set player to the height of planet
		float alpha = 0.0f;
		float delta = 0.0f;
		const Planet * planet = PlanetManager::getPlanetManager()->getCurrentPlanet();
		const D3DXVECTOR3 &center = planet->getCenter();
		planet->getPolarAngles(position, alpha, delta);
		float height = planet->getHeight(alpha, delta);
		setAlpha(alpha);
		setDelta(delta);
		if(height==0.0f)
			height=planet->getRadius();
		setPosition( center + height*localUp );

		// saving initial player position
		initialTransformMatrix = transformMatrix;
	}
	else if (elem=="mesh") {
		
		std::string meshType=atts["type"];
		std::string name=atts["name"];
		std::string graphName = atts["graph"];
		std::string initialCycleName = atts["initial_cycle"];

		if(meshType=="custom") {
			mesh = MeshFactory::getMeshFactory()->createMesh(name);
			assert(mesh);
		}
		else if (meshType=="cal3d") {
			CalAnimCoreModel *calAnimCoreModel = 
				CalCoreModelFactory::getCalCoreModelFactory()->createCalAnimCoreModel(name);
			assert(calAnimCoreModel);

			calAnimModel = new CalAnimModel();
			calAnimModel->meshId = calAnimCoreModel->meshId;
			calAnimModel->idAnimMap = calAnimCoreModel->idAnimMap;
			calAnimModel->calModel = new CalModel(calAnimCoreModel->calCoreModel);
			calAnimModel->calModel->attachMesh(calAnimModel->meshId);

			// Set the first Blend Cycle
			const int idleId = getCalIdAnim(initialCycleName);
			if(idleId!=-1)
				calAnimModel->calModel->getMixer()->blendCycle(idleId, 1, 0.5f);

			// Set cal core model mesh
			CalCoreMesh *calCoreMesh = calAnimModel->calModel->getMesh(calAnimModel->meshId)->getCoreMesh();
			mesh = MeshFactory::getMeshFactory()->createMesh(name, calCoreMesh);
			assert(mesh);
		}
		
		// The state animation graph always has to be initialized
		AnimationStates *animationStates = AnimationStateGraphFactory::getAnimationStateGraphFactory()->createAnimationStateGraph(graphName);
		assert(animationStates);
		if(animationStates!=NULL) {
			AnimationState *initialState = animationStates->findCycle(initialCycleName);
			assert(initialState);
			if(initialState!=NULL)
				animationGraph.setCurrentState(initialState);
		}

		// Now mesh exists, create collision object
		collisionShapeHandle = 
			CollisionShapeFactory::getCollisionShapeFactory()->createCollisionShape("player"
			, mesh, PLAYER_SHAPE, true);
		assert(collisionShapeHandle);
		collisionObjectHandle = DT_CreateObject(this, collisionShapeHandle->shapeHandle);
		setTransformMatrix(transformMatrix);
	}
	else if (elem=="material") {
		std::string materialName=atts["name"];
		std::string shaderName=atts["shader"];
		std::string techniqueName=atts["technique"];
		SETMATERIAL(materialName, shaderName, techniqueName); // VERY VERY OPTIONAL!!
	}
	else if (elem=="animation_controller") {
		std::string animController=atts["type"];
		setAnimationController( animationControllerFactory(animController) );
	}
	else if (elem=="controller") {
		std::string controller=atts["type"];
		setPlayerController( controllerFactory(controller) );
	}
	else if (elem=="front_kick_effect") {
		float angle = atts.getFloat( "angle", 0.0f ); // Read angle in degree
		frontKickAngleEffect = angle * D3DX_PI / 360; // Save angle in radians

		frontKickDistanceEffect = atts.getFloat( "distance", 0.0f );
	}
}

IPlayerController * Player::controllerFactory(const std::string &controller) {
	if(controller=="railxz_player") {
		return new RailXZPlayerController();
	}
	else if(controller=="sphere_player") {
		// Obtain Player reference direction to obtain currentAngle
		D3DXVECTOR3 referenceDirection;
		D3DXVec3Cross(&referenceDirection, &(D3DXVECTOR3(0,1,0)), &localUp);
		Global::normalizeVector3(referenceDirection, referenceDirection);

		float cosAngle = D3DXVec3Dot(&referenceDirection, &localFront);
		float currentAngle = acos(cosAngle);

		// Now we have the angle, let's obtain the sign for this angle
		// calculating the localUp/referenceDirection vectors make and 
		// seeing where the (playerPosition+localFront) is.
		D3DXVECTOR3 normal;
		D3DXVec3Cross(&normal, &localUp, &referenceDirection);

		// Obtain D value of the ecuation plane
		float D = - ( normal.x*position.x + normal.y*position.y + normal.z*position.z );

		D3DXVECTOR3 referencePoint( position+localFront ); 
		float sign = normal.x*referencePoint.x + normal.y*referencePoint.y +
			normal.z*referencePoint.z + D;

		if(sign<0)
			currentAngle=-currentAngle;

		return new SpherePlayerController(currentAngle);
	}

	return NULL;
}

IAnimationController * Player::animationControllerFactory(const std::string &animController) {
	if(animController=="oscilating_position_roa") {
		return new OscilatingPositionROA(getPosition()
			, D3DXQUATERNION(0,0,0,1), D3DXVECTOR3(0,1,0), 2, 1.0f);
	}
	else if(animController=="player_animation") {
		return new PlayerAnimation(getPosition()
			, D3DXQUATERNION(0,0,0,1), D3DXVECTOR3(0,1,0), 2.5, 1.0f);
	}
	/*
	else if(animController=="keyframed_roa") {

		DataLocatorManager *dataLocatorManager = DataLocatorManager::getDataLocatorManager();
		const std::string &pathToRoa = dataLocatorManager->getPath("roa");

		std::string xmlFile = pathToRoa + "cube_anim.xml";
	
		RigidAnimationResource *rigidAnimationResource = new RigidAnimationResource();
		rigidAnimationResource->initFromFile(xmlFile.c_str());
		// TODO DELETE DEL RIGIDANIMATIONRESOURCE
		// In this constructor we do not have the position and rotation yet, the animation
		// is going to begin in (0,0,0) with rotation (0,0,0,1)
		D3DXQUATERNION rotationQuaternion;
		D3DXQuaternionRotationMatrix(&rotationQuaternion, &transformMatrix);
		KeyFramedROA *keyFramedROA = new KeyFramedROA(rigidAnimationResource, 
			getPosition(), rotationQuaternion);
		keyFramedROA->setTimeScale(0.5f);
		return keyFramedROA;
	}
	*/
	else {
		assert( !"Not type of animation controller allowed in Player" );
		return NULL;
	}

	return NULL;
}

Slot* Player::getFreeSlot(unsigned int maxSlots)
{
	// this function has to:
	//	- check slots.size() < maxPlayerAttacants
	//  - if it is, try to find any slot with toWho = NULL
	//  - if not found, create new Slot()
	// return NULL, found or created Slot respectively

	// check if we already have a slot position created
	// but with a NULL element
	Slots::const_iterator it = slots.begin();
	while( it != slots.end() )
	{
		if( (*it)->isFree() )
			return *it;
		++it;
	}

	// if no more slots are allowed, return NULL
	if ( slots.size() >= maxSlots )
		return NULL;

	// if we are here means that:
	//  - we still can create one more Slot
	//	- and no one of the existing is free
	D3DXVECTOR3 localSlotPosition(0,0,0);
	Slot *s = new Slot( localSlotPosition );
	slots.push_back(s);

	return s;
}

unsigned int Player::getSlotIndex(Slot *s) const
{
	Slots::const_iterator it = slots.begin();
	unsigned int index = 0;
	while( it != slots.end() )
	{
		if( (*it) == s ) 
			return index;
		index++;
		++it;
	}

	return -1;
}

Slot* Player::getSlotByIndex(unsigned int i) const
{
	Slot *slot;
	try
	{
		slot = slots.at(i);
	} catch (std::out_of_range o)
	{
		slot = NULL;
	}
	return slot;
}