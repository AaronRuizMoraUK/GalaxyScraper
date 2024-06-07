#include "IuranusToBoilerTransitionState.h"
#include "GameState/Boiler/BoilerState.h"
#include "Global/GlobalFunctions.h"
#include "Global/GlobalVariables.h"
#include "Source/Game.h"
#include "Object/CollisionableObject/AnimatedObject/Player/Player.h"
#include "Managers/PlanetAtmosphereManager.h"
#include "Managers/IuranusManagers/IuranusFixedParticlesManager.h"
#include "Managers/LogicManager/LogicManager.h"
#include "Managers/PlanetManager.h"
#include "Managers/StaticObjectManager.h"
#include "Managers/Static2DObjectManager.h"
#include "Managers/IuranusManagers/IuranusPlanetElementsManager.h"
#include "Managers/IuranusManagers/IuranusAnimatedObjectManager.h"
#include "Managers/BoilerManagers/BoilerEnemyManager.h"
#include "Managers/BoilerManagers/LeversManager.h"
#include "Managers/BoilerManagers/CanonsManager.h"
#include "Managers/DataLocatorManager.h"
#include "PlayerControllers/KeyFramed/PlayerKeyFramedROA.h"
#include "Factories/QuadFactory.h"

// ---------------------------------------------------------------
#include "Managers/BoilerManagers/BoilerAnimatedObjectManager.h"

// ---------------------------------------------------------------
#include "Managers/IuranusManagers/IuranusEnemyManager.h"
#include "Managers/CollisionManager/IuranusCollisionManager.h"
#include "Managers/ParticlesManager.h"
#include "Managers/SoundManager.h"

std::string IuranusToBoilerTransitionState::name = "iuranus_to_boiler_transition_state";

void IuranusToBoilerTransitionState::update( float deltaTime ) {

	// Update Player -- RECEMOS
	Player *player = Player::getPlayer();
	player->refreshPreviousTransformMatrix();
	player->update(deltaTime);

	// Update camera 
	Global::camera->update(deltaTime);
	
	// Update frustum
	Global::frustum.update();
	Global::backSideFrustum.update();

	updateGlowPlayer( deltaTime );

	// Update Planet Atmospheres positions
	PlanetAtmosphereManager::getPlanetAtmosphereManager()->update();

	// Update IURANUS Fixed Particles
	IuranusFixedParticlesManager::getIuranusFixedParticlesManager()->update(deltaTime);

	// Update of animated objects so these can be rendered
	player->updateAnimation(deltaTime);
	BoilerEnemyManager::getBoilerEnemyManager()->updateAnimation(deltaTime);
	//IuranusPlanetElementsManager::getIuranusPlanetElementsManager()->updateAnimation(deltaTime);
	//IuranusAnimatedObjectManager::getIuranusAnimatedObjectManager()->updateAnimation(deltaTime);

	LogicManager::getLogicManager()->update(deltaTime);

	IPlayerController *playerController = player->getPlayerController();
	if( playerController && playerController->isFinished() ) {
		//playerController->restart();
		getState("init_boiler_state")->getFocus();
	}

	/*
	IPlayerController *playerController = player->getPlayerController();
	if(playerController && playerController->isFinished()) {
		//playerController->restart();
		getState("end_game_iuranus_state")->getFocus();
	}
	*/

}

void IuranusToBoilerTransitionState::updateGlowPlayer( float deltaTime ) {

	D3DXMATRIX transform;
	D3DXMatrixIdentity(&transform);

	// Player center and radius
	D3DXVECTOR3 center = Player::getPlayer()->getPosition()+D3DXVECTOR3(0.0f,0.8f,0.0f);
	float radius = 1.0f ;

	// Direction
	const D3DXVECTOR3 &cameraPosition = Global::camera->getPosition();
	D3DXVECTOR3 localFront( cameraPosition - center );
	Global::normalizeVector3(localFront, localFront);

	// Position
	D3DXVECTOR3 position(center);
	position += radius*localFront;

	// Rotation
	const D3DXVECTOR3 &cameraLocalUp = Global::camera->getLocalUpVector();
	D3DXVECTOR3 localLeft;
	D3DXVec3Cross(&localLeft, &cameraLocalUp, &localFront);
	Global::normalizeVector3(localLeft, localLeft);
	D3DXVECTOR3 localUp;
	D3DXVec3Cross(&localUp, &localFront, &localLeft);

	// Update matWorld
	transform(0,0) = localLeft.x;
	transform(0,1) = localLeft.y;
	transform(0,2) = localLeft.z;
	transform(1,0) = localUp.x;
	transform(1,1) = localUp.y;
	transform(1,2) = localUp.z;
	transform(2,0) = localFront.x;
	transform(2,1) = localFront.y;
	transform(2,2) = localFront.z;

	// Bigger and more little
	static float delta_scale = 0.0f;
	const static float value_scale = 85.0f;
	delta_scale += deltaTime * value_scale;
	float scaleUniform = 4.0f + 1.0f * sin( delta_scale );
	D3DXMATRIX newScale;
	D3DXMatrixScaling( &newScale, scaleUniform, scaleUniform, scaleUniform);
	D3DXMatrixMultiply(&newScale, &transform, &newScale);

	// Rotation over local up
	static float delta_rotation = 0.0f;
	const static float value_rotation = 88.0f;	
	delta_rotation += deltaTime * value_rotation;
	D3DXMATRIX newRot;
	D3DXMatrixRotationAxis( &newRot, &localFront, delta_rotation );
	D3DXMatrixMultiply(&newRot, &newScale, &newRot);

	glowPlayer.setTransformMatrix(newRot);
	glowPlayer.setPosition(position);
}

void IuranusToBoilerTransitionState::preRender( ) {
}

void IuranusToBoilerTransitionState::renderWorld3D( ) {
	Global::renderStateEnableZTest( );
	Global::renderStateEnableZWriting( );

	// Game Renders
	Player::getPlayer()->render();
	PlanetManager::getPlanetManager()->render();
	BoilerAnimatedObjectManager::getBoilerAnimatedObjectManager()->render( );
	IuranusPlanetElementsManager::getIuranusPlanetElementsManager()->render( );
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

void IuranusToBoilerTransitionState::renderWorld2D( ) {
	Global::renderStateEnableAlphaBlending( );
	Global::renderStateEnableZTest( );

	Global::renderStateEnableAlphaTest( );
	// Draw Bushes
	IuranusAnimatedObjectManager::getIuranusAnimatedObjectManager()->render();
	Global::renderStateDisableAlphaTest( );

	// Render IURANUS Fixed Particles
	IuranusFixedParticlesManager::getIuranusFixedParticlesManager()->render();

	if( Global::enabledRenderTexture ) {
		// Render To Texture to do the air lava effect
		//Global::renderStateEnableAlphaBlending( );
		BoilerState * boilerState = (BoilerState *) getState( "boiler_state" );
		boilerState->lavaDistorsion.render( );
	}

	Global::renderStateEnableAdditiveAlphaBlending();
	Static2DObjectManager::getStatic2DObjectManager()->render();
	PlanetAtmosphereManager::getPlanetAtmosphereManager()->render();

	glowPlayer.render( );

	Global::renderStateDisableZTest( );
	Global::renderStateDisableAlphaBlending( );

	//const D3DXVECTOR3 &cameraPos = Global::camera->getPosition( );
	//const D3DXVECTOR3 &cameraTar = Global::camera->getTarget( );
	//Global::print(0,40, 0xffff0000, "Camera position: (%f, %f, %f)", cameraPos.x, cameraPos.y, cameraPos.z);
	//Global::print(0,50, 0xffff0000, "Camera targert:  (%f, %f, %f)", cameraTar.x, cameraTar.y, cameraTar.z);
}

void IuranusToBoilerTransitionState::renderPostProcessed2D( ) {
}

void IuranusToBoilerTransitionState::renderGameUI( ) {
}

void IuranusToBoilerTransitionState::postRender( ) {
	// Print FPS
	Global::print(0,0, 0xffff0000, "Frames per second %0.2f", Game::fps);
	Global::print(0,10, 0xffff0000, "Iuranus To Boiler Transition State");
}

void IuranusToBoilerTransitionState::renderFrame( ) {
	Global::renderTextureQuad.render( );
}

void IuranusToBoilerTransitionState::onLoad( ) {

	LogicManager::getLogicManager()->createUserEvent("iuranus_boiler_transition");
	
	// Changes player controller
	Player *player = Player::getPlayer( );
	player->setPosition( D3DXVECTOR3( -21.2f, -57.6f, -7.0f) );
	player->setRelationToState( CYCLE, "transition", true, true );

	PlayerKeyFramedROA *transitionPlayerAnimation = new PlayerKeyFramedROA( transitionPlayerAnimationResource );
	player->setPlayerController( transitionPlayerAnimation );

	//D3DXVECTOR3 endPosition( -355.1598f, -59.313f,    -231.6574f );
	//D3DXVECTOR3 endTarget( -354.388702f, -59.211868f, -231.0289f );

	//D3DXVECTOR3 endPosition( player->getPosition( ) - player->getLocalFrontVector( ) * 5.0f );
	//D3DXVECTOR3 endPosition( -109.754f, -87.782f, 109.117f ); // 3-a
	D3DXVECTOR3 endPosition( -97.3f, -87.7f, 63.9f ); // 3-b
	const D3DXVECTOR3 &endTarget = player->getPosition( );
	Global::camera->setUpVector( D3DXVECTOR3(0,1,0) );
	Global::camera->lookAt( endPosition, endTarget );
	Global::camera->loadFollowPlayerCameraController( );
}

void IuranusToBoilerTransitionState::onUnload( ) {
	//Player *player = Player::getPlayer( );
	//player->setRelationToState( CYCLE, "idle", true, true );



	// ---------------------------------------------------------------
	// Destroy everything abour iuranus
	LogicManager * logicManager = LogicManager::getLogicManager();
	logicManager->createUserEvent("iuranus_to_boiler");
	logicManager->createUserEvent("iuranus_reset_enemies_in_screen");

	logicManager->cleanLogicTimerManager();
	SoundManager::getSoundManager()->stopCurrentStream();

	IuranusEnemyManager::getIuranusEnemyManager()->clean();
	IuranusCollisionManager::getIuranusCollisionManager()->clean();

	// Reset particles (destroy them)
	ParticlesManager::getParticlesManager( )->cleanUp( );
}

void IuranusToBoilerTransitionState::loadObjects() {
	DataLocatorManager *dataLocatorManager = DataLocatorManager::getDataLocatorManager();
	const std::string &pathToRoa = dataLocatorManager->getPath("roa");

	// Animation Resources
	transitionPlayerAnimationResource = new RigidAnimationResource();
	std::string filename = pathToRoa + "player_transition.xml";
	bool ok = transitionPlayerAnimationResource->initFromFile( filename.c_str() );
	assert(ok);

	// Glow Player
	D3DXMATRIX glowPlayertransformMatrix;
	D3DXMatrixIdentity(&glowPlayertransformMatrix);
	glowPlayer.setTransformMatrix(glowPlayertransformMatrix);

	glowPlayer.SETMATERIAL("estela.dds", "shader.fx", "quad_tech");

	IMesh * glowMesh = QuadFactory::getQuadFactory()->createQuad(1.0f, 1.0f);
	assert(glowMesh);
	glowPlayer.setMesh(glowMesh);
}

void IuranusToBoilerTransitionState::reloadShader() {
	glowPlayer.reloadShader();
}