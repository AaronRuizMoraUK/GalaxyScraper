#include "LostBoilerState.h"
#include "BoilerState.h"
#include "Global/GlobalFunctions.h"
#include "Global/GlobalVariables.h"
#include "Source/Game.h"
#include "Managers/PlanetManager.h"
#include "Managers/PlanetAtmosphereManager.h"
#include "Managers/ParticlesManager.h"
#include "Managers/StaticObjectManager.h"
#include "Managers/Static2DObjectManager.h"
#include "Managers/LogicManager/LogicManager.h"
#include "Object/CollisionableObject/AnimatedObject/Player/Player.h"
#include "CameraControllers/SSPlayer/SSPlayerCameraController.h"
#include "Factories/QuadFactory.h"
#include "Logic/PlanetLogic/BoilerPlanetLogic/BoilerPlanetLogic.h"
#include "Managers/BoilerManagers/BoilerFixedParticlesManager.h"
#include "Managers/BoilerManagers/BoilerAnimatedObjectManager.h"
#include "Managers/BoilerManagers/LeversManager.h"
#include "Managers/BoilerManagers/CanonsManager.h"
#include "Managers/BoilerManagers/BoilerEnemyManager.h"
#include "Managers/BoilerManagers/BoilerFireballManager.h"
#include "Managers/IuranusManagers/IuranusShadowManager.h"

std::string LostBoilerState::name = "lost_boiler_state";

const D3DXVECTOR3 LostBoilerState::onRetryPosition( -0.1f, 0.0f, 0.0f );
D3DXVECTOR3 LostBoilerState::onBackToMenuPosition( -0.1f, -0.2f, 0.0f );

const float LostBoilerState::cameraDistanceNomal = 25.0f;
const float LostBoilerState::cameraPositionSpeedNomal = 0.94f;
const float LostBoilerState::cameraTargetSpeedNomal = 2.15f;

const float LostBoilerState::cameraDistanceLose = 70.0f;
const float LostBoilerState::cameraPositionSpeedLose = 0.2f;
const float LostBoilerState::cameraTargetSpeedLose = 2.15f;

void LostBoilerState::update( float deltaTime ) {

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

	// Update Planet Atmospheres positions
	PlanetAtmosphereManager::getPlanetAtmosphereManager()->update();

	// Update BOILER Particles
	ParticlesManager::getParticlesManager()->update(deltaTime);
	BoilerFixedParticlesManager::getBoilerFixedParticlesManager()->update(deltaTime);

	BoilerEnemyManager::getBoilerEnemyManager()->updateAnimation(deltaTime);

	// Update Lost UI
	if ( enter_lost ) {
		if( !Global::fade.isFinished( ) )
			Global::fade.update( deltaTime );
		else
			enter_lost = false;
	}
	else if( exit_lost ) {
		BoilerPlanetLogic *boilerPlanetLogic = BoilerPlanetLogic::getBoilerPlanetLogic();
		int numberOfContinuesLeft = boilerPlanetLogic->getNumberOfContinuesLeft();
		Global::cameraVibrationEnabled = false;

		if( !Global::fade.isFinished( ) ) {
			Global::fade.update( deltaTime );
		}
		else if ( numberOfContinuesLeft == 0 ) {
			// AQUI SI NOS QUEDAMOS SIN CONTINUES
			getState( "end_boiler_to_menu_state" )->getFocus( );
		}
		else if ( onRetry ) 
		{
			// TODO Restart Boiler Planet (include player in initial point)
			const Planet *planet = PlanetManager::getPlanetManager()->getCurrentPlanet();
			const D3DXVECTOR3 &endTarget = planet->getPosition();
			D3DXVECTOR3 endPosition = endTarget + D3DXVECTOR3(0, 0, 65);
			Global::camera->setUpVector( D3DXVECTOR3(0,1,0) );
			Global::camera->lookAt( endPosition, endTarget );

			Player::getPlayer( )->setPlayerToInitPosition( );
			Player::getPlayer( )->setPosition( planet->getPosition( ) + D3DXVECTOR3(0.0f,0.0f,40.0f) );

			BoilerPlanetLogic *boilerPlanetLogic = BoilerPlanetLogic::getBoilerPlanetLogic();
			boilerPlanetLogic->decrementContinues();
			boilerPlanetLogic->alreadyEnteredBossZone = false;

			// Restore camera distance
			SSPlayerCameraController * cameraController = (SSPlayerCameraController *) Global::camera->getCameraController();
			cameraController->setDistance( cameraDistanceNomal );
			cameraController->setPositionSpeed( cameraPositionSpeedNomal );
			cameraController->setTargetSpeed( cameraTargetSpeedNomal );

			// Leave player animation to idle
			Player::getPlayer( )->setRelationToState( CYCLE, "idle", true, true );

			// Reset particles (destroy them)
			ParticlesManager::getParticlesManager( )->cleanUp( );
			BoilerFireballManager::getBoilerFireballManager()->renewParticles();

			// Reset levers and cannons states
			LeversManager::getLeversManager()->restart();
			CanonsManager::getCanonsManager()->restart();

			// Reset door state
			BoilerAnimatedObjectManager::getBoilerAnimatedObjectManager()->restart();

			BoilerEnemyManager::getBoilerEnemyManager()->restart();

			LogicManager::getLogicManager()->createUserEvent("boiler_restart_planet");

			
			// Go to boiler state, but telling it that not make its onLoad
			BoilerState * boilerState = (BoilerState *) getState( "boiler_state" );
			boilerState->doOnLoad = false;
			boilerState->getFocus( );
		}
		else {
			getState( "end_boiler_to_menu_state" )->getFocus( );
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

void LostBoilerState::preRender( ) {
}

void LostBoilerState::renderWorld3D( ) {
	Global::renderStateEnableZTest( );
	Global::renderStateEnableZWriting( );

	// Game Renders
	PlanetManager::getPlanetManager()->render();
	BoilerAnimatedObjectManager::getBoilerAnimatedObjectManager()->render( );
	BoilerEnemyManager::getBoilerEnemyManager()->render( );
	CanonsManager::getCanonsManager()->render();
	LeversManager::getLeversManager()->render();

	Global::renderStateEnableAlphaTest( );
	Global::renderStateEnableAlphaBlending( );
	StaticObjectManager::getStaticObjectManager()->render();
	Global::renderStateDisableAlphaBlending( );
	Global::renderStateDisableAlphaTest( );

	Global::renderStateDisableZWriting( );
	Global::renderStateDisableZTest( );
}

void LostBoilerState::renderWorld2D( ) {
	Global::renderStateEnableAlphaBlending( );
	Global::renderStateEnableZTest( );

	// Draw Shadows
	Global::renderStateEnableAlphaTest( );
	IuranusShadowManager::getIuranusShadowManager()->render();
	Global::renderStateDisableAlphaTest( );

	ParticlesManager::getParticlesManager()->render();
	BoilerFixedParticlesManager::getBoilerFixedParticlesManager()->render();

	if( Global::enabledRenderTexture ) {
		// Render To Texture to do the air lava effect
		//Global::renderStateEnableAlphaBlending( );
		BoilerState * boilerState = (BoilerState *) getState( "boiler_state" );
		boilerState->lavaDistorsion.render( );
	}

	Global::renderStateEnableAdditiveAlphaBlending();
	Static2DObjectManager::getStatic2DObjectManager()->render();
	PlanetAtmosphereManager::getPlanetAtmosphereManager()->render();

	Global::renderStateDisableZTest( );
	Global::renderStateDisableAlphaBlending( );
}

void LostBoilerState::renderPostProcessed2D( ) {
}

void LostBoilerState::renderGameUI( ) {
	Global::renderStateEnableAlphaBlending();

	BoilerState * boilerState = (BoilerState *) getState( "boiler_state" );
	boilerState->boilerUI.render( );

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

		BoilerPlanetLogic *boilerPlanetLogic = BoilerPlanetLogic::getBoilerPlanetLogic();
		int numberOfContinuesLeft = boilerPlanetLogic->getNumberOfContinuesLeft();

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

void LostBoilerState::postRender( ) {
	// Print FPS
	Global::print(0,0, 0xffff0000, "Frames per second %0.2f", Game::fps);
	Global::print(0,10, 0xffff0000, "Lost Boiler Status");
}

void LostBoilerState::renderFrame( ) {
	Global::renderTextureQuad.render( );
}

void LostBoilerState::onLoad( ) {
	enter_lost = true;
	exit_lost = false;
	onRetry = true;
	arrow.setPosition( onRetryPosition );

	/*
	// If we enter here by lose, delete planet timeout timer
	if( Global::youLose ) {
		LogicManager * logicManager = LogicManager::getLogicManager();
		logicManager->deleteTimer("iuranus_planet_timeout");
	}
	*/
}

void LostBoilerState::onUnload( ) {
	// AQUI NADA
}

void LostBoilerState::loadObjects( ) {
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

void LostBoilerState::reloadShader( ) {
	arrow.reloadShader( );
	alphabetRender.reloadShader( );
	flowerUp.reloadShader( );
}
