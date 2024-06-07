#include "MenuState.h"
#include "Global/GlobalFunctions.h"
#include "Global/GlobalVariables.h"
#include "Source/Game.h"
#include "Factories/QuadFactory.h"
#include "Managers/DataLocatorManager.h"
#include "AnimationControllers/KeyFramed/KeyFramedROA.h"
#include "Object/CollisionableObject/AnimatedObject/Player/Player.h"
#include "Managers/IuranusManagers/IuranusEnemyManager.h"
#include "Managers/CollisionManager/IuranusCollisionManager.h"
#include "Managers/CollisionManager/BoilerCollisionManager.h"
#include "Managers/LogicManager/LogicManager.h"
#include "Managers/SoundManager.h"
#include <cassert>

std::string MenuState::name = "menu_state";

MenuState::MenuState()
: IGameState()
, doOnLoad(true)
, menuStateAction(START)
, inPlanet(false)
, inStartAction(false)
, inOptionsAction(false)
, inCreditsAction(false)
, inExitAction(false)
, nextAnimationResource(NULL)
, previousAnimationResource(NULL)
, inSky(false)
, inTitle(false)
{
}

void MenuState::onStartElement (const std::string &elem, MKeyValue &atts) {
	if( inPlanet ) {
		createPlanet(elem, atts);
		return;
	}
	else if( inStartAction ) {
		createAction(startAction, elem, atts);
		return;
	}
	else if( inOptionsAction ) {
		createAction(optionsAction, elem, atts);
		return;
	}
	else if( inCreditsAction ) {
		createAction(creditsAction, elem, atts);
		return;
	}
	else if( inExitAction ) {
		createAction(exitAction, elem, atts);
		return;
	}
	else if( inSky ) {
		createSky(elem, atts);
		return;
	}
	else if( inTitle ) {
		createTitle(elem, atts);
		return;
	}

	if( elem == "planet" )
		inPlanet=true;
	else if( elem == "start_action" )
		inStartAction=true;
	else if( elem == "options_action" )
		inOptionsAction=true;
	else if( elem == "credits_action" )
		inCreditsAction=true;
	else if( elem == "exit_action" )
		inExitAction=true;
	else if( elem == "sky" )
		inSky=true;
	else if( elem == "title" )
		inTitle=true;
}

void MenuState::onEndElement (const std::string &elem) {
	if( elem == "planet" )
		inPlanet=false;
	else if( elem == "start_action" )
		inStartAction=false;
	else if( elem == "options_action" )
		inOptionsAction=false;
	else if( elem == "credits_action" )
		inCreditsAction=false;
	else if( elem == "exit_action" )
		inExitAction=false;
	else if( elem == "sky" )
		inSky=false;
	else if( elem == "title" )
		inTitle=false;
}

void MenuState::createPlanet(const std::string &elem, MKeyValue &atts) {
	if( elem == "localization" ) {
		std::string position=atts["transform"];
		D3DXMATRIX matrix;
		D3DXMatrixIdentity(&matrix);
		sscanf_s(position.c_str(),"%f %f %f %f %f %f %f %f %f %f %f %f",
			&matrix(0,0), &matrix(0,1), &matrix(0,2),
			&matrix(1,0), &matrix(1,1), &matrix(1,2),
			&matrix(2,0), &matrix(2,1), &matrix(2,2),
			&matrix(3,0), &matrix(3,1), &matrix(3,2));
		menuPlanet.setTransformMatrix(matrix);

		DataLocatorManager *dataLocatorManager = DataLocatorManager::getDataLocatorManager();
		const std::string &pathToRoa = dataLocatorManager->getPath("roa");

		// Animation Resources
		nextAnimationResource = new RigidAnimationResource();
		std::string filename = pathToRoa + "menu_next.xml";
		bool ok = nextAnimationResource->initFromFile(filename.c_str());
		assert(ok);
		previousAnimationResource = new RigidAnimationResource();
		filename = pathToRoa + "menu_previous.xml";
		ok = previousAnimationResource->initFromFile(filename.c_str());
		assert(ok);
	}
	else if( elem == "mesh" ) {
		std::string name=atts["name"];

		IMesh *mesh = MeshFactory::getMeshFactory()->createMesh(name);
		assert(mesh);
		menuPlanet.setMesh(mesh);
	}
	else if( elem == "material" ) {
		std::string materialName=atts["name"];
		std::string shaderName=atts["shader"];
		std::string techniqueName=atts["technique"];
		menuPlanet.SETMATERIAL(materialName, shaderName, techniqueName); // VERY VERY OPTIONAL!!
	}
}

void MenuState::createAction(Object &action, const std::string &elem, MKeyValue &atts) {
	if( elem == "localization" ) {
		std::string position=atts["transform"];
		D3DXMATRIX matrix;
		D3DXMatrixIdentity(&matrix);
		sscanf_s(position.c_str(),"%f %f %f %f %f %f %f %f %f %f %f %f",
			&matrix(0,0), &matrix(0,1), &matrix(0,2),
			&matrix(1,0), &matrix(1,1), &matrix(1,2),
			&matrix(2,0), &matrix(2,1), &matrix(2,2),
			&matrix(3,0), &matrix(3,1), &matrix(3,2));
		action.setParentObject(&menuPlanet);
		action.setLocalTransformMatrix(matrix);
		action.updateTransformMatrix();
	}
	else if( elem == "mesh" ) {
		std::string name=atts["name"];

		IMesh *mesh = MeshFactory::getMeshFactory()->createMesh(name);
		assert(mesh);
		action.setMesh(mesh);
	}
	else if( elem == "material" ) {
		std::string materialName=atts["name"];
		std::string shaderName=atts["shader"];
		std::string techniqueName=atts["technique"];
		action.SETMATERIAL(materialName, shaderName, techniqueName); // VERY VERY OPTIONAL!!
	}
}

void MenuState::createSky(const std::string &elem, MKeyValue &atts) {
	if( elem == "localization" ) {
		std::string position=atts["transform"];
		D3DXMATRIX matrix;
		D3DXMatrixIdentity(&matrix);
		sscanf_s(position.c_str(),"%f %f %f %f %f %f %f %f %f %f %f %f",
			&matrix(0,0), &matrix(0,1), &matrix(0,2),
			&matrix(1,0), &matrix(1,1), &matrix(1,2),
			&matrix(2,0), &matrix(2,1), &matrix(2,2),
			&matrix(3,0), &matrix(3,1), &matrix(3,2));
		sky.setTransformMatrix(matrix);
	}
	else if( elem == "mesh" ) {
		std::string name=atts["name"];

		IMesh *mesh = MeshFactory::getMeshFactory()->createMesh(name);
		assert(mesh);
		sky.setMesh(mesh);
	}
	else if( elem == "material" ) {
		std::string materialName=atts["name"];
		std::string shaderName=atts["shader"];
		std::string techniqueName=atts["technique"];
		sky.SETMATERIAL(materialName, shaderName, techniqueName); // VERY VERY OPTIONAL!!
	}
}

void MenuState::createTitle(const std::string &elem, MKeyValue &atts) {
	if( elem == "localization" ) {
		std::string position=atts["transform"];
		D3DXMATRIX matrix;
		D3DXMatrixIdentity(&matrix);
		sscanf_s(position.c_str(),"%f %f %f %f %f %f %f %f %f %f %f %f",
			&matrix(0,0), &matrix(0,1), &matrix(0,2),
			&matrix(1,0), &matrix(1,1), &matrix(1,2),
			&matrix(2,0), &matrix(2,1), &matrix(2,2),
			&matrix(3,0), &matrix(3,1), &matrix(3,2));
		title.setTransformMatrix(matrix);

		// Now the mesh
		IMesh * mesh = NULL;
		if( Game::configOptions.aspectRatio == 4.0f/3.0f || Game::configOptions.aspectRatio == 5.0f/4.0f )
			mesh = QuadFactory::getQuadFactory()->createQuad(0.5f, 0.5f * Game::configOptions.aspectRatio);
		else
			mesh = QuadFactory::getQuadFactory()->createQuad(0.4f, 0.4f * Game::configOptions.aspectRatio);
		assert(mesh);
		title.setMesh(mesh);
	}
	else if( elem == "material" ) {
		std::string materialName=atts["name"];
		std::string shaderName=atts["shader"];
		std::string techniqueName=atts["technique"];
		title.SETMATERIAL(materialName, shaderName, techniqueName); // VERY VERY OPTIONAL!!
	}
}

void MenuState::update( float deltaTime ) {
	// FIX DELTA TIME FOR THE WAITING OF DESTROY EVERYTHING
	if( deltaTime > 1.0f/20.0f )
		deltaTime = 1.0f/60.0f;

	if( Global::fade.isFinished( ) ) {

		if( (Global::ioStatus.start.becomesPressed() 
			|| Global::ioStatus.selectOption.becomesPressed())
			&& menuPlanet.getAnimationController()==NULL ) {
			if( Global::samplesEnabled ) {
				SoundManager *soundManager = SoundManager::getSoundManager();
				soundManager->playSample( "option_selected" );
			}

			// If Play go to init iuranus
			if( menuStateAction==START ) {
				getState("init_iuranus_state")->getFocus();
				return;
			}
			// If Options go to options
			if( menuStateAction==OPTIONS ) {
				getState("options_state")->getFocus();
				return;
			}
			// If Credits go to credits
			if( menuStateAction==CREDITS ) {
				getState("credits_state")->getFocus();
				return;
			}
			// If Exit exit game
			else if( menuStateAction==EXIT ) {
				Global::exitGame = true;
				return;
			}
			else {
				assert( !"Action not exists in MenuState" );
				return;
			}
		}

		// Update menu planet
		updateMenuPlanet(deltaTime);
	}
	else
		Global::fade.update( deltaTime );

	// Update cartels
	startAction.updateTransformMatrix();
	optionsAction.updateTransformMatrix();
	creditsAction.updateTransformMatrix();
	exitAction.updateTransformMatrix();
}

void MenuState::updateMenuPlanet(float deltaTime) {
	IAnimationController *menuPlanetAnimationController = menuPlanet.getAnimationController();

	if( menuPlanetAnimationController != NULL ) {

		// If animation is finished, set menu planet animation to NULL
		if( menuPlanetAnimationController->isFinished() ) {
			if( Global::samplesEnabled ) {
				SoundManager *soundManager = SoundManager::getSoundManager();
				soundManager->playSample( "option_fixed" );
			}

			menuPlanet.setAnimationController(NULL);
		}
		// If animation is not finished, update it
		else
			menuPlanet.updateAnimation(deltaTime);
	}
	else {
		// Check player pushed UP
		if( Global::ioStatus.walk.becomesPressedDown() || Global::ioStatus.walk.becomesPressedLeft() ) {
			if( Global::samplesEnabled ) {
				SoundManager *soundManager = SoundManager::getSoundManager();
				soundManager->playSample( "option_movement" );
			}

			D3DXQUATERNION rotationQuaternion;
			D3DXQuaternionRotationMatrix(&rotationQuaternion, &menuPlanet.getTransformMatrix());

			KeyFramedROA *nextAnimation = new KeyFramedROA( nextAnimationResource, menuPlanet.getPosition(), rotationQuaternion);
			//nextAnimation->setTimeScale(0.5f);

			menuPlanet.setAnimationController(nextAnimation);
			menuPlanet.updateAnimation(deltaTime);

			menuStateAction = (MenuStateAction)((menuStateAction+NUM_MENU_STATE_ACTIONS-1)%NUM_MENU_STATE_ACTIONS);
		}
		// Check player pushed DOWN
		else if( Global::ioStatus.walk.becomesPressedUp() || Global::ioStatus.walk.becomesPressedRight() ) {
			if( Global::samplesEnabled ) {
				SoundManager *soundManager = SoundManager::getSoundManager();
				soundManager->playSample( "option_movement" );
			}

			D3DXQUATERNION rotationQuaternion;
			D3DXQuaternionRotationMatrix(&rotationQuaternion, &menuPlanet.getTransformMatrix());
			
			KeyFramedROA *previousAnimation = new KeyFramedROA( previousAnimationResource, menuPlanet.getPosition(), rotationQuaternion);
			//previousAnimation->setTimeScale(0.5f);

			menuPlanet.setAnimationController(previousAnimation);
			menuPlanet.updateAnimation(deltaTime);

			menuStateAction = (MenuStateAction)((menuStateAction+1)%NUM_MENU_STATE_ACTIONS);
		}
	}
}

void MenuState::preRender( ) {
}

void MenuState::renderWorld3D( ) {
	Global::renderStateEnableZTest( );
	Global::renderStateEnableZWriting( );

	menuPlanet.render();
	startAction.render();
	optionsAction.render();
	creditsAction.render();
	exitAction.render();

	sky.render();

	Global::renderStateDisableZWriting( );
	Global::renderStateDisableZTest( );
}

void MenuState::renderWorld2D( ) {
}

void MenuState::renderPostProcessed2D( ) {
	Global::renderStateEnableAlphaBlending( );

	title.render();

	Global::renderStateDisableAlphaBlending( );
}

void MenuState::renderGameUI( ) {
}

void MenuState::postRender( ) {
	// Print FPS
	Global::print(0, 0, 0xffff0000, "Frames per second %0.2f", Game::fps);
	Global::print(0, 10, 0xffff0000, "Menu Status: %d", menuStateAction);
	const D3DXVECTOR3 &cameraPosition = Global::camera->getPosition();
	Global::print(0, 20, 0xffff0000, "Camera Position: (%1.2f,%1.2f,%1.2f)", cameraPosition.x, cameraPosition.y, cameraPosition.z);
	Global::print(0, 30, 0xffff0000, "'G' Render Texture %s", Global::enabledRenderTexture?"YES":"NO");

	Global::renderStateEnableAlphaBlending( );
	Global::fade.renderFadeIn( 0.5f );
	Global::renderStateDisableAlphaBlending( );
}

void MenuState::renderFrame( ) {
	Global::renderTextureQuad.render( );
}

void MenuState::onLoad( ) {

	SoundManager *soundManager = SoundManager::getSoundManager();
	soundManager->playStream("menu_background");
	Global::samplesEnabled = true;

	if( doOnLoad ) {
		//LogicManager::getLogicManager()->createUserEvent("state/on_enter_menu_state");


		// Play sound
		//SoundManager::getSoundManager();

		// Set the camera in init menu position
		D3DXVECTOR3 cameraInitialPosition(419.05f, 20.64f, 394.65f);
		D3DXVECTOR3 cameraLookAt( menuPlanet.getPosition()+D3DXVECTOR3(0.0f,5.0f,4.0f) );
		Global::camera->setUpVector( D3DXVECTOR3(0,1,0) );
		Global::camera->lookAt( cameraInitialPosition, cameraLookAt );

		// Player
		Player *player = Player::getPlayer();
		player->setPlayerToInitPosition();
		// TODO Preguntar a xavi si esto quita todas las animaciones y carga el idle
		player->setRelationToState( CYCLE, "idle", true, true );

		// ESTO AL FINAL DE CADA PLANETA
		// the Polar Angles Partitioner
		//PolarAnglesPartitioner::getPolarAnglesPartitioner()->clean();
		// Logic
		//LogicManager::getLogicManager()->clean();
		// Temporal Particles
		//ParticlesManager::getParticlesManager()->clean();
		// Sound
		//SoundManager::getSoundManager();


		// Iuranus Logic
		//IuranusPlanetLogic::getIuranusPlanetLogic();

		// Iuranus Collisions
		IuranusCollisionManager::getIuranusCollisionManager()->clean();

		// Iuranus enemies
		IuranusEnemyManager::getIuranusEnemyManager()->clean();


		// Boiler Logic
		//BoilerPlanetLogic::getBoilerPlanetLogic();

		// Boiler Collisions
		BoilerCollisionManager::getBoilerCollisionManager()->clean();
	}
	else
		doOnLoad = true;
}

/*
void MenuState::resetCommonThings( ) {
	// Play sound
	//SoundManager::getSoundManager();

	// Set the camera in init menu position
	D3DXVECTOR3 cameraInitialPosition(419.05f, 20.64f, 394.65f);
	D3DXVECTOR3 cameraLookAt( menuPlanet.getPosition()+D3DXVECTOR3(0.0f,5.0f,4.0f) );
	Global::camera->setUpVector( D3DXVECTOR3(0,1,0) );
	Global::camera->lookAt( cameraInitialPosition, cameraLookAt );

	// Player
	Player *player = Player::getPlayer();
	player->setPlayerToInitPosition();
	// TODO Preguntar a xavi si esto quita todas las animaciones y carga el idle
	player->setRelationToState( CYCLE, "idle", true, true );

	// ESTO AL FINAL DE CADA PLANETA
	// the Polar Angles Partitioner
	//PolarAnglesPartitioner::getPolarAnglesPartitioner()->clean();
	// Logic
	//LogicManager::getLogicManager()->clean();
	// Temporal Particles
	//ParticlesManager::getParticlesManager()->clean();
	// Sound
	//SoundManager::getSoundManager();
}

void MenuState::resetIuranusThings( ) {
	// Iuranus Logic
	//IuranusPlanetLogic::getIuranusPlanetLogic();

	// Iuranus Collisions
	IuranusCollisionManager::getIuranusCollisionManager()->clean();

	// Iuranus enemies
	IuranusEnemyManager::getIuranusEnemyManager()->clean();
}

void MenuState::resetBoilerThings( ) {
	// Boiler Logic
	//BoilerPlanetLogic::getBoilerPlanetLogic();

	// Boiler Collisions
	BoilerCollisionManager::getBoilerCollisionManager()->clean();
}
*/

void MenuState::onUnload( ) {
}

void MenuState::loadObjects() {
	DataLocatorManager *dataLocatorManager = DataLocatorManager::getDataLocatorManager();
	const std::string &pathToXml = dataLocatorManager->getPath("xml");

	bool ok = xmlParseFile(pathToXml + "menu.xml");
	assert(ok);
}

void MenuState::reloadShader() {
	menuPlanet.reloadShader();
	sky.reloadShader();
	title.reloadShader();

	startAction.reloadShader();
	optionsAction.reloadShader();
	creditsAction.reloadShader();
	exitAction.reloadShader();
}
