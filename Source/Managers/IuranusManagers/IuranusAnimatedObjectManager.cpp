#include "IuranusAnimatedObjectManager.h"
#include "Object/CollisionableObject/AnimatedObject/AnimatedObject.h"
#include "Global/GlobalFunctions.h"
#include "Global/GlobalVariables.h"
#include "Factories/MeshFactory.h"
#include "Factories/MeshFactory.h"
#include "Factories/CalCoreModelFactory.h"
#include "Factories/AnimationStateGraphFactory.h"
#include "AnimationControllers/BushAnimation/BushAnimation.h"
#include "Managers/PlanetManager.h"
#include "Managers/DataLocatorManager.h"
#include "AnimationControllers/KeyFramed/KeyFramedROA.h"
#include <fstream>
#include <cassert>

IuranusAnimatedObjectManager * IuranusAnimatedObjectManager::iuranusAnimatedObjectManager = NULL;

IuranusAnimatedObjectManager * IuranusAnimatedObjectManager::getIuranusAnimatedObjectManager() {
	if(iuranusAnimatedObjectManager == NULL)
		iuranusAnimatedObjectManager = new IuranusAnimatedObjectManager();

	return iuranusAnimatedObjectManager;
}
 
IuranusAnimatedObjectManager::IuranusAnimatedObjectManager()
: currentParsed(NULL)
, currentParsedElementName("")
, bushResource(NULL)
{
	init();
}

void IuranusAnimatedObjectManager::init()
{
	DataLocatorManager *dataLocatorManager = DataLocatorManager::getDataLocatorManager();

	// Animation Resources
	const std::string &pathToRoa = dataLocatorManager->getPath("roa");
	std::string filename = pathToRoa + "bush.xml";
	bushResource = new RigidAnimationResource();
	bool ok = bushResource->initFromFile(filename.c_str());
	assert(ok);

	const std::string &pathToXml = dataLocatorManager->getPath("xml");
	std::string xmlFile = pathToXml + "/iuranus/iuranus_animated_objects.xml";
	ok = xmlParseFile(xmlFile);
	assert(ok);
}

void IuranusAnimatedObjectManager::onEndElement (const std::string &elem)
{
	if( elem == "animated_object" )
	{
		(*this)[currentParsedElementName] = currentParsed;
		// push_back(currentParsed);
		currentParsed = NULL;
	}
}

void IuranusAnimatedObjectManager::onStartElement (const std::string &elem, MKeyValue &atts) 
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

void IuranusAnimatedObjectManager::createElement(const std::string &elem, MKeyValue &atts) 
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

		const Planet *currentPlanet = PlanetManager::getPlanetManager( )->getCurrentPlanet( );
		currentParsed->setParentObject( currentPlanet ); // The parent is the current planet
		currentParsed->setLocalTransformMatrix(matrix); // This is the local transform matrix, it never changes
		currentParsed->updateTransformMatrix( ); // This updates its transform matrix too
	}
	else if( elem == "mesh" )
	{
		std::string type = atts["type"];
		std::string name = atts["name"];

		// If mesh is cal3d
		if( type == "cal3d" ) {
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
		}
		// If mesh is custom
		else if( type == "custom" ) {
			IMesh *mesh = MeshFactory::getMeshFactory()->createMesh( name );
			assert( mesh );
			currentParsed->setMesh( mesh );

			// Animation controller ROA
			D3DXQUATERNION rotationQuaternion;
			D3DXQuaternionRotationMatrix(&rotationQuaternion, &currentParsed->getTransformMatrix());
			KeyFramedROA *bushAnimation = new KeyFramedROA( bushResource, currentParsed->getPosition(), rotationQuaternion);
			currentParsed->setAnimationController(bushAnimation);
		}
		else {
			assert( !"Type of mesh unknown in IuranusAnimatedObjectManager" );
			return;
		}
	}
	else if( elem == "material" )
	{
		std::string texture = atts["name"];
		std::string shader = atts["shader"];
		std::string technique = atts["technique"];
		currentParsed->SETMATERIAL(texture, shader, technique);
 	}
	else if( elem == "animation_controller" )
	{

		std::string animControllerName = atts["name"];
		IAnimationController *animationController = NULL;

		if( animControllerName=="bush_animation" ) {
			animationController = new BushAnimation();
		}
		else {
			assert( !"Not type of animation constroller allowed in IuranusAnimatedObjectManager" );
			return;
		}
		
		if( animationController )
			currentParsed->setAnimationController(animationController);
	}
}

void IuranusAnimatedObjectManager::updateAnimation(float deltaTime)
{
	iterator it = begin();
	while (it != end() ) 
	{
		AnimatedObject *object = it->second;
		const CalModel * calModel = object->getCalModel();

		if( calModel != NULL )
			object->updateAnimation(deltaTime);
		else {
			KeyFramedROA * objectAnimationController = (KeyFramedROA *) object->getAnimationController();
			assert( objectAnimationController );
			if( !objectAnimationController->isFinished() )
				object->updateAnimation(deltaTime);
		}
		++it;
	}
}

void IuranusAnimatedObjectManager::render() const {
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

void IuranusAnimatedObjectManager::cleanUp() {
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

void IuranusAnimatedObjectManager::moveObject(const std::string &name)
{
	AnimatedObject *object = getAnimatedObject( name );

	const CalModel * calModel = object->getCalModel();

	if( calModel != NULL ) {
		object->setRelationToState(ACTION, "moving", false, true);
	}
	else {
		KeyFramedROA * objectAnimationController = (KeyFramedROA *) object->getAnimationController();
		assert( objectAnimationController );
		if( objectAnimationController->isFinished() ) {
			D3DXQUATERNION rotationQuaternion;
			D3DXQuaternionRotationMatrix(&rotationQuaternion, &object->getTransformMatrix());
			objectAnimationController->restart( object->getPosition(), rotationQuaternion );
		}
	}
}

AnimatedObject * IuranusAnimatedObjectManager::getAnimatedObject( const std::string &name ) const {
	const_iterator it = find( name );
	if( it == end() ) {
		assert( !"Iuranus animated object that not exist in IuranusAnimatedObjectManager" );
		return NULL;
	}

	return it->second;
}

void IuranusAnimatedObjectManager::reloadShader() {
	iterator it = begin();

	while( it != end() ) {
		it->second->reloadShader();
		++it;
	}
}
