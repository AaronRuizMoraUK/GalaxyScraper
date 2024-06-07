#include "IuranusState.h"
#include "Global/GlobalFunctions.h"
#include "Global/GlobalVariables.h"
#include "Source/Game.h"
#include "Managers/IuranusManagers/IuranusEnemyManager.h"
#include "Managers/IuranusManagers/IuranusPlanetElementsManager.h"
#include "Managers/IuranusManagers/IuranusAnimatedObjectManager.h"
#include "Managers/PlanetAtmosphereManager.h"
#include "Managers/ParticlesManager.h"
#include "Managers/IuranusManagers/IuranusFixedParticlesManager.h"
#include "Managers/LogicManager/LogicManager.h"
#include "Managers/PlanetManager.h"
#include "Managers/StaticObjectManager.h"
#include "Managers/Static2DObjectManager.h"
#include "Managers/SoundManager.h"
#include "Managers/CollisionManager/IuranusCollisionManager.h"
#include "Managers/IuranusManagers/IuranusShadowManager.h"
#include "Object/CollisionableObject/AnimatedObject/Player/Player.h"
#include "Util/SpatialPartitioner/PolarAnglesPartitioner.h"
#include "CameraControllers/SSPlayer/SSPlayerCameraController.h"
#include <cassert>

std::string IuranusState::name = "iuranus_state";

const float IuranusState::cameraDistanceNomal = 16.0f;
const float IuranusState::cameraPositionSpeedNomal = 1.8f;
const float IuranusState::cameraTargetSpeedNomal = 12.0f;

void IuranusState::update( float deltaTime ) {

	updateDeltaTime = deltaTime;

	updateTimeStart = updateCameraFrustumTimeStart = timeGetTime(); // START

	// Update camera
	Global::camera->update(deltaTime);

	// Update frustum
	Global::frustum.update();
	Global::backSideFrustum.update();

	updatePlayerTimeStart = updateCameraFrustumTimeEnd = timeGetTime(); // END

	Player *player = Player::getPlayer();
	if( !Global::takePlayerControlOut && !Global::takePlayerControlOutUpdatingAnimation )
	{
		// Update Player
		player->refreshPreviousTransformMatrix();
		player->update(deltaTime);
	}

	updateAITimeStart = updatePlayerTimeEnd = timeGetTime(); // END

	if(Global::IAEnabled) {
		// Update AI
		IuranusEnemyManager::getIuranusEnemyManager()->updateAI(deltaTime);
		IuranusPlanetElementsManager::getIuranusPlanetElementsManager()->updateAI(deltaTime);
	}

	updateAnimationTimeStart = updateAITimeEnd = timeGetTime(); // END

	if(Global::animationEnabled) {
		// Update Animation of player
		if( !Global::takePlayerControlOut )
			player->updateAnimation(deltaTime);

		// Update Animation of enemies
		IuranusEnemyManager::getIuranusEnemyManager()->updateAnimation(deltaTime);
		IuranusPlanetElementsManager::getIuranusPlanetElementsManager()->updateAnimation(deltaTime);

		IuranusAnimatedObjectManager::getIuranusAnimatedObjectManager()->updateAnimation(deltaTime);
	}

	updateAtmosphereParticlesTimeStart = updateAnimationTimeEnd = timeGetTime(); // END

	// Update Planet Atmospheres positions
	PlanetAtmosphereManager::getPlanetAtmosphereManager()->update();

	// Update IURANUS Particles
	ParticlesManager::getParticlesManager()->update(deltaTime);
	IuranusFixedParticlesManager::getIuranusFixedParticlesManager()->update(deltaTime);

	updateSpaceTimeStart = updateAtmosphereParticlesTimeEnd = timeGetTime(); // END

	// Update Space
	PolarAnglesPartitioner *polarAnglesPartitioner = PolarAnglesPartitioner::getPolarAnglesPartitioner();
	timeoutUpdateSpace.update(deltaTime);
	if(timeoutUpdateSpace.isFinished() || firstUpdateSpace) {
		firstUpdateSpace=false;
		timeoutUpdateSpace.restart();

		// Update space
		polarAnglesPartitioner->updateSpace();
	}

	updateCollisionTimeStart = updateSpaceTimeEnd = timeGetTime(); // END

	if(Global::collisionEnabled) {
		// Detect Collisions
		IuranusCollisionManager::getIuranusCollisionManager()->detectCollisions();

		// Treat Collisions
		IuranusCollisionManager::getIuranusCollisionManager()->treatCollisions();
	}

	updateLogicTimeStart = updateCollisionTimeEnd = timeGetTime(); // END

	if(Global::logicEnabled) {
		// Update Logic
		LogicManager::getLogicManager()->update(deltaTime);
	}

	updateTimeEnd = updateLogicTimeEnd = timeGetTime(); // END

	// Update UI
	iuranusUI.update( deltaTime );

	// --------------------------------------------------------------
	// CHECK CHANGE GAME STATE

	// If player wins go to end_iuranus state
	if ( Global::youWin ) {
		getState("end_iuranus_state")->getFocus();
		return;
	}
	// If player loses restart game in iuranus planet
	else if( Global::youLose || Global::youLoseTimeOut ) {
		getState("lost_iuranus_state")->getFocus();
		return;
	}
	// If start was pressed go to pause iuranus state
	else if( Global::ioStatus.start.becomesPressed() ) {
		getState("pause_iuranus_state")->getFocus();
		return;
	}
}

void IuranusState::preRender( ) {
	// Activate the camera in the device.
	// For normal execution it is not necesary, but for
	// calling any Global draw function it is.
	Global::camera->activate();

	// --- Debug draws --- //
	// Initiate the wold matrix
	//Global::world.initiate();
	//Global::world.activate();

	// Draw axis
	//Global::drawAxis();

	// Draw Grid XZ
	//Global::drawGridXZ();

	// Draw Back Side Frustum
	//Global::backSideFrustum.render();
	// ------------------ //

	// SET WIREFRAME FILLMODE
	//Global::renderStateSetWireframeFillMode();
}

void IuranusState::renderWorld3D( ) {
	Global::renderStateEnableZTest( );
	Global::renderStateEnableZWriting( );
	renderPlayerTimeStart = timeGetTime(); // START

	// Draw player
	Player::getPlayer()->render();

	renderPlanetsTimeStart = renderPlayerTimeEnd = timeGetTime(); // END

	// Draw Scene
	PlanetManager::getPlanetManager()->render();
	renderEnemiesTimeStart = renderPlanetsTimeEnd = timeGetTime(); // END
	IuranusEnemyManager::getIuranusEnemyManager()->render();
	renderPlanetElementsTimeStart = renderEnemiesTimeEnd = timeGetTime(); // END
	IuranusPlanetElementsManager::getIuranusPlanetElementsManager()->render();
	renderStaticObjectsTimeStart = renderPlanetElementsTimeEnd = timeGetTime(); // END

	Global::renderStateEnableAlphaTest( );
	Global::renderStateEnableAlphaBlending( );
	StaticObjectManager::getStaticObjectManager()->render();
	Global::renderStateDisableAlphaBlending( );
	Global::renderStateDisableAlphaTest( );

	renderStaticObjectsTimeEnd = timeGetTime(); // END

	Global::renderStateDisableZWriting( );
	Global::renderStateDisableZTest( );
}

void IuranusState::renderWorld2D( ) {
	Global::renderStateEnableAlphaBlending( );
	Global::renderStateEnableZTest( );

	renderAnimatedObjectsTimeStart = timeGetTime(); // START

	Global::renderStateEnableAlphaTest( );
	// Draw Bushes
	IuranusAnimatedObjectManager::getIuranusAnimatedObjectManager()->render();

	// Draw Shadows
	IuranusShadowManager::getIuranusShadowManager()->render();
	Global::renderStateDisableAlphaTest( );

	render2DTimeStart = renderAnimatedObjectsTimeEnd = timeGetTime(); // END

	// Render IURANUS Particles
	ParticlesManager::getParticlesManager()->render();
	IuranusFixedParticlesManager::getIuranusFixedParticlesManager()->render();

	Global::renderStateEnableAdditiveAlphaBlending();
	Static2DObjectManager::getStatic2DObjectManager()->render();
	PlanetAtmosphereManager::getPlanetAtmosphereManager()->render();

	Global::renderStateDisableZTest( );
	Global::renderStateDisableAlphaBlending( );

	render2DTimeEnd = timeGetTime(); // END
}

void IuranusState::renderPostProcessed2D( ) {
}

void IuranusState::renderGameUI( ) {
	renderUITimeStart = timeGetTime(); // START

	Global::renderStateEnableAlphaBlending();

	iuranusUI.render( );

	Global::renderStateDisableAlphaBlending();

	renderUITimeEnd = timeGetTime(); // END
}

void IuranusState::postRender( ) {
	// Print FPS
	Global::print(0, 100, 0xffff0000, "Frames per second %0.2f", Game::fps);
	Global::print(0, 110, 0xffff0000, "Update Time %f", ((float)(updateTimeEnd-updateTimeStart))*0.001f);
	Global::print(0, 120, 0xffff0000, "Render Time %f", Global::renderTime);
	Global::print(0, 130, 0xffff0000, "'G' Render Texture %s", Global::enabledRenderTexture?"YES":"NO");
	Global::print(0, 140, 0xffff0000, "'T' show/hide statistics");

	if( Global::showStatistic ) {
		Global::print(0, 160, 0xffffffff, "Cycle %s", Player::getPlayer()->getCurrentCycle().c_str());
		Global::print(0, 170, 0xffffffff, "Action %s", Player::getPlayer()->getCurrentAction().c_str());
		Global::print(0, 180, 0xffffffff,  "(z) Collisions   %s", Global::collisionEnabled?"enabled":"disabled");
		Global::print(0, 190, 0xffffffff, "(Z) IA              %s", Global::IAEnabled?"enabled":"disabled");
		Global::print(0, 200, 0xffffffff, "(x) Animations %s", Global::animationEnabled?"enabled":"disabled");
		Global::print(0, 210, 0xffffffff, "(X) Logic         %s", Global::logicEnabled?"enabled":"disabled");
						 
		Global::print(0, 230, 0xffffffff, "Update Time (Camera-Frustum) %f", ((float)(updateCameraFrustumTimeEnd-updateCameraFrustumTimeStart))*0.001f );
		Global::print(0, 240, 0xffffffff, "Update Time (Player) %f", ((float)(updatePlayerTimeEnd-updatePlayerTimeStart))*0.001f );
		Global::print(0, 250, 0xffffffff, "Update Time (AI) %f", ((float)(updateAITimeEnd-updateAITimeStart))*0.001f );
		Global::print(0, 260, 0xffffffff, "Update Time (Animation) %f", ((float)(updateAnimationTimeEnd-updateAnimationTimeStart))*0.001f );
		Global::print(0, 270, 0xffffffff, "Update Time (Atmosphere-Particles) %f", ((float)(updateAtmosphereParticlesTimeEnd-updateAtmosphereParticlesTimeStart))*0.001f );
		Global::print(0, 280, 0xffffffff, "Update Time (Space) %f", ((float)(updateSpaceTimeEnd-updateSpaceTimeStart))*0.001f );
		Global::print(0, 290, 0xffffffff, "Update Time (Collision) %f", ((float)(updateCollisionTimeEnd-updateCollisionTimeStart))*0.001f );
		Global::print(0, 300, 0xffffffff, "Update Time (Logic) %f", ((float)(updateLogicTimeEnd-updateLogicTimeStart))*0.001f );
						 
		Global::print(0, 320, 0xffffffff, "Render Time (Player) %f", ((float)(renderPlayerTimeEnd-renderPlayerTimeStart))*0.001f );
		Global::print(0, 330, 0xffffffff, "Render Time (Planets) %f", ((float)(renderPlanetsTimeEnd-renderPlanetsTimeStart))*0.001f );
		Global::print(0, 340, 0xffffffff, "Render Time (Enemies) %f", ((float)(renderEnemiesTimeEnd-renderEnemiesTimeStart))*0.001f );
		Global::print(0, 350, 0xffffffff, "Render Time (PlanetElements) %f", ((float)(renderPlanetElementsTimeEnd-renderPlanetElementsTimeStart))*0.001f );
		Global::print(0, 360, 0xffffffff, "Render Time (StaticObjects) %f", ((float)(renderStaticObjectsTimeEnd-renderStaticObjectsTimeStart))*0.001f );
		Global::print(0, 370, 0xffffffff, "Render Time (AnimatedObjects) %f", ((float)(renderAnimatedObjectsTimeEnd-renderAnimatedObjectsTimeStart))*0.001f );
		Global::print(0, 380, 0xffffffff, "Render Time (2D) %f", ((float)(render2DTimeEnd-render2DTimeStart))*0.001f );
		Global::print(0, 390, 0xffffffff, "Render Time (UI) %f", ((float)(renderUITimeEnd-renderUITimeStart))*0.001f );

		Global::print(0, 410, 0xffffffff, "Delta  Time %f - Frames By Delta Time %f", updateDeltaTime, 1.0f/updateDeltaTime );
	}
}

void IuranusState::renderFrame( ) {
	Global::renderTextureQuad.render( );
}

void IuranusState::onLoad( ) {

	if( doOnLoad ) {
		// Play sound
		//SoundManager::getSoundManager();

		// Set the camera for playing in Iuranus planet
		Global::camera->setUpVector( D3DXVECTOR3(0,1,0) );
		Global::camera->loadSSPlayerCameraController();
		SSPlayerCameraController * cameraController = (SSPlayerCameraController *) Global::camera->getCameraController();
		cameraController->setDistance( cameraDistanceNomal );
		cameraController->setPositionSpeed( cameraPositionSpeedNomal );
		cameraController->setTargetSpeed( cameraTargetSpeedNomal );

		// Restart the planet
		//	LogicManager::getLogicManager()->createUserEvent("iuranus_init");
		LogicManager::getLogicManager()->createUserEvent("iuranus_restart_planet");

		// Reset variables for spatial partitioner
		firstUpdateSpace=true;
		timeoutUpdateSpace.restart();
	}
	else
		doOnLoad = true;

}

void IuranusState::onUnload( ) {
	Global::cameraVibrationEnabled = false;

	// Clear timer logics and stop current music
	//LogicManager::getLogicManager()->cleanLogicTimerManager();
	//SoundManager::getSoundManager()->stopCurrentStream();
}

void IuranusState::loadObjects( ) {
	iuranusUI.create( );
}

void IuranusState::reloadShader( ) {
	iuranusUI.reloadShader( );
}

/*
void IuranusState::nextLevel( ) {
	PlanetManager * planetManager = PlanetManager::getPlanetManager();
	planetManager->setCurrentPlanet( "other1" );

	// -----------------

	// Aplicar la transformada del planeta actual a las cosas que lo requiran:
	// - Player Initial Position
	// - ¿Camera?
	// - Iuranus Elements
	// - Iuranus Animated Objects
	// - Iuranus Logic Volumes

	const Planet *planet = planetManager->getCurrentPlanet( );

	// Player
	const D3DXMATRIX &planetTransformMatrix = planet->getTransformMatrix( );
	Player *player = Player::getPlayer( );
	const D3DXMATRIX &playerTransformMatrix = player->getTransformMatrix( );
	D3DXMATRIX newTransformMatrix;
	D3DXMatrixMultiply( &newTransformMatrix, &playerTransformMatrix, &planetTransformMatrix );
	player->setTransformMatrix( newTransformMatrix );

	// Animated Objects (bushes)
	IuranusAnimatedObjectManager * iuranusAnimatedObjectManager = IuranusAnimatedObjectManager::getIuranusAnimatedObjectManager( );
	IuranusAnimatedObjectManager::iterator itAOM = iuranusAnimatedObjectManager->begin( );
	while( itAOM != iuranusAnimatedObjectManager->end( ) ) {
		(*itAOM)->setParentObject( planet );
		(*itAOM)->updateTransformMatrix( );

		++itAOM;
	}

	// Elements
	IuranusPlanetElementsManager * iuranusPlanetElementsManager = IuranusPlanetElementsManager::getIuranusPlanetElementsManager( );
	IuranusPlanetElementsManager::iterator itPEM = iuranusPlanetElementsManager->begin( );
	while( itPEM != iuranusPlanetElementsManager->end( ) ) {
		(itPEM->second)->setParentObject( planet );
		(itPEM->second)->updateTransformMatrix( );

		++itPEM;
	}

	// Logic Volumes
	LogicVolumeManager * logicVolumeManager = LogicVolumeManager::getLogicVolumeManager( );
	LogicVolumeManager::iterator itLVM = logicVolumeManager->begin( );
	while( itLVM != logicVolumeManager->end( ) ) {
		(itLVM->second)->setParentObject( planet );
		(itLVM->second)->updatePositions( );

		++itLVM;
	}

	// -----------------
		
	// PlayState OnUnload
	LogicManager::getLogicManager()->cleanLogicTimerManager();
	SoundManager::getSoundManager()->stopCurrentStream();

	// -----------------

	// EndIuranusState onUnload
	LogicManager::getLogicManager()->createUserEvent("iuranus_restart_planet");
	LogicManager::getLogicManager()->createUserEvent("iuranus_reset_enemies_in_screen");

	LogicManager::getLogicManager()->cleanLogicTimerManager();
	SoundManager::getSoundManager()->stopCurrentStream();

	IuranusEnemyManager::getIuranusEnemyManager()->clean();
	IuranusCollisionManager::getIuranusCollisionManager()->clean();

	// Leave player animation to idle
	player->setRelationToState( CYCLE, "idle", true, true );

	// Reset particles (destroy them)
	ParticlesManager::getParticlesManager( )->cleanUp( );

	// -----------------

	// PlayState Onload
	//LogicManager::getLogicManager()->createUserEvent("iuranus_restart_planet"); // This make all start again
	firstUpdateSpace=true;
	timeoutUpdateSpace.restart();
}
*/
