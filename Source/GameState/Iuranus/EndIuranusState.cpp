#include "EndIuranusState.h"
#include "Global/GlobalFunctions.h"
#include "Global/GlobalVariables.h"
#include "Game/Game.h"
#include "Managers/IuranusManagers/IuranusEnemyManager.h"
#include "Managers/IuranusManagers/IuranusPlanetElementsManager.h"
#include "Managers/PlanetManager.h"
#include "Managers/StaticObjectManager.h"
#include "Managers/Static2DObjectManager.h"
#include "Managers/IuranusManagers/IuranusAnimatedObjectManager.h"
#include "Managers/PlanetAtmosphereManager.h"
#include "Managers/ParticlesManager.h"
#include "Managers/IuranusManagers/IuranusFixedParticlesManager.h"
#include "Managers/CollisionManager/IuranusCollisionManager.h"
#include "Managers/LogicManager/LogicManager.h"
#include "Managers/SoundManager.h"
#include "Managers/IuranusManagers/IuranusShadowManager.h"
#include "Object/CollisionableObject/AnimatedObject/Player/Player.h"
#include "CameraControllers/ICameraController.h"
#include "Managers/DataLocatorManager.h"
#include "PlayerControllers/KeyFramed/PlayerKeyFramedROA.h"
#include <cassert>

std::string EndIuranusState::name = "end_iuranus_state";

void EndIuranusState::update( float deltaTime ) {
	// Update camera 
	Global::camera->update(deltaTime);
	// Make camera goes far
	Global::camera->lookAt(Global::camera->getPosition() - Global::camera->getLocalFrontVector() * deltaTime * 3.0f, Global::camera->getTarget());

	// Update frustum
	Global::frustum.update();
	Global::backSideFrustum.update();

	// Update Player
	Player *player = Player::getPlayer();
	player->refreshPreviousTransformMatrix();
	player->update(deltaTime);

	// Update Planet Atmospheres positions
	PlanetAtmosphereManager::getPlanetAtmosphereManager()->update();

	// Update IURANUS Fixed Particles
	IuranusFixedParticlesManager::getIuranusFixedParticlesManager()->update(deltaTime);

	// Update of animated objects so these can be rendered
	//player->updateAnimation(deltaTime);
	IuranusEnemyManager::getIuranusEnemyManager()->updateAnimation(deltaTime);
	IuranusPlanetElementsManager::getIuranusPlanetElementsManager()->updateAnimation(deltaTime);
	IuranusAnimatedObjectManager::getIuranusAnimatedObjectManager()->updateAnimation(deltaTime);

	LogicManager::getLogicManager()->update(deltaTime);

	/*
	ICameraController *cameraController = Global::camera->getCameraController();
	if(cameraController && cameraController->isFinished()) {
		getState("end_game_iuranus_state")->getFocus();
	}
	*/

	IPlayerController *playerController = player->getPlayerController();
	if(playerController && playerController->isFinished()) {
		getState("iuranus_to_boiler_transition_state")->getFocus();
	}
}

void EndIuranusState::preRender( ) {
}

void EndIuranusState::renderWorld3D( ) {
	Global::renderStateEnableZTest( );
	Global::renderStateEnableZWriting( );

	// Game Renders
	Player::getPlayer()->render();
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

void EndIuranusState::renderWorld2D( ) {
	Global::renderStateEnableAlphaBlending( );
	Global::renderStateEnableZTest( );

	Global::renderStateEnableAlphaTest( );
	// Draw Bushes
	IuranusAnimatedObjectManager::getIuranusAnimatedObjectManager()->render();

	// Draw Shadows
	IuranusShadowManager::getIuranusShadowManager()->render();
	Global::renderStateDisableAlphaTest( );

	// Render IURANUS Fixed Particles
	IuranusFixedParticlesManager::getIuranusFixedParticlesManager()->render();

	Global::renderStateEnableAdditiveAlphaBlending();
	Static2DObjectManager::getStatic2DObjectManager()->render();
	PlanetAtmosphereManager::getPlanetAtmosphereManager()->render();

	Global::renderStateDisableZTest( );
	Global::renderStateDisableAlphaBlending( );
}

void EndIuranusState::renderPostProcessed2D( ) {
}

void EndIuranusState::renderGameUI( ) {
}

void EndIuranusState::postRender( ) {
	// Print FPS
	Global::print(0,0, 0xffff0000, "Frames per second %0.2f", Game::fps);
	Global::print(0,10, 0xffff0000, "End Iuranus Status");
}

void EndIuranusState::renderFrame( ) {
	Global::renderTextureQuad.render( );
}

void EndIuranusState::onLoad( ) {

	// If iuranus ends because of going to boiler planet set the target point for the
	// changing level point
	//D3DXVECTOR3 endPosition( 0.0f, -50.0f, 200.0f );
	//D3DXVECTOR3 endTarget( 0.0f, 0.0f, 0.0f );
	//Global::camera->setUpVector( D3DXVECTOR3(0,1,0) );
	//Global::camera->loadTargetPointCameraController(endPosition, endTarget);
	Global::camera->loadDummyCameraController();

	LogicManager::getLogicManager()->cleanLogicTimerManager();
	SoundManager::getSoundManager()->stopCurrentStream();

	// Destroy iuranus planet timer
	LogicManager * logicManager = LogicManager::getLogicManager();
	logicManager->deleteTimer("iuranus_planet_timeout");

	// Changes player controller
	Player *player = Player::getPlayer( );
	//player->setRelationToState( CYCLE, "thrown", true, true );

	D3DXQUATERNION rotationQuaternion;
	D3DXQuaternionRotationMatrix(&rotationQuaternion, &player->getTransformMatrix());

	PlayerKeyFramedROA *thrownOutAnimation = new PlayerKeyFramedROA( thrownOutAnimationResource, player->getPosition(), rotationQuaternion);
	player->setPlayerController( thrownOutAnimation );

	Global::cameraVibrationEnabled = true;
	LogicManager::getLogicManager()->createTimer( "disable_camera_vibration", 0.6f);
}

void EndIuranusState::onUnload( ) {
	Global::cameraVibrationEnabled = false;
	SoundManager::getSoundManager()->stopCurrentStream();

	// Leave player animation to idle
	Player *player = Player::getPlayer( );
	// player->setRelationToState(ACTION, "finish_jump", false, true);
	//player->setRelationToState( CYCLE, "idle", true, true );

	/*
	Global::cameraVibrationEnabled = false;

	// Destroy everything abour iuranus
	LogicManager * logicManager = LogicManager::getLogicManager();
	logicManager->createUserEvent("iuranus_to_boiler");
	logicManager->createUserEvent("iuranus_reset_enemies_in_screen");

	logicManager->cleanLogicTimerManager();
	SoundManager::getSoundManager()->stopCurrentStream();

	IuranusEnemyManager::getIuranusEnemyManager()->clean();
	IuranusCollisionManager::getIuranusCollisionManager()->clean();

	// Leave player animation to idle
	Player *player = Player::getPlayer( );
	player->setRelationToState( CYCLE, "idle", true, true );

	// Reset particles (destroy them)
	ParticlesManager::getParticlesManager( )->cleanUp( );
	*/
}

void EndIuranusState::loadObjects() {
	DataLocatorManager *dataLocatorManager = DataLocatorManager::getDataLocatorManager();
	const std::string &pathToRoa = dataLocatorManager->getPath("roa");

	// Animation Resources
	thrownOutAnimationResource = new RigidAnimationResource();
	std::string filename = pathToRoa + "player_thrown_out.xml";
	bool ok = thrownOutAnimationResource->initFromFile( filename.c_str() );
	assert(ok);
}
