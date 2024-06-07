#include "LeversManager.h"
#include "Managers/DataLocatorManager.h"
#include "Managers/PlanetManager.h"
#include "Managers/LogicManager/LogicManager.h"
#include "Managers/SoundManager.h"
#include "Object/CollisionableObject/AnimatedObject/Player/Player.h"
#include "AnimationControllers/KeyFramed/KeyFramedROA.h"
#include "Global/GlobalVariables.h"
#include <cassert>

LeversManager * LeversManager::leversManager = NULL;

LeversManager * LeversManager::getLeversManager() 
{
	if(leversManager == NULL)
		leversManager = new LeversManager();

	return leversManager;
}
 
LeversManager::LeversManager()
: currentParsed(NULL)
, currentParsedElementId(0)
, activateLeverAnimationResource(NULL)
{
	init();
}

void LeversManager::init()
{
	DataLocatorManager *dataLocatorManager = DataLocatorManager::getDataLocatorManager();
	const std::string &pathToRoa = dataLocatorManager->getPath("roa");

	// Animation Resources
	activateLeverAnimationResource = new RigidAnimationResource();
	std::string filename = pathToRoa + "activate_lever.xml";
	bool ok = activateLeverAnimationResource->initFromFile( filename.c_str() );
	assert(ok);

	const std::string &pathToXml = dataLocatorManager->getPath("xml");
	std::string xmlFile = pathToXml + "boiler/boiler_levers.xml";
	ok = xmlParseFile(xmlFile);
	assert(ok);
}

void LeversManager::onEndElement (const std::string &elem)
{
	if( elem == "lever" )
	{
		// creating ROA instance
		D3DXQUATERNION rotationQuaternion;
		D3DXQuaternionRotationMatrix(&rotationQuaternion, &(currentParsed->getTransformMatrix()));
		
		KeyFramedROA *activateLeverAnimation = new KeyFramedROA( activateLeverAnimationResource, currentParsed->getPosition(), rotationQuaternion );
		currentParsed->setAnimationController( activateLeverAnimation );

		// Create collision object
		ShapeHandle *collisionShapeHandle = 
			CollisionShapeFactory::getCollisionShapeFactory()->createCollisionShape("lever"
			, currentParsed->getMesh(), LEVER_SHAPE, true);
		assert(collisionShapeHandle);
		DT_ObjectHandle collisionObjectHandle = DT_CreateObject(currentParsed, collisionShapeHandle->shapeHandle);
		currentParsed->setCollisionShape(collisionShapeHandle);
		currentParsed->setCollisionObject(collisionObjectHandle);
		// Do know the setTransformMatrix for update also the collision position in world
		currentParsed->setTransformMatrix( currentParsed->getTransformMatrix() );

		(*this)[currentParsedElementId] = currentParsed;
		currentParsed = NULL;
	}
}

void LeversManager::onStartElement (const std::string &elem, MKeyValue &atts) 
{
	if( currentParsed ) {
		createElement( elem, atts );
		return;
	}

	if( elem == "lever" )
	{
		currentParsed = new LeverObject();
		currentParsedElementId = atts.getInt( "lever_id", 0 );
	}
}

void LeversManager::createElement(const std::string &elem, MKeyValue &atts) 
{ 
	if( elem == "localization" )
	{
		std::string position=atts["transform"];
		D3DXMATRIX matrix;
		D3DXMatrixIdentity(&matrix);
		sscanf_s(position.c_str(),"%f %f %f %f %f %f %f %f %f %f %f %f",
			&matrix(0,0), &matrix(0,1), &matrix(0,2),
			&matrix(1,0), &matrix(1,1), &matrix(1,2),
			&matrix(2,0), &matrix(2,1), &matrix(2,2),
			&matrix(3,0), &matrix(3,1), &matrix(3,2));

		// Boiler planet center and radius
		const Planet *planet = PlanetManager::getPlanetManager()->getPlanet("boiler");
		const D3DXVECTOR3 &center = planet->getCenter();

		currentParsed->setTransformMatrix(matrix);
		currentParsed->setPosition(currentParsed->getPosition() + center);

		// Set the base lever transform matrix
		currentParsed->setBaseLeverTransformMatrix( currentParsed->getTransformMatrix() );

		// Necesary to restart lever
		currentParsed->saveInitialTransformMatrix( );
	}
	else if( elem == "mesh" )
	{
		std::string coreMesh = atts["name"];
		std::string baseCoreMesh = atts["name_base"];
		
		// Create Cores
		IMesh * mesh = MeshFactory::getMeshFactory()->createMesh(coreMesh);
		assert(mesh);
		currentParsed->setMesh(mesh);

		// Base Lever Mesh
		IMesh * meshBase = MeshFactory::getMeshFactory()->createMesh(baseCoreMesh);
		assert(meshBase);
		currentParsed->setBaseMesh(meshBase);
	}
	else if( elem == "material" )
	{
		std::string texture = atts["texture"];
		std::string shader = atts["shader"];
		std::string technique = atts["technique"];
		currentParsed->SETMATERIAL(texture, shader, technique);
 	}
}

void LeversManager::restart() {
	iterator i = begin();
	while(i!=end()) {
		i->second->restart();
		++i;
	}
}

void LeversManager::render() const 
{
	const_iterator i = begin();

	while(i!=end()) {
		const Object *obj = i->second;

		const IMesh * mesh = obj->getMesh();

		if (mesh)
		{
			const D3DXVECTOR3 &center = mesh->getBoundingSphereCenter();
			float radius = mesh->getBoundingSphereRadius();

			D3DXVECTOR3 centerInstance = center + obj->getPosition();
			if( Global::frustum.isBSphereVisible(centerInstance, radius) &&
				Global::backSideFrustum.isBSphereVisible(centerInstance, radius) ) {
					obj->render();
		}
		}
		++i;
	}
}


void LeversManager::updateAnimation(float deltaTime)
{
	iterator it = begin();
	while (it != end() ) 
	{
		LeverObject *lever = it->second;
		IAnimationController * cnt = lever->getAnimationController();
		assert(cnt);
		// if lever activation roa is not finished, update animation
		if( lever->getLeverActivated() && !cnt->isFinished() )
			it->second->updateAnimation(deltaTime);
		else if( lever->getLeverActivated() && cnt->isFinished() && !lever->getOpenedDoor() )
		{
			// if lever is activated and animation is finished, open door part
			lever->setOpenedDoor(true);
			LogicManager::getLogicManager()->createUserEvent("on_open_door_part");
		}
		++it;
	}
}

void LeversManager::activateLever( int leverId )
{
	// check if activate or not lever, only if the player is kicking
	Player *player = Player::getPlayer();
	bool playerKicking = player->isPlayerKicking();
	if(!playerKicking) return;


	iterator it = find(leverId);
	if( it == end() )
	{
		assert( !"[activateLever]::Lever Id not existing in LeversManager" );
	}

	if( Global::samplesEnabled )
	{
		SoundManager *soundManager = SoundManager::getSoundManager();
		soundManager->playSample("lever_metal_kick");
	}

	LeverObject *lever = it->second;
	bool leverActive = lever->getLeverActivated();
	if( !leverActive )
	{
		lever->setLeverActivated(true);

		char scriptNameTmp[ 256 ];
		sprintf_s(scriptNameTmp, sizeof(scriptNameTmp), "on_lever_opened_%d", leverId);
		std::string scriptName = scriptNameTmp;	

		LogicManager::getLogicManager()->createUserEvent(scriptName);
	}

	/*
 	IntelligentObject *lever = it->second;
	LeverAnimation *leverAnimation = (LeverAnimation *) lever->getAnimationController();
	bool leverActive = leverAnimation->isActivated();
	if( !leverActive )
	{
		lever->setRelationToState( ACTION, LEVER_MOVING, false, true);
		leverAnimation->activate();
	}
	*/
}

void LeversManager::cleanUp() 
{
	iterator it = begin();
	while(it != end())
	{	
		LeverObject *lever = (it->second);
		if(lever)
			delete lever, lever=NULL;
		++it;
	}
	clear();	
}

void LeversManager::reloadShader() 
{
	iterator it = begin();
	while( it != end() ) 
	{
		(it->second)->reloadShader();
		++it;
	}
}
