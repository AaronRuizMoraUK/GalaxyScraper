#include "BoilerEnemyManager.h"
#include "Managers/DataLocatorManager.h"
#include "Managers/PlanetManager.h"
#include "Factories/CalCoreModelFactory.h"
#include "Factories/AnimationStateGraphFactory.h"
#include "AnimationControllers/BoilerAnimation/BoilerAnimation.h"
#include "AIControllers/BoilerBehaviour/BoilerBehaviour.h"
#include <cassert>

BoilerEnemyManager * BoilerEnemyManager::boilerEnemyManager = NULL;

BoilerEnemyManager * BoilerEnemyManager::getBoilerEnemyManager() {
	if(boilerEnemyManager == NULL)
		boilerEnemyManager = new BoilerEnemyManager();

	return boilerEnemyManager;
}
 
BoilerEnemyManager::BoilerEnemyManager()
: currentParsed( NULL )
, currentParsedElementName( "" )
, currentFlameZone( NULL )
, currentFlameZoneId(0)
, currentFlameTimeSequence(0.0f)
, currentNumFlamesZones(0)
{
	init();
}

void BoilerEnemyManager::init()
{
	DataLocatorManager *dataLocatorManager = DataLocatorManager::getDataLocatorManager();
	const std::string &pathToXml = dataLocatorManager->getPath("xml");

	std::string xmlFile = pathToXml + "/boiler/boiler_enemies.xml";
	bool ok = xmlParseFile(xmlFile);
	assert(ok);
}

void BoilerEnemyManager::onEndElement (const std::string &elem)
{
	if( elem == "boiler_enemy" )
	{
		(*this)[currentParsedElementName] = currentParsed;
		currentParsed = NULL;
		currentParsedElementName = "";
	}
	else if( elem == "flame_zone" )
	{
		flameZones[currentFlameZoneId] = currentFlameZone;
		currentFlameZoneId = 0;
		currentFlameZone = NULL;
	}
	else if( elem == "ai_controller" )
	{
		IAIController * aiController;
		aiController = new BoilerBehaviour(flameZones, currentFlameTimeSequence, currentNumFlamesZones);
		if ( aiController )
			currentParsed->setAIController(aiController);
		currentFlameTimeSequence = 0.0f;
		flameZones.clear();
	}
}

void BoilerEnemyManager::onStartElement (const std::string &elem, MKeyValue &atts) 
{
	/*
	if( currentFlameZone )
	{
		createFlameZones( elem, atts );
		return;
	}
	*/

	if( elem == "flame_zone" )
	{
		currentFlameZoneId = atts.getInt("id", 0);
		currentFlameZone = new FlameZone();
		currentFlameZone->flameZoneId = currentFlameZoneId;
		currentFlameZone->boilerFirePlatId = atts.getInt("boiler_fire_plat_id", 0);
		currentFlameZone->numFlamePlatforms = atts.getInt("num_flame_platforms", 0);
		currentFlameZone->timeoutFlameZoneDuration = atts.getFloat("duration", 0.0f);
		currentFlameZone->timeoutFlameInterval = atts.getFloat("flames_interval", 0.0f);
		return;
	}
	else if( elem == "flame_platform" )
	{
		assert( currentFlameZone );
		currentFlameZone->onStartElement( elem, atts );
		return;
	}

	if( currentParsed ) 
	{
		createElement( elem, atts );
		return;
	}

	if( elem == "boiler_enemy" )
	{
		currentParsed = new IntelligentObject();
		currentParsedElementName = atts["name"];
	}
}

void BoilerEnemyManager::createElement(const std::string &elem, MKeyValue &atts) 
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

		currentParsed->setTransformMatrix(matrix);

		const Planet *planet = PlanetManager::getPlanetManager()->getPlanet( "boiler" );
		const D3DXVECTOR3 &planet_center = planet->getCenter();
		currentParsed->setPosition( currentParsed->getPosition() + planet_center );
	}
	else if( elem == "mesh" )
	{
		std::string type = atts["type"];
		std::string name = atts["name"];
		std::string graph  = atts["graph"];
		std::string initialCycle = atts["initial_cycle"];
		std::string initialAction = atts["initial_action"];

		if( type == "cal3d" ) {
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
		else {
			assert( !"Not supported mesh in BoilerEnemyManager" );
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
	else if( elem == "ai_controller" )
	{
		//IAIController * aiController;
		std::string aiControllerType = atts["type"];

		if( aiControllerType == "boiler_behaviour" )
		{
			//aiController = new BoilerBehaviour();
			currentFlameTimeSequence = atts.getFloat("next_flame_start_time", 0.0f);
			currentNumFlamesZones = atts.getInt("num_flame_zones", 0);
			//currentFlameZone = &FlameZone();

		}
		else {
			assert( !"Not type of AI controller allowed in BoilerEnemyManager" );
			return;
		}

		//if ( aiController )
		//	currentParsed->setAIController(aiController);
	}
	else if (elem=="animation_controller") 
	{

		IAnimationController *animationController = NULL;	
		std::string animController=atts["type"];

		if( animController == "boiler_animation" ) {
			animationController = new BoilerAnimation( );
		}
		else {
			assert( !"Not type of animation controller allowed in IuranusEnemyManager" );
			return;
		}

		if ( animationController )
			currentParsed->setAnimationController(animationController);
	}
}

void BoilerEnemyManager::updateAnimation(float deltaTime)
{
	/*
	iterator it = begin();
	while (it != end() ) 
	{
		it->second->updateAnimation(deltaTime);
		++it;
	}
	*/
}

void BoilerEnemyManager::updateAI(float deltaTime)
{
	iterator it = begin();
	while (it != end() ) 
	{
		it->second->updateAI(deltaTime);
		++it;
	}
}


void BoilerEnemyManager::render() const 
{
	/*
	const_iterator i = begin();

	while(i!=end()) {
		i->second->render();

		++i;
	}
	*/
}

void BoilerEnemyManager::cleanUp() 
{
	iterator it = begin();
	while(it != end())
	{	
		IntelligentObject *intelligentObject = (it->second);
		if( intelligentObject )
			delete intelligentObject, intelligentObject=NULL;
		++it;
	}
	clear();
}

void BoilerEnemyManager::reloadShader() 
{
	iterator it = begin();

	while( it != end() ) {
		it->second->reloadShader();
		++it;
	}
}


void BoilerEnemyManager::initEnemyFlameZone(const std::string &enemyName, int flameZoneId)
{
	const_iterator it = find( enemyName );
	if( it == end() ) 
	{
		assert( !"[initEnemyFlameZone] Boiler Enemy object that not exist in BoilerEnemyManager" );
	}
	else
	{
		BoilerBehaviour *boilerBehaviour = (BoilerBehaviour *) (it->second)->getAIController();
		boilerBehaviour->initFlameZone( flameZoneId );
	}
}

void BoilerEnemyManager::initEnemyFlamesSequence(const std::string &enemyName)
{
	const_iterator it = find( enemyName );
	if( it == end() )
	{
		assert( !"[initEnemyFlamesSequence] Boiler Enemy object that not exist in BoilerEnemyManager" );
	}
	else
	{
		BoilerBehaviour *boilerBehaviour = (BoilerBehaviour *) (it->second)->getAIController();
		boilerBehaviour->initFlamesSequence();
	}
}

void BoilerEnemyManager::killEnemyFlameZone(const std::string &enemyName, int flameZoneId)
{
	const_iterator it = find( enemyName );
	if( it == end() )
	{
		assert( !"[killEnemyFlameZone] Boiler Enemy object that not exist in BoilerEnemyManager" );
	}
	else
	{
		BoilerBehaviour *boilerBehaviour = (BoilerBehaviour *) (it->second)->getAIController();
		boilerBehaviour->killFlameZone( flameZoneId );
	}
}

void BoilerEnemyManager::restart()
{
	iterator it = begin();
	while(it != end())
	{	
		BoilerBehaviour *boilerBehaviour = (BoilerBehaviour *) (it->second)->getAIController();
		boilerBehaviour->restart();
		++it;
	}
	
}