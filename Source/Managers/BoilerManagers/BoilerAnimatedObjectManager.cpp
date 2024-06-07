#include "BoilerAnimatedObjectManager.h"
#include "Global/GlobalFunctions.h"
#include "Global/GlobalVariables.h"
#include "Factories/MeshFactory.h"
#include "Managers/PlanetManager.h"
#include "Managers/DataLocatorManager.h"
#include "Managers/SoundManager.h"
#include "AnimationControllers/KeyFramed/KeyFramedROA.h"
#include <cassert>

BoilerAnimatedObjectManager * BoilerAnimatedObjectManager::boilerAnimatedObjectManager = NULL;

DOOR_ANIMATIONS BoilerAnimatedObjectManager::doorAnimationSequence[numberDoorAnimations+1] = {DOOR_ANIM_SHORT, DOOR_ANIM_SHORT, DOOR_ANIM_LONG, DOOR_NO_ANIM};

BoilerAnimatedObjectManager * BoilerAnimatedObjectManager::getBoilerAnimatedObjectManager() {
	if(boilerAnimatedObjectManager == NULL)
		boilerAnimatedObjectManager = new BoilerAnimatedObjectManager();

	return boilerAnimatedObjectManager;
}
 
BoilerAnimatedObjectManager::BoilerAnimatedObjectManager()
: currentParsed(NULL)
, currentParsedElementName("")
, doorDownShortResource( NULL )
, doorDownLongResource( NULL )
, currentDoorAnimation( 0 )
{
	D3DXMatrixIdentity(&doorInitialTransformMatrix);
	init();
}

void BoilerAnimatedObjectManager::init()
{
	DataLocatorManager *dataLocatorManager = DataLocatorManager::getDataLocatorManager();
	const std::string &pathToRoa = dataLocatorManager->getPath("roa");

	// Animation Resources
	doorDownShortResource = new RigidAnimationResource();
	std::string filename = pathToRoa + "door_short.xml";
	bool ok = doorDownShortResource->initFromFile( filename.c_str() );
	assert(ok);

	doorDownLongResource = new RigidAnimationResource();
	filename = pathToRoa + "door_long.xml";
	ok = doorDownLongResource->initFromFile( filename.c_str() );
	assert(ok);

	const std::string &pathToXml = dataLocatorManager->getPath("xml");

	std::string xmlFile = pathToXml + "/boiler/boiler_animated_objects.xml";
	ok = xmlParseFile(xmlFile);
	assert(ok);
}

void BoilerAnimatedObjectManager::onEndElement (const std::string &elem)
{
	if( elem == "animated_object" )
	{
		(*this)[currentParsedElementName] = currentParsed;
		// push_back(currentParsed);
		currentParsed = NULL;
	}
}

void BoilerAnimatedObjectManager::onStartElement (const std::string &elem, MKeyValue &atts) 
{
	if( currentParsed ) {
		createElement( elem, atts );
		return;
	}

	if( elem == "animated_object" )
	{
		currentParsed = new AnimatedObject();
		currentParsedElementName = atts["name"];
	}
}

void BoilerAnimatedObjectManager::createElement(const std::string &elem, MKeyValue &atts) 
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

		//currentParsed->setTransformMatrix(matrix);

		const Planet *currentPlanet = PlanetManager::getPlanetManager()->getPlanet("boiler");
		currentParsed->setParentObject( currentPlanet ); // The parent is the current planet
		currentParsed->setLocalTransformMatrix(matrix); // This is the local transform matrix, it never changes
		currentParsed->updateTransformMatrix( ); // This updates its transform matrix too

		// Save the initial position of door
		if( currentParsedElementName == "door" ) {
			doorInitialTransformMatrix = currentParsed->getTransformMatrix();
		}
	}
	else if( elem == "mesh" )
	{
		std::string type = atts["type"];
		std::string name = atts["name"];

		if( type == "cal3d" ) {
			assert( !"Cal3d is pending" );
			return;
			/*
			std::string graph  = atts["graph"];
			std::string initialCycle = atts["initial_cycle"];
			std::string initialAction = atts["initial_action"];

			CalAnimModel *calAnimModel = NULL;
			CalAnimCoreModel *calAnimCoreModel = 
				CalCoreModelFactory::getCalCoreModelFactory()->createCalAnimCoreModel(name);
			assert(calAnimCoreModel);

			calAnimModel = new CalAnimModel();
			calAnimModel->meshId = calAnimCoreModel->meshId;
			calAnimModel->idAnimMap = calAnimCoreModel->idAnimMap;
			calAnimModel->calModel = new CalModel(calAnimCoreModel->calCoreModel);
			calAnimModel->calModel->attachMesh(calAnimModel->meshId);

			currentParsed->setCalAnimModel(calAnimModel);
			// intelligentObject->getCalModel()->getMixer()->blendCycle(0, 1.0f, 0.0f);

			CalCoreMesh *calCoreMesh = calAnimModel->calModel->getMesh(calAnimModel->meshId)->getCoreMesh();
			IMesh * calMesh = MeshFactory::getMeshFactory()->createMesh(name, calCoreMesh);
			assert( calMesh );
			currentParsed->setMesh(calMesh);
			
			AnimationStates *animationStates = AnimationStateGraphFactory::getAnimationStateGraphFactory()->createAnimationStateGraph(graph);
			assert(animationStates);
			if(animationStates!=NULL) {
				AnimationState *initialState = animationStates->findState(initialCycle, initialAction);
				assert(initialState);
				if(initialState!=NULL)
					currentParsed->setCurrentAnimationState(initialState);
			}

			calAnimModel->calModel->getMixer()->blendCycle(0, 1.0f, 0.0f);
			*/
		}
		else if( type=="custom" ) {
			IMesh *mesh = MeshFactory::getMeshFactory()->createMesh(name);
			assert(mesh);
			currentParsed->setMesh(mesh);
		}
		else {
			assert( !"This should not happend" );
			return;
		}

		// Collision
		ShapeHandle *collisionShapeHandle = 
			CollisionShapeFactory::getCollisionShapeFactory()->createCollisionShape("boiler_anim_objs"
			, currentParsed->getMesh(), DOOR_SHAPE, false);
		assert(collisionShapeHandle);
		DT_ObjectHandle collisionObjectHandle = DT_CreateObject(currentParsed, collisionShapeHandle->shapeHandle);
		currentParsed->setCollisionShape(collisionShapeHandle);
		currentParsed->setCollisionObject(collisionObjectHandle);
		// Do know the setTransformMatrix for update also the collision position in world
		currentParsed->setTransformMatrix( currentParsed->getTransformMatrix() );
	}
	else if( elem == "material" )
	{
		std::string texture = atts["name"];
		std::string shader = atts["shader"];
		std::string technique = atts["technique"];
		currentParsed->SETMATERIAL(texture, shader, technique);
 	}
}

void BoilerAnimatedObjectManager::restart() 
{
	AnimatedObject *door = getAnimatedObject( "door" );
	door->setTransformMatrix( doorInitialTransformMatrix );
	door->setAnimationController( NULL );
	currentDoorAnimation = 0;
}

void BoilerAnimatedObjectManager::updateAnimation(float deltaTime)
{
	iterator it = begin();
	while (it != end() ) 
	{
		// Update door animation
		if( it->second->getAnimationController() && !it->second->getAnimationController()->isFinished() )
			it->second->updateAnimation( deltaTime );
		++it;
	}
}

void BoilerAnimatedObjectManager::doNextDoorAnimation( ) {
	AnimatedObject *door = getAnimatedObject( "door" );

	DOOR_ANIMATIONS door_animation_now = doorAnimationSequence[currentDoorAnimation];
	assert( door_animation_now != DOOR_NO_ANIM );

	// creating ROA instance
	D3DXQUATERNION rotationQuaternion;
	D3DXQuaternionRotationMatrix(&rotationQuaternion, &(door->getTransformMatrix()));

	if( door_animation_now == DOOR_ANIM_SHORT ) 
	{
		KeyFramedROA *doorDownShort = new KeyFramedROA( doorDownShortResource, door->getPosition(), rotationQuaternion );
		door->setAnimationController( doorDownShort );

		if( Global::samplesEnabled )
		{	
			SoundManager *soundManager = SoundManager::getSoundManager();
			soundManager->playSample("open_door_short");
			soundManager->playSample("open_door_short_earthquake");
		}
	}
	else if ( door_animation_now == DOOR_ANIM_LONG ) 
	{
		KeyFramedROA *doorDownLong = new KeyFramedROA( doorDownLongResource, door->getPosition(), rotationQuaternion );
		door->setAnimationController( doorDownLong );

		if( Global::samplesEnabled )
		{
			SoundManager *soundManager = SoundManager::getSoundManager();
			soundManager->playSample("open_door_long");
			soundManager->playSample("open_door_long_earthquake");
		}
	}
	else {
		assert( !"This should not happend" );
		return;
	}

	// Next animaxion
	currentDoorAnimation++;
	if( doorAnimationSequence[currentDoorAnimation] == DOOR_NO_ANIM )
		currentDoorAnimation = 0;
}

void BoilerAnimatedObjectManager::render() const {
	const_iterator i = begin();

#ifdef DEBUG
	int staticElementsDrawn = 0;
#endif

	while(i!=end()) {
		const Object *obj = i->second;

		const IMesh * mesh = obj->getMesh();
		const D3DXVECTOR3 &center = mesh->getBoundingSphereCenter();
		float radius = mesh->getBoundingSphereRadius();

		D3DXVECTOR3 centerInstance = center + obj->getPosition();
		if( Global::frustum.isBSphereVisible(centerInstance, radius) &&
			Global::backSideFrustum.isBSphereVisible(centerInstance, radius) ) {
			obj->render();
		}
		++i;
	}

#ifdef DEBUG
	Global::print(0,40, 0xffff0000, "Animated objects drawn: %d", staticElementsDrawn);
#endif 
}

void BoilerAnimatedObjectManager::cleanUp() {
	iterator it = begin();
	while(it != end())
	{	
		AnimatedObject *animatedObject = (it->second);
		if(animatedObject)
			delete animatedObject, animatedObject=NULL;
		++it;
	}
	clear();
}

AnimatedObject * BoilerAnimatedObjectManager::getAnimatedObject( const std::string &name ) const {
	const_iterator it = find( name );
	if( it == end() ) {
		assert( !"Iuranus animated object that not exist in BoilerAnimatedObjectManager" );
		return NULL;
	}

	return it->second;
}

void BoilerAnimatedObjectManager::reloadShader() {
	iterator it = begin();

	while( it != end() ) {
		it->second->reloadShader();
		++it;
	}
}
