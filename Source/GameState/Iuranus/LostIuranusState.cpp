#include "LostIuranusState.h"
#include "IuranusState.h"
#include "Global/GlobalFunctions.h"
#include "Global/GlobalVariables.h"
#include "Game/Game.h"
#include "Managers/PlanetManager.h"
#include "Managers/IuranusManagers/IuranusEnemyManager.h"
#include "Managers/IuranusManagers/IuranusPlanetElementsManager.h"
#include "Managers/StaticObjectManager.h"
#include "Managers/Static2DObjectManager.h"
#include "Managers/IuranusManagers/IuranusAnimatedObjectManager.h"
#include "Managers/ParticlesManager.h"
#include "Managers/IuranusManagers/IuranusFixedParticlesManager.h"
#include "Managers/PlanetAtmosphereManager.h"
#include "Managers/CollisionManager/IuranusCollisionManager.h"
#include "Managers/LogicManager/LogicManager.h"
#include "Managers/IuranusManagers/IuranusShadowManager.h"
#include "Object/CollisionableObject/AnimatedObject/Player/Player.h"
#include "CameraControllers/SSPlayer/SSPlayerCameraController.h"
#include "Factories/QuadFactory.h"
#include "Logic/PlanetLogic/IuranusPlanetLogic/IuranusPlanetLogic.h"

std::string LostIuranusState::name = "lost_iuranus_state";

const D3DXVECTOR3 LostIuranusState::onRetryPosition( -0.1f, 0.0f, 0.0f );
D3DXVECTOR3 LostIuranusState::onBackToMenuPosition( -0.1f, -0.2f, 0.0f );

const float LostIuranusState::cameraDistanceNomal = 16.0f;
const float LostIuranusState::cameraPositionSpeedNomal = 1.8f;
const float LostIuranusState::cameraTargetSpeedNomal = 12.0f;

const float LostIuranusState::cameraDistanceLose = 26.0f;
const float LostIuranusState::cameraPositionSpeedLose = 0.4f;
const float LostIuranusState::cameraTargetSpeedLose = 12.0f;

void LostIuranusState::update( float deltaTime ) {

	// Update camera
	Global::camera->update(deltaTime);
	// Retires the camera
	SSPlayerCameraController * cameraController = (SSPlayerCameraController *) Global::camera->getCameraController();
	cameraController->setDistance( cameraDistanceLose );
	cameraController->setPositionSpeed( cameraPositionSpeedLose );
	cameraController->setTargetSpeed( cameraTargetSpeedLose );

	// Update frustum
	Global::frustum.update();
	Global::backSideFrustum.update();

	if(Global::IAEnabled) {
		// Update AI
		IuranusEnemyManager::getIuranusEnemyManager()->updateAI(deltaTime);
		IuranusPlanetElementsManager::getIuranusPlanetElementsManager()->updateAI(deltaTime);
	}

	if(Global::animationEnabled) {
		// Update Animation of enemies
		IuranusEnemyManager::getIuranusEnemyManager()->updateAnimation(deltaTime);
		IuranusPlanetElementsManager::getIuranusPlanetElementsManager()->updateAnimation(deltaTime);

		IuranusAnimatedObjectManager::getIuranusAnimatedObjectManager()->updateAnimation(deltaTime);
	}

	// Update Planet Atmospheres positions
	PlanetAtmosphereManager::getPlanetAtmosphereManager()->update();

	// Update IURANUS Particles
	ParticlesManager::getParticlesManager()->update(deltaTime);
	IuranusFixedParticlesManager::getIuranusFixedParticlesManager()->update(deltaTime);

	// Update Space
	PolarAnglesPartitioner *polarAnglesPartitioner = PolarAnglesPartitioner::getPolarAnglesPartitioner();
	timeoutUpdateSpace.update(deltaTime);
	if(timeoutUpdateSpace.isFinished()) {
		timeoutUpdateSpace.restart();

		// Update space
		polarAnglesPartitioner->updateSpace();
	}

	if(Global::collisionEnabled) {
		// Detect Collisions
		IuranusCollisionManager::getIuranusCollisionManager()->detectCollisions();

		// Treat Collisions
		IuranusCollisionManager::getIuranusCollisionManager()->treatCollisions();
	}

	/*
	if(Global::logicEnabled) {
		// Update Logic
		LogicManager::getLogicManager()->update(deltaTime);
	}
	*/

	// Update Lost UI
	if ( enter_lost ) {
		if( !Global::fade.isFinished( ) )
			Global::fade.update( deltaTime );
		else
			enter_lost = false;
	}
	else if( exit_lost ) {
		
		IuranusPlanetLogic *iuranusPlanetLogic = IuranusPlanetLogic::getIuranusPlanetLogic();
		int numberOfContinuesLeft = iuranusPlanetLogic->getNumberOfContinuesLeft();

		if( !Global::fade.isFinished( ) ) {
			Global::fade.update( deltaTime );
		}
		else if ( numberOfContinuesLeft == 0 ) {
			getState( "end_iuranus_to_menu_state" )->getFocus( );
		}
		else if ( onRetry ) {
			// Try again
			LogicManager::getLogicManager()->createUserEvent("iuranus_restart_planet");

			// Decrement continues
			IuranusPlanetLogic *iuranusPlanetLogic = IuranusPlanetLogic::getIuranusPlanetLogic();
			iuranusPlanetLogic->decrementContinues();

			// Restore camera distance
			SSPlayerCameraController * cameraController = (SSPlayerCameraController *) Global::camera->getCameraController();
			cameraController->setDistance( cameraDistanceNomal );
			cameraController->setPositionSpeed( cameraPositionSpeedNomal );
			cameraController->setTargetSpeed( cameraTargetSpeedNomal );

			// Leave player animation to idle
			Player::getPlayer( )->setRelationToState( CYCLE, "idle", true, true );

			// Reset particles (destroy them)
			ParticlesManager::getParticlesManager( )->cleanUp( );

			// Go to iuranus state, but telling it that not make its onLoad
			IuranusState * iuranusState = (IuranusState *) getState( "iuranus_state" );
			iuranusState->doOnLoad = false;
			iuranusState->getFocus( );
		}
		else {
			getState( "end_iuranus_to_menu_state" )->getFocus( );
		}
	}
	else {
		if( Global::ioStatus.start.becomesPressed( ) 
			|| Global::ioStatus.selectOption.becomesPressed() ) {
			exit_lost = true;
		}
		else if (  Global::ioStatus.walk.becomesPressedUp() ) {
			onRetry = true;
			arrow.setPosition( onRetryPosition );
		}
		else if (  Global::ioStatus.walk.becomesPressedDown() ) {
			onRetry = false;
			arrow.setPosition( onBackToMenuPosition );
		}
	}

}

void LostIuranusState::preRender( ) {
}

void LostIuranusState::renderWorld3D( ) {
	Global::renderStateEnableZTest( );
	Global::renderStateEnableZWriting( );

	if( Global::youLoseTimeOut && !Global::youLose )
		Player::getPlayer()->render( );

	// Draw Scene
	PlanetManager::getPlanetManager()->render();
	IuranusEnemyManager::getIuranusEnemyManager()->render();
	IuranusPlanetElementsManager::getIuranusPlanetElementsManager()->render();
	
	Global::renderStateEnableAlphaTest( );
	Global::renderStateEnableAlphaBlending( );
	StaticObjectManager::getStaticObjectManager()->render();
	Global::renderStateDisableAlphaBlending( );
	Global::renderStateDisableAlphaTest( );

	Global::renderStateDisableZWriting( );
	Global::renderStateDisableZTest( );
}

void LostIuranusState::renderWorld2D( ) {
	Global::renderStateEnableAlphaBlending( );
	Global::renderStateEnableZTest( );

	Global::renderStateEnableAlphaTest( );
	// Draw Bushes
	IuranusAnimatedObjectManager::getIuranusAnimatedObjectManager()->render();

	// Draw Shadows
	IuranusShadowManager::getIuranusShadowManager()->render();
	Global::renderStateDisableAlphaTest( );

	// Render IURANUS Particles
	ParticlesManager::getParticlesManager()->render();
	IuranusFixedParticlesManager::getIuranusFixedParticlesManager()->render();

	Global::renderStateEnableAdditiveAlphaBlending();
	Static2DObjectManager::getStatic2DObjectManager()->render();
	PlanetAtmosphereManager::getPlanetAtmosphereManager()->render();

	Global::renderStateDisableZTest( );
	Global::renderStateDisableAlphaBlending( );
}

void LostIuranusState::renderPostProcessed2D( ) {
}

void LostIuranusState::renderGameUI( ) {
	Global::renderStateEnableAlphaBlending();

	IuranusState * iuranusState = (IuranusState *) getState( "iuranus_state" );
	iuranusState->iuranusUI.render( );

	// Render fade medium out
	if( enter_lost )
		Global::fade.renderMediumFadeOut(0.5f, D3DXVECTOR3(0, 0.2f, 0));
	// Render fade in
	else if( exit_lost )
		Global::fade.renderFadeIn(0.5f, D3DXVECTOR3(0, 0.2f, 0));
	// Render lost UI
	else {
		Global::fade.renderMediumFadeOut(0.5f, D3DXVECTOR3(0, 0.2f, 0));

		flowerUp.render( );

		//Global::loseQuadUI.render();

		IuranusPlanetLogic *iuranusPlanetLogic = IuranusPlanetLogic::getIuranusPlanetLogic();
		int numberOfContinuesLeft = iuranusPlanetLogic->getNumberOfContinuesLeft();

		alphabetRender.print(0.3f, 0.7f, 0.1f, 0.1f, "GAME OVER");
		if( numberOfContinuesLeft > 0 ) {
			alphabetRender.print(0.5f, 0.5f, 0.05f, 0.05f, "Continue -%d-\n\n\nReturn To Menu", numberOfContinuesLeft);
			arrow.render( );
		}
		else {
			alphabetRender.print(0.38f, 0.5f, 0.05f, 0.05f, "Press Start");
		}
	}

	Global::renderStateDisableAlphaBlending();
}

void LostIuranusState::postRender( ) {
	// Print FPS
	Global::print(0,0, 0xffff0000, "Frames per second %0.2f", Game::fps);
	Global::print(0,10, 0xffff0000, "Lost Iuranus Status");
}

void LostIuranusState::renderFrame( ) {
	Global::renderTextureQuad.render( );
}

void LostIuranusState::onLoad( ) {
	enter_lost = true;
	exit_lost = false;
	onRetry = true;
	arrow.setPosition( onRetryPosition );

	// Reset variables for spatial partitioner
	timeoutUpdateSpace.restart();

	// If we enter here by lose, delete planet timeout timer
	if( Global::youLose ) {
		LogicManager * logicManager = LogicManager::getLogicManager();
		logicManager->deleteTimer("iuranus_planet_timeout");
	}
}

void LostIuranusState::onUnload( ) {
	// AQUI NADA
}

void LostIuranusState::loadObjects( ) {
	// Arrow object
	D3DXMATRIX arrowtransformMatrix;
	D3DXMatrixIdentity( &arrowtransformMatrix );
	arrow.setTransformMatrix( arrowtransformMatrix );

	arrow.SETMATERIAL("selection_icon.dds", "shader.fx", "dummy_tech");

	IMesh * arrowMesh = QuadFactory::getQuadFactory()->createQuad(0.1f, 0.1f * Game::configOptions.aspectRatio );
	assert(arrowMesh);
	arrow.setMesh(arrowMesh);

	// Flower up
	D3DXMATRIX flowerUptransformMatrix;
	D3DXMatrixIdentity( &flowerUptransformMatrix );
	flowerUp.setTransformMatrix( flowerUptransformMatrix );
	flowerUp.setPosition( D3DXVECTOR3(-0.4f, 0.0f, 0.0f) );

	flowerUp.SETMATERIAL("flower_gameover.dds", "shader.fx", "dummy_tech");

	IMesh * flowerUpMesh = QuadFactory::getQuadFactory()->createQuad(0.45f, 0.45f * Game::configOptions.aspectRatio );
	assert(flowerUpMesh);
	flowerUp.setMesh(flowerUpMesh);

	// -------------------------------

	// Create the alphabet render
	alphabetRender.create( );

	if( Game::configOptions.aspectRatio != 4.0f/3.0f && Game::configOptions.aspectRatio != 5.0f/4.0f )
		onBackToMenuPosition = D3DXVECTOR3( -0.1f, -0.27f, 0.0f );
}

void LostIuranusState::reloadShader( ) {
	arrow.reloadShader( );
	alphabetRender.reloadShader( );
	flowerUp.reloadShader( );
}
