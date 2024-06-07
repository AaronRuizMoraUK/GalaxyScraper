#include "IuranusPlanetElementsManager.h"
#include "Global/GlobalFunctions.h"
#include "Global/GlobalVariables.h"
#include "Factories/MeshFactory.h"
#include "Factories/CalCoreModelFactory.h"
#include "Factories/AnimationStateGraphFactory.h"
#include "AnimationControllers/AssAnimation/AssAnimation.h"
#include "AnimationControllers/MouthAnimation/MouthAnimation.h"
#include "AnimationControllers/TongueAnimation/TongueAnimation.h"
#include "AIControllers/IAISlotController.h"
#include "AIControllers/MouthBehaviour/MouthBehaviour.h"
#include "AIControllers/AssBehaviour/AssBehaviour.h"
#include "Managers/PlanetManager.h"
#include "Managers/DataLocatorManager.h"
#include <fstream>
#include <cassert>

IuranusPlanetElementsManager * IuranusPlanetElementsManager::iuranusPlanetElementsManager = NULL;

IuranusPlanetElementsManager * IuranusPlanetElementsManager::getIuranusPlanetElementsManager() {
	if(iuranusPlanetElementsManager == NULL)
		iuranusPlanetElementsManager = new IuranusPlanetElementsManager();

	return iuranusPlanetElementsManager;
}

IuranusPlanetElementsManager::IuranusPlanetElementsManager()
: currentParsed(NULL)
, currentParsedElementName("")
, mouthRadius(0.0f)
, assRadius(0.0f)
{
	init();
}

void IuranusPlanetElementsManager::init()
{
	DataLocatorManager *dataLocatorManager = DataLocatorManager::getDataLocatorManager();
	const std::string &pathToXml = dataLocatorManager->getPath("xml");

	std::string xmlFile = pathToXml + "iuranus/iuranus_elements.xml";
	bool ok = xmlParseFile(xmlFile);
	assert(ok);
}

void IuranusPlanetElementsManager::onEndElement (const std::string &elem)
{
	if( elem == "element" )
	{
		(*this)[currentParsedElementName] = currentParsed;
		currentParsed = NULL;
	}
}

void IuranusPlanetElementsManager::onStartElement (const std::string &elem, MKeyValue &atts) 
{
	if( currentParsed ) {
		createElement( elem, atts );
		return;
	}

	if( elem == "element" )
	{
		currentParsed = new IntelligentObject();
		currentParsedElementName = atts["name"];
	}
}

void IuranusPlanetElementsManager::createElement(const std::string &elem, MKeyValue &atts) 
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
	else if( elem == "material" )
	{
		std::string texture = atts["name"];
		std::string shader = atts["shader"];
		std::string technique = atts["technique"];
		currentParsed->SETMATERIAL(texture, shader, technique);
 	}
	else if( elem == "radius" )
	{
		float radius = atts.getFloat( "value", 0.0f );

		if( currentParsedElementName == "mouth" )
			mouthRadius = radius;
		else if( currentParsedElementName == "ass" )
			assRadius = radius;
	}
	else if( elem == "ai_controller" )
	{
		IAIController *aiController = NULL;
		
		std::string aiControllerName = atts["name"];
		int numSlots = atts.getInt( "num_slots", 0 );

		if( aiControllerName == "mouth_behaviour" )
			aiController = new MouthBehaviour(numSlots);
		else if (aiControllerName == "ass_behaviour")
			aiController = new AssBehaviour(numSlots);
		else {
			assert( !"Not type of AI controller allowed in PlanetElementManager" );
			return;
		}
		
		if (aiController)
			currentParsed->setAIController(aiController);
	}
	else if( elem == "animation_controller" )
	{
		IAnimationController *animationController = NULL;

		std::string animControllerName = atts["name"];

		if( animControllerName=="mouth_animation")
			animationController = new MouthAnimation();
		else if( animControllerName == "ass_animation" )
			animationController = new AssAnimation();
		else if(animControllerName=="tongue_animation" )
			animationController = new TongueAnimation();
		else {
			assert( !"Not type of animation constroller allowed in IuranusPlanetElementsManager" );
			return;
		}

		if( animationController )
			currentParsed->setAnimationController(animationController);
	}
}

void IuranusPlanetElementsManager::updateAnimation(float deltaTime) 
{
	iterator it = begin();

	while (it != end() ) 
	{
		(it->second)->updateAnimation(deltaTime);
		++it;
	}
}

void IuranusPlanetElementsManager::updateAI(float deltaTime) 
{
	iterator it = begin();

	while (it != end() ) 
	{
		(it->second)->updateAI(deltaTime);
		++it;
	}
}

void IuranusPlanetElementsManager::render() const 
{
	const_iterator i = begin();

#ifdef DEBUG
	bool mouthDrawn = false;
	bool assDrawn = false;
#endif

	while(i!=end()) {
		const Object *obj = i->second;

		const IMesh * mesh = obj->getMesh();
		const D3DXVECTOR3 &center = mesh->getBoundingSphereCenter();
		float radius = mesh->getBoundingSphereRadius();

		D3DXVECTOR3 centerInstance = center + obj->getPosition();
		if( Global::frustum.isBSphereVisible(centerInstance, radius) &&
			Global::backSideFrustum.isBSphereVisible(centerInstance, radius) ) 
		{
#ifdef DEBUG
			if(i->first == "mouth")
				mouthDrawn = true;
			else if(i->first == "ass")
				assDrawn = true;
#endif
			obj->render();
		}
		++i;
	}

#ifdef DEBUG
	if(mouthDrawn)
		Global::print(0,50, 0xffff0000, "Mouth drawn");
	if(assDrawn && !mouthDrawn)
		Global::print(0, 50, 0xffff0000, "Ass drawn");
	else if(assDrawn && mouthDrawn)
		Global::print(0, 60, 0xffff0000, "Ass drawn");
#endif
}

void IuranusPlanetElementsManager::cleanUp() 
{
	iterator it = begin();
	while(it != end())
	{	
		IntelligentObject *intObj = (it->second);
		if(intObj)
			delete intObj, intObj=NULL;
		++it;
	}
	clear();
}

IntelligentObject * IuranusPlanetElementsManager::getElement(const std::string &elem) const
{
	const_iterator it = find(elem);
	if( it == end() ) {
		assert( !"Element not fount in IuranusPlanetElementsManager" );
		return NULL;
	}

	return it->second;
}

float IuranusPlanetElementsManager::getElementRadius(const std::string &elem) const
{
	if(elem == "mouth")
		return mouthRadius;
	else if(elem == "ass")
		return assRadius;
	else {
		assert( !"Radius is not getted in getElementRadius() in IuranusPlanetElementsManager" );
		return -1;
	}
}

Slot * IuranusPlanetElementsManager::getFreeElementSlot(const std::string &elem) const
{
	const_iterator it = find(elem);
	if( it == end() ) {
		assert( !"Element not found in IuranusPlanetElementsManager" );
		return NULL;
	}

	IAISlotController *aiSlotController = (IAISlotController *) it->second->getAIController();
	return aiSlotController->getFreeSlot();
}

unsigned int IuranusPlanetElementsManager::getElementSlotIndex(const std::string &elem, Slot *s) const
{
	const_iterator it = find(elem);
	if( it == end() ) {
		assert( !"Element not fount in IuranusPlanetElementsManager" );
		return -1;
	}

	IAISlotController *aiSlotController = (IAISlotController *) it->second->getAIController();
	return aiSlotController->getSlotIndex(s);
}

Slot * IuranusPlanetElementsManager::getElementSlotByIndex(const std::string &elem, unsigned int i) const
{
	const_iterator it = find(elem);
	if( it == end() ) {
		assert( !"Element not fount in IuranusPlanetElementsManager" );
		return NULL;
	}

	IAISlotController *aiSlotController = (IAISlotController *) it->second->getAIController();
	return aiSlotController->getSlotByIndex(i);
}

void IuranusPlanetElementsManager::animateAss(ActionType type, const std::string &animation)
{
	iterator it = find("ass");
	if (it == end()) {
		assert( !"Element not fount in IuranusPlanetElementsManager" );
		return;
	}
	(it->second)->setRelationToState(type, animation, false, true);

	it = find("ass_inside");
	if (it == end()) {
		assert( !"Element not fount in IuranusPlanetElementsManager" );
		return;
	}
	(it->second)->setRelationToState(type, animation, false, true);
}

void IuranusPlanetElementsManager::animateMouth(ActionType type, const std::string &animation)
{
	iterator it = find("mouth");
	if (it == end()) {
		assert( !"Element not fount in IuranusPlanetElementsManager" );
		return;
	}
	(it->second)->setRelationToState(type, animation, false, true);

	it = find("land");
	if (it == end()) {
		assert( !"Element not fount in IuranusPlanetElementsManager" );
		return;
	}
	(it->second)->setRelationToState(type, animation, false, true);
}

void IuranusPlanetElementsManager::animateTongue(ActionType type, const std::string &animation)
{
	iterator it = find("tongue");
	if (it == end()) {
		assert( !"Element not fount in IuranusPlanetElementsManager" );
		return;
	}
	(it->second)->setRelationToState(type, animation, false, true);
}

void IuranusPlanetElementsManager::reloadShader() {
	iterator it = begin();
	while( it != end() ) {
		(it->second)->reloadShader();
		++it;
	}
}
