#include "InitBoilerState.h"
#include "BoilerState.h"
#include "Global/GlobalVariables.h"
#include "Game/Game.h"
#include "Managers/PlanetManager.h"
#include "Managers/PlanetAtmosphereManager.h"
#include "Managers/StaticObjectManager.h"
#include "Managers/Static2DObjectManager.h"
#include "Managers/SoundManager.h"
#include "Object/CollisionableObject/AnimatedObject/Player/Player.h"
#include "CameraControllers/ICameraController.h"
#include "Managers/DataLocatorManager.h"
#include "PlayerControllers/KeyFramed/PlayerKeyFramedROA.h"

#include "Managers/BoilerManagers/BoilerFixedParticlesManager.h"

std::string InitBoilerState::name = "init_boiler_state";

void InitBoilerState::update( float deltaTime ) {
	// FIX DELTA TIME FOR THE WAITING OF DESTROY EVERYTHING
	if( deltaTime > 1.0f/20.0f )
		deltaTime = 1.0f/60.0f;

	// Update camera 
	Global::camera->update(deltaTime);
	// Make camera goes near
	Global::camera->lookAt(Global::camera->getPosition() + Global::camera->getLocalFrontVector() * deltaTime * 5.0f, Global::camera->getTarget());

	// Update frustum
	Global::frustum.update();
	Global::backSideFrustum.update();

	// Update Player
	Player *player = Player::getPlayer();
	player->refreshPreviousTransformMatrix();
	player->update(deltaTime);

	// Update Planet Atmospheres positions
	PlanetAtmosphereManager::getPlanetAtmosphereManager()->update();

	// TODO Update BOILER Fixed Particles
	BoilerFixedParticlesManager::getBoilerFixedParticlesManager()->update(deltaTime);

	// Update of animated objects so these can be rendered
	//Player::getPlayer()->updateAnimation(deltaTime);

	/*
	ICameraController *cameraController = Global::camera->getCameraController();
	if(cameraController && cameraController->isFinished()) {
		getState("boiler_state")->getFocus();
	}
	*/

	IPlayerController *playerController = player->getPlayerController();
	const D3DXVECTOR3 &playerPosition = player->getPosition();
	const Planet *currentPlanet = PlanetManager::getPlanetManager()->getPlanet("boiler");
	const D3DXVECTOR3 &planetCenter = currentPlanet->getPosition();
	float distance = Global::distanceSqrt(playerPosition, planetCenter);
	static const float distanceToFinishState = 30.2f;
	if( (playerController && playerController->isFinished()) || distance < distanceToFinishState) {
		// Set player to a distanceToFinishState distance to avoid dying if deltatime is big and now
		// it is in a distance less than 20 and he in next state will be die
		player->setPosition( planetCenter + distanceToFinishState *player->getLocalUpVector( ) );

		BoilerState::cameraVibrationCurrentTime = 0.0f;
		BoilerState::doAnyVibration = true;
		getState("boiler_state")->getFocus();
	}
}

void InitBoilerState::preRender( ) {
}

void InitBoilerState::renderWorld3D( ) {
	Global::renderStateEnableZTest( );
	Global::renderStateEnableZWriting( );

	// Game Renders
	Player::getPlayer()->render();
	PlanetManager::getPlanetManager()->render();

	Global::renderStateEnableAlphaTest( );
	Global::renderStateEnableAlphaBlending( );
	StaticObjectManager::getStaticObjectManager()->render();
	Global::renderStateDisableAlphaBlending( );
	Global::renderStateDisableAlphaTest( );

	Global::renderStateDisableZWriting( );
	Global::renderStateDisableZTest( );
}

void InitBoilerState::renderWorld2D( ) {
	Global::renderStateEnableAlphaBlending( );
	Global::renderStateEnableZTest( );

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

void InitBoilerState::renderPostProcessed2D( ) {
}

void InitBoilerState::renderGameUI( ) {
}

void InitBoilerState::postRender( ) {
	// Print FPS
	Global::print(0,0, 0xffff0000, "Frames per second %0.2f", Game::fps);
	Global::print(0,10, 0xffff0000, "Init Boiler Status");
}

void InitBoilerState::renderFrame( ) {
	Global::renderTextureQuad.render( );
}

void InitBoilerState::onLoad( ) {
	PlanetManager * planetManager = PlanetManager::getPlanetManager();
	planetManager->setCurrentPlanet( "boiler" );

	// Load the target position to go from changing level point to boiler planet
	const Planet *planet = planetManager->getCurrentPlanet();
	const D3DXVECTOR3 &endTarget = planet->getPosition();
	D3DXVECTOR3 endPosition = endTarget + D3DXVECTOR3(0, 0, 60);
	Global::camera->setUpVector( D3DXVECTOR3(0,1,0) );
	Global::camera->lookAt( endPosition, endTarget );
	//Global::camera->loadTargetPointCameraController(endPosition, endTarget);
	Global::camera->loadDummyCameraController();

	// TODO Set the player in boiler planet
	Player *player = Player::getPlayer();
	player->setPlayerToInitPosition();
	player->setPosition( planet->getPosition( ) + D3DXVECTOR3(0.0f,0.0f,65.0f) );
	//player->setRelationToState(ACTION, "prepare_jump", true, true);
	//player->setRelationToState(CYCLE, "idle", true, true);

	D3DXQUATERNION rotationQuaternion;
	D3DXQuaternionRotationMatrix(&rotationQuaternion, &player->getTransformMatrix());

	PlayerKeyFramedROA *thrownInAnimation = new PlayerKeyFramedROA( thrownInAnimationResource, player->getPosition(), rotationQuaternion);
	player->setPlayerController( thrownInAnimation );
}

void InitBoilerState::onUnload( ) {
	Global::cameraVibrationEnabled = true;
	BoilerState *boilerState = (BoilerState *) getState( "boiler_state" );
	boilerState->boilerUI.showWelcomeMsg();
	SoundManager::getSoundManager()->playSample("player_boiler_arrival");
}

void InitBoilerState::loadObjects() {
	DataLocatorManager *dataLocatorManager = DataLocatorManager::getDataLocatorManager();
	const std::string &pathToRoa = dataLocatorManager->getPath("roa");

	// Animation Resources
	thrownInAnimationResource = new RigidAnimationResource();
	std::string filename = pathToRoa + "player_thrown_in.xml";
	bool ok = thrownInAnimationResource->initFromFile( filename.c_str() );
	assert(ok);
}
