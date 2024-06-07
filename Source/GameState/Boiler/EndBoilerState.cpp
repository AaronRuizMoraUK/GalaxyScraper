#include "EndBoilerState.h"
#include "BoilerState.h"
#include "Global/GlobalFunctions.h"
#include "Global/GlobalVariables.h"
#include "Source/Game.h"
#include "CameraControllers/ICameraController.h"

#include "Managers/PlanetManager.h"
#include "Managers/PlanetAtmosphereManager.h"
#include "Managers/ParticlesManager.h"
#include "Managers/StaticObjectManager.h"
#include "Managers/Static2DObjectManager.h"
#include "Object/CollisionableObject/AnimatedObject/Player/Player.h"

#include "Managers/BoilerManagers/BoilerFixedParticlesManager.h"
#include "Managers/BoilerManagers/BoilerAnimatedObjectManager.h"
#include "Managers/BoilerManagers/LeversManager.h"
#include "Managers/BoilerManagers/CanonsManager.h"
#include "Managers/BoilerManagers/BoilerEnemyManager.h"
#include "Managers/IuranusManagers/IuranusShadowManager.h"

std::string EndBoilerState::name = "end_boiler_state";

void EndBoilerState::update( float deltaTime ) {
	// Update camera 
	Global::camera->update(deltaTime);

	// Update frustum
	Global::frustum.update();
	Global::backSideFrustum.update();

	// Update Planet Atmospheres positions
	PlanetAtmosphereManager::getPlanetAtmosphereManager()->update();

	// Update Particles
	ParticlesManager::getParticlesManager()->update(deltaTime);
	BoilerFixedParticlesManager::getBoilerFixedParticlesManager()->update(deltaTime);

	BoilerEnemyManager::getBoilerEnemyManager()->updateAnimation(deltaTime);

	ICameraController *cameraController = Global::camera->getCameraController();
	if(cameraController && cameraController->isFinished()) {
		getState("end_game_boiler_state")->getFocus();
	}
}

void EndBoilerState::preRender( ) {
}

void EndBoilerState::renderWorld3D( ) {
	Global::renderStateEnableZTest( );
	Global::renderStateEnableZWriting( );

	// Draw player
	Player::getPlayer()->render();

	// Draw Scene
	PlanetManager::getPlanetManager()->render();
	BoilerAnimatedObjectManager::getBoilerAnimatedObjectManager()->render( );
	BoilerEnemyManager::getBoilerEnemyManager()->render();
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

void EndBoilerState::renderWorld2D( ) {
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

void EndBoilerState::renderPostProcessed2D( ) {
}

void EndBoilerState::renderGameUI( ) {
}

void EndBoilerState::postRender( ) {
	// Print FPS
	Global::print(0,0, 0xffff0000, "Frames per second %0.2f", Game::fps);
	Global::print(0,10, 0xffff0000, "End Boiler Status");
}

void EndBoilerState::renderFrame( ) {
	Global::renderTextureQuad.render( );
}

void EndBoilerState::onLoad( ) {
	// If iuranus ends because of going to boiler planet set the target point for the
	// changing level point
	const Planet *boiler = PlanetManager::getPlanetManager()->getPlanet("boiler");
	const D3DXVECTOR3 &boilerPos = boiler->getPosition();

	D3DXVECTOR3 endPosition( 0.0f, -50.0f, 200.0f );
	Global::camera->setUpVector( D3DXVECTOR3(0,1,0) );
	Global::camera->loadTargetPointCameraController(endPosition, boilerPos);
}

void EndBoilerState::onUnload( ) {
}
