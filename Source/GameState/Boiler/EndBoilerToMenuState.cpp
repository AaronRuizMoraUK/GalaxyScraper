#include "EndBoilerToMenuState.h"
#include "Global/GlobalVariables.h"
#include "Source/Game.h"
#include "GameState/MenuState.h"
#include "Managers/PlanetAtmosphereManager.h"
#include "Managers/PlanetManager.h"
#include "Managers/StaticObjectManager.h"
#include "Managers/Static2DObjectManager.h"
#include "Managers/ParticlesManager.h"
#include "Managers/SoundManager.h"
#include "Managers/LogicManager/LogicManager.h"
#include "Object/CollisionableObject/AnimatedObject/Player/Player.h"
#include "CameraControllers/ICameraController.h"
#include "Logic/PlanetLogic/BoilerPlanetLogic/BoilerPlanetLogic.h"

#include "Managers/BoilerManagers/BoilerFixedParticlesManager.h"
#include "Managers/BoilerManagers/BoilerAnimatedObjectManager.h"
#include "Managers/CollisionManager/BoilerCollisionManager.h"
#include "Managers/BoilerManagers/LeversManager.h"
#include "Managers/BoilerManagers/CanonsManager.h"
#include "Managers/BoilerManagers/BoilerEnemyManager.h"
#include "Managers/BoilerManagers/BoilerFireballManager.h"
#include "Managers/IuranusManagers/IuranusShadowManager.h"

std::string EndBoilerToMenuState::name = "end_boiler_to_menu_state";

void EndBoilerToMenuState::update( float deltaTime ) {
	// Update camera 
	Global::camera->update(deltaTime);

	// Update frustum
	Global::frustum.update();
	Global::backSideFrustum.update();

	// Update Planet Atmospheres positions
	PlanetAtmosphereManager::getPlanetAtmosphereManager()->update();

	// TODO Update BOILER Fixed Particles
	BoilerFixedParticlesManager::getBoilerFixedParticlesManager()->update(deltaTime);

	// Update of animated objects so these can be rendered
	Player::getPlayer()->updateAnimation(deltaTime);

	BoilerAnimatedObjectManager::getBoilerAnimatedObjectManager()->updateAnimation( deltaTime );

	ICameraController *cameraController = Global::camera->getCameraController();
	if(cameraController && cameraController->isFinished()) {
		getState("menu_state")->getFocus();
	}
}

void EndBoilerToMenuState::preRender( ) {
}

void EndBoilerToMenuState::renderWorld3D( ) {
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
	BoilerAnimatedObjectManager::getBoilerAnimatedObjectManager()->render( );
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

void EndBoilerToMenuState::renderWorld2D( ) {
	Global::renderStateEnableAlphaBlending( );
	Global::renderStateEnableZTest( );

	// Draw Shadows
	Global::renderStateEnableAlphaTest( );
	IuranusShadowManager::getIuranusShadowManager()->render();
	Global::renderStateDisableAlphaTest( );

	BoilerFixedParticlesManager::getBoilerFixedParticlesManager()->render();

	Global::renderStateEnableAdditiveAlphaBlending();
	Static2DObjectManager::getStatic2DObjectManager()->render();
	PlanetAtmosphereManager::getPlanetAtmosphereManager()->render();

	Global::renderStateDisableZTest( );
	Global::renderStateDisableAlphaBlending( );
}

void EndBoilerToMenuState::renderPostProcessed2D( ) {
}

void EndBoilerToMenuState::renderGameUI( ) {
}

void EndBoilerToMenuState::postRender( ) {
	// Print FPS
	Global::print(0,0, 0xffff0000, "Frames per second %0.2f", Game::fps);
	Global::print(0,10, 0xffff0000, "End Boiler To Menu Status");
}

void EndBoilerToMenuState::renderFrame( ) {
	Global::renderTextureQuad.render( );
}

void EndBoilerToMenuState::onLoad( ) {
	// If boiler ends because of going to menu, set the target point for the camera
	D3DXVECTOR3 endPosition(419.05f, 20.64f, 394.65f);
	MenuState *menuState = (MenuState *) getState( "menu_state" );
	D3DXVECTOR3 endTarget( menuState->menuPlanet.getPosition() + D3DXVECTOR3(0.0f, 5.0f, 4.0f) );
	Global::camera->setUpVector( D3DXVECTOR3(0,1,0) );
	Global::camera->loadTargetPointCameraController(endPosition, endTarget);

	LogicManager::getLogicManager()->cleanLogicTimerManager();
	SoundManager::getSoundManager()->stopCurrentStream();
	Global::samplesEnabled = false;
}

void EndBoilerToMenuState::onUnload( ) {
	// TODO Destroy everything abour boiler
	Global::cameraVibrationEnabled = false;
	
	// Reset continues and planet logic states
	BoilerPlanetLogic *boilerPlanetLogic = BoilerPlanetLogic::getBoilerPlanetLogic();
	boilerPlanetLogic->resetContinues();
	boilerPlanetLogic->alreadyEnteredBossZone = false;

	LogicManager * logicManager = LogicManager::getLogicManager();
	logicManager->cleanLogicTimerManager();
	SoundManager::getSoundManager()->stopCurrentStream();

	BoilerCollisionManager::getBoilerCollisionManager()->clean();
	
	// Reset levers and cannons states
	LeversManager::getLeversManager()->restart();
	CanonsManager::getCanonsManager()->restart();

	// Reset door state
	BoilerAnimatedObjectManager::getBoilerAnimatedObjectManager()->restart();

	BoilerEnemyManager::getBoilerEnemyManager()->restart();

	// Leave player animation to idle
	Player *player = Player::getPlayer( );
	player->setRelationToState( CYCLE, "idle", true, true );

	// Reset particles (destroy them)
	ParticlesManager::getParticlesManager( )->cleanUp( );
	BoilerFireballManager::getBoilerFireballManager()->renewParticles();

	logicManager->createUserEvent("boiler_restart_planet_to_menu");
}
