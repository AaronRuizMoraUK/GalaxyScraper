#include "EndIuranusToMenuState.h"
#include "Global/GlobalFunctions.h"
#include "Global/GlobalVariables.h"
#include "Source/Game.h"
#include "GameState/MenuState.h"
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
#include "Logic/PlanetLogic/IuranusPlanetLogic/IuranusPlanetLogic.h"
#include <cassert>

std::string EndIuranusToMenuState::name = "end_iuranus_to_menu_state";

void EndIuranusToMenuState::update( float deltaTime ) {
	// Update camera 
	Global::camera->update(deltaTime);

	// Update frustum
	Global::frustum.update();
	Global::backSideFrustum.update();

	// Update Planet Atmospheres positions
	PlanetAtmosphereManager::getPlanetAtmosphereManager()->update();

	// Update IURANUS Fixed Particles
	IuranusFixedParticlesManager::getIuranusFixedParticlesManager()->update(deltaTime);

	// Update of animated objects so these can be rendered
	Player::getPlayer()->updateAnimation(deltaTime);
	IuranusEnemyManager::getIuranusEnemyManager()->updateAnimation(deltaTime);
	IuranusPlanetElementsManager::getIuranusPlanetElementsManager()->updateAnimation(deltaTime);
	IuranusAnimatedObjectManager::getIuranusAnimatedObjectManager()->updateAnimation(deltaTime);

	ICameraController *cameraController = Global::camera->getCameraController();
	if(cameraController && cameraController->isFinished()) {
		getState("menu_state")->getFocus();
	}
}

void EndIuranusToMenuState::preRender( ) {
}

void EndIuranusToMenuState::renderWorld3D( ) {
	Global::renderStateEnableZTest( );
	Global::renderStateEnableZWriting( );

	// Menu State Renders
	MenuState *menuState = (MenuState *) getState( "menu_state" );
	if( isVisible(menuState->menuPlanet) )
		menuState->menuPlanet.render();
	if( isVisible(menuState->startAction) )
		menuState->startAction.render();
	if( isVisible(menuState->optionsAction) )
		menuState->optionsAction.render();
	if( isVisible(menuState->creditsAction) )
		menuState->creditsAction.render();
	if( isVisible(menuState->exitAction) )
		menuState->exitAction.render();

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

void EndIuranusToMenuState::renderWorld2D( ) {
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

void EndIuranusToMenuState::renderPostProcessed2D( ) {
}

void EndIuranusToMenuState::renderGameUI( ) {
}

void EndIuranusToMenuState::postRender( ) {
	// Print FPS
	Global::print(0,0, 0xffff0000, "Frames per second %0.2f", Game::fps);
	Global::print(0,10, 0xffff0000, "End Iuranus To Menu Status");
}

void EndIuranusToMenuState::renderFrame( ) {
	Global::renderTextureQuad.render( );
}

void EndIuranusToMenuState::onLoad( ) {

	// If iuranus ends because of going to menu, set the target point for the camera
	D3DXVECTOR3 endPosition(419.05f, 20.64f, 394.65f);
	MenuState *menuState = (MenuState *) getState( "menu_state" );
	D3DXVECTOR3 endTarget( menuState->menuPlanet.getPosition() + D3DXVECTOR3(0.0f, 5.0f, 4.0f) );
	Global::camera->setUpVector( D3DXVECTOR3(0,1,0) );
	Global::camera->loadTargetPointCameraController(endPosition, endTarget);

	LogicManager::getLogicManager()->cleanLogicTimerManager();
	SoundManager::getSoundManager()->stopCurrentStream();
	Global::samplesEnabled = false;
}

void EndIuranusToMenuState::onUnload( ) {
	// Destroy everything abour iuranus
	LogicManager * logicManager = LogicManager::getLogicManager();
	logicManager->createUserEvent("iuranus_restart_planet");
	logicManager->createUserEvent("iuranus_reset_enemies_in_screen");

	// Reset continues
	IuranusPlanetLogic *iuranusPlanetLogic = IuranusPlanetLogic::getIuranusPlanetLogic();
	iuranusPlanetLogic->resetContinues();

	logicManager->cleanLogicTimerManager();
	SoundManager::getSoundManager()->stopCurrentStream();

	IuranusEnemyManager::getIuranusEnemyManager()->clean();
	IuranusCollisionManager::getIuranusCollisionManager()->clean();

	// Leave player animation to idle
	Player *player = Player::getPlayer( );
	player->setRelationToState( CYCLE, "idle", true, true );

	// Reset particles (destroy them)
	ParticlesManager::getParticlesManager( )->cleanUp( );
}
