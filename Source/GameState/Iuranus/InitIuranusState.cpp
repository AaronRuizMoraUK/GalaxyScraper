#include "InitIuranusState.h"
#include "IuranusState.h"
#include "Global/GlobalFunctions.h"
#include "Global/GlobalVariables.h"
#include "Source/Game.h"
#include "GameState/MenuState.h"
#include "Managers/IuranusManagers/IuranusPlanetElementsManager.h"
#include "Managers/PlanetManager.h"
#include "Managers/StaticObjectManager.h"
#include "Managers/Static2DObjectManager.h"
#include "Managers/IuranusManagers/IuranusAnimatedObjectManager.h"
#include "Managers/PlanetAtmosphereManager.h"
#include "Managers/IuranusManagers/IuranusFixedParticlesManager.h"
#include "Object/CollisionableObject/AnimatedObject/Player/Player.h"
#include "CameraControllers/ICameraController.h"
#include <cassert>

std::string InitIuranusState::name = "init_iuranus_state";

void InitIuranusState::update( float deltaTime ) {
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
	IuranusPlanetElementsManager::getIuranusPlanetElementsManager()->updateAnimation(deltaTime);
	IuranusAnimatedObjectManager::getIuranusAnimatedObjectManager()->updateAnimation(deltaTime);

	// If camera controller has finished go to iuranus state
	ICameraController *cameraController = Global::camera->getCameraController();
	if(cameraController && cameraController->isFinished()) {
		getState("iuranus_state")->getFocus();
		return;
	}
}

void InitIuranusState::preRender( ) {
}

void InitIuranusState::renderWorld3D( ) {
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
	IuranusPlanetElementsManager::getIuranusPlanetElementsManager()->render();
	
	Global::renderStateEnableAlphaTest( );
	Global::renderStateEnableAlphaBlending( );
	StaticObjectManager::getStaticObjectManager()->render();
	Global::renderStateDisableAlphaBlending( );
	Global::renderStateDisableAlphaTest( );

	Global::renderStateDisableZWriting( );
	Global::renderStateDisableZTest( );
}

void InitIuranusState::renderWorld2D( ) {
	Global::renderStateEnableAlphaBlending( );
	Global::renderStateEnableZTest( );

	Global::renderStateEnableAlphaTest( );
	// Draw Bushes
	IuranusAnimatedObjectManager::getIuranusAnimatedObjectManager()->render();
	Global::renderStateDisableAlphaTest( );

	// Render IURANUS Fixed Particles
	IuranusFixedParticlesManager::getIuranusFixedParticlesManager()->render();

	Global::renderStateEnableAdditiveAlphaBlending();
	Static2DObjectManager::getStatic2DObjectManager()->render();
	PlanetAtmosphereManager::getPlanetAtmosphereManager()->render();

	Global::renderStateDisableZTest( );
	Global::renderStateDisableAlphaBlending( );
}

void InitIuranusState::renderPostProcessed2D( ) {
}

void InitIuranusState::renderGameUI( ) {
}

void InitIuranusState::postRender( ) {
	// Print FPS
	Global::print(0,0, 0xffff0000, "Frames per second %0.2f", Game::fps);
	Global::print(0,10, 0xffff0000, "Init Play Status");
}

void InitIuranusState::renderFrame( ) {
	Global::renderTextureQuad.render( );
}

void InitIuranusState::onLoad( ) {
	// Play sound
	//SoundManager::getSoundManager();

	// Set Iuranus planet as current planet
	PlanetManager * planetManager = PlanetManager::getPlanetManager();
	planetManager->setCurrentPlanet( "iuranus" );

	// Load the target position to go from menu to iuranus planet
	const Planet *planet = planetManager->getCurrentPlanet();
	const D3DXVECTOR3 &endTarget = planet->getPosition();
	D3DXVECTOR3 endPosition = endTarget + D3DXVECTOR3(0, 0, 31);
	Global::camera->setUpVector( D3DXVECTOR3(0,1,0) );
	Global::camera->loadTargetPointCameraController(endPosition, endTarget);

	/*
	Global::camera->setUpVector( D3DXVECTOR3(0,1,0) );
	Global::camera->loadKeyFramedCameraController( "camera_world.xml", false );
	*/
}

void InitIuranusState::onUnload( ) {
	IuranusState *iuranusState = (IuranusState *) getState( "iuranus_state" );
	iuranusState->iuranusUI.showWelcomeMsg();
}
