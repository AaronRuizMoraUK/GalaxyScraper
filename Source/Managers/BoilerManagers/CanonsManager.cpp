#include "CanonsManager.h"
#include "Managers/DataLocatorManager.h"
#include "Managers/PlanetManager.h"
#include "Managers/LogicManager/LogicManager.h"
#include "Managers/SoundManager.h"
#include "Global/GlobalVariables.h"
#include "Object/CollisionableObject/AnimatedObject/Player/Player.h"
#include "AnimationControllers/KeyFramed/KeyFramedROA.h"
#include <cassert>

CanonsManager * CanonsManager::canonsManager = NULL;

CanonsManager * CanonsManager::getCanonsManager() 
{
	if(canonsManager == NULL)
		canonsManager = new CanonsManager();

	return canonsManager;
}
 
CanonsManager::CanonsManager()
: currentParsed(NULL)
, currentParsedElementId(0)
, activateCanonAnimationResource(NULL)
, shootCanonAnimationResource(NULL)
{
	init();
}

void CanonsManager::init()
{
	DataLocatorManager *dataLocatorManager = DataLocatorManager::getDataLocatorManager();
	const std::string &pathToRoa = dataLocatorManager->getPath("roa");

	// Animation Resources
	activateCanonAnimationResource = new RigidAnimationResource();
	std::string filename = pathToRoa + "activate_canon.xml";
	bool ok = activateCanonAnimationResource->initFromFile( filename.c_str() );
	assert(ok);

	
	shootCanonAnimationResource = new RigidAnimationResource();
	filename = pathToRoa + "shoot_canon.xml";
	ok = shootCanonAnimationResource->initFromFile( filename.c_str() );
	assert(ok);
	

	const std::string &pathToXml = dataLocatorManager->getPath("xml");
	std::string xmlFile = pathToXml + "boiler/boiler_canons.xml";
	ok = xmlParseFile(xmlFile);
	assert(ok);
}

void CanonsManager::onEndElement (const std::string &elem)
{
	if( elem == "canon" )
	{
		// creating ROA instance
		D3DXQUATERNION rotationQuaternion;
		D3DXQuaternionRotationMatrix(&rotationQuaternion, &(currentParsed->getTransformMatrix()));
		
		KeyFramedROA *activateCanon = new KeyFramedROA( activateCanonAnimationResource, currentParsed->getPosition(), rotationQuaternion );
		currentParsed->setAnimationController( activateCanon );

		//KeyFramedROA *shootCanon = new KeyFramedROA( shootCanonAnimationResource, currentParsed->getPosition(), rotationQuaternion );

		/*
		// NOW COLLISIONS WITH STATIC BOXS
		// Create collision object
		ShapeHandle *collisionShapeHandle = 
			CollisionShapeFactory::getCollisionShapeFactory()->createCollisionShape("canon"
			, currentParsed->getMesh(), LEVER_SHAPE, true);
		assert(collisionShapeHandle);
		DT_ObjectHandle collisionObjectHandle = DT_CreateObject(currentParsed, collisionShapeHandle->shapeHandle);
		currentParsed->setCollisionShape(collisionShapeHandle);
		currentParsed->setCollisionObject(collisionObjectHandle);
		// Do know the setTransformMatrix for update also the collision position in world
		currentParsed->setTransformMatrix( currentParsed->getTransformMatrix() );
		*/

		(*this)[currentParsedElementId] = currentParsed;
		currentParsed = NULL;
	}
}

void CanonsManager::onStartElement (const std::string &elem, MKeyValue &atts) 
{
	if( currentParsed ) {
		createElement( elem, atts );
		return;
	}

	if( elem == "canon" )
	{
		currentParsed = new CanonObject();
		currentParsedElementId = atts.getInt( "canon_id", 0 );
	}
}

void CanonsManager::createElement(const std::string &elem, MKeyValue &atts) 
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
		currentParsed->setBaseCanonTransformMatrix( currentParsed->getTransformMatrix() );

		// Necesary to restart cannon
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

void CanonsManager::restart() {
	iterator i = begin();
	while(i!=end()) {
		CanonObject *canon = i->second;
		canon->restart();

		// Set the initial animation
		D3DXQUATERNION rotationQuaternion;
		D3DXQuaternionRotationMatrix(&rotationQuaternion, &(canon->getTransformMatrix()));
		
		KeyFramedROA *activateCanon = new KeyFramedROA( activateCanonAnimationResource, canon->getPosition(), rotationQuaternion );
		canon->setAnimationController( activateCanon );
		++i;
	}
}

void CanonsManager::render() const 
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


void CanonsManager::updateAnimation(float deltaTime)
{
	iterator it = begin();
	while (it != end() ) 
	{
		CanonObject *canon = it->second;
		IAnimationController * cnt = canon->getAnimationController();
		assert(cnt);
		// if lever activation roa is not finished, update animation
		if( canon->getCanonActivated() && !cnt->isFinished() )
			it->second->updateAnimation(deltaTime);
		else if( canon->getCanonActivated() && cnt->isFinished() && !canon->getCanonMoved() )
		{
			// if lever is activated and animation is finished, open door part
			canon->setCanonMoved(true);
			// LogicManager::getLogicManager()->createUserEvent("on_open_door_part");
		}
		else if (canon->getCanonShot() && !cnt->isFinished() )
			it->second->updateAnimation(deltaTime);
		else if (canon->getCanonShot() && cnt->isFinished() && !canon->getBoilerHit())
			canon->setBoilerHit(true);
		++it;
	}
}

void CanonsManager::activateCanon( int canonId )
{
	// check if activate or not lever, only if the player is kicking
	Player *player = Player::getPlayer();
	bool playerKicking = player->isPlayerKicking();
	if(!playerKicking) return;


	iterator it = find(canonId);
	if( it == end() )
	{
		assert( !"[activateCanon]::CanonID not existing in CanonManager" );
	}

	if( Global::samplesEnabled )
	{
		SoundManager *soundManager = SoundManager::getSoundManager();
		soundManager->playSample("lever_metal_kick");
	}

	CanonObject *canon = it->second;
	bool canonActive = canon->getCanonActivated();
	bool canonShot = canon->getCanonShot();
	bool canonMoved = canon->getCanonMoved();

	if( !canonActive )
	{
		canon->setCanonActivated(true);

		char scriptNameTmp[ 256 ];
		sprintf_s(scriptNameTmp, sizeof(scriptNameTmp), "on_canon_opened_%d", canonId);
		std::string scriptName = scriptNameTmp;	

		LogicManager::getLogicManager()->createUserEvent(scriptName);
	}
	else if (canonMoved && !canonShot)
	{
		canon->setCanonShot(true);

		char scriptNameTmp[ 256 ];
		sprintf_s(scriptNameTmp, sizeof(scriptNameTmp), "on_canon_shot_%d", canonId);
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

void CanonsManager::changeAnimationController(int canonId)
{
	iterator it = find(canonId);
	if( it == end() )
	{
		assert( !"[activateCanon]::CanonID not existing in CanonManager" );
	}

	CanonObject *canon = it->second;

	D3DXQUATERNION rotationQuaternion;
	D3DXQuaternionRotationMatrix(&rotationQuaternion, &(canon->getTransformMatrix()));

	KeyFramedROA *shootCanon = new KeyFramedROA( shootCanonAnimationResource, canon->getPosition(), rotationQuaternion );

	canon->setAnimationController(shootCanon);

	
}

void CanonsManager::cleanUp() 
{
	iterator it = begin();
	while(it != end())
	{	
		CanonObject *canon = (it->second);
		if(canon)
			delete canon, canon=NULL;
		++it;
	}
	clear();	
}

void CanonsManager::reloadShader() 
{
	iterator it = begin();
	while( it != end() ) 
	{
		(it->second)->reloadShader();
		++it;
	}
}
