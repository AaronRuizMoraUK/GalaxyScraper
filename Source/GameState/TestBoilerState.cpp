#include <Windows.h>
#include "TestBoilerState.h"
#include "Global/GlobalFunctions.h"
#include "Global/GlobalVariables.h"
#include "Game/Game.h"
#include "Object/CollisionableObject/AnimatedObject/Player/Player.h"
#include "Managers/PlanetManager.h"
#include "Managers/PlanetAtmosphereManager.h"
#include "Managers/ParticlesManager.h"
#include "Managers/CollisionManager/IuranusCollisionManager.h"
#include "Managers/StaticObjectManager.h"
#include "Managers/Static2DObjectManager.h"
#include "CameraControllers/SSPlayer/SSPlayerCameraController.h"

std::string TestBoilerState::name = "test_boiler_state";

TestBoilerState::TestBoilerState( )
: IGameState()
, timeoutUpdateSpace(0.1f)
, firstUpdateSpace(true)
{
}

TestBoilerState::~TestBoilerState()
{
	OutputDebugString("Destroying TestBoilerState...\n");
};

void TestBoilerState::updateCameraValues( float deltaTime ) {
	SSPlayerCameraController * cameraController = (SSPlayerCameraController *) Global::camera->getCameraController();

	// Change value
	if( Global::isPushed( 'U' ) ) {
		float cameraDistanceValue = cameraController->getDistance() + deltaTime*4.0f;
		cameraController->setDistance( cameraDistanceValue );
	}
	else if( Global::isPushed( 'J' ) ) {
		float cameraDistanceValue = cameraController->getDistance() - deltaTime*4.0f;
		cameraController->setDistance( cameraDistanceValue );
	}
	else if( Global::isPushed( 'I' ) ) {
		float cameraPositionSpeedValue = cameraController->getPositionSpeed() + deltaTime*2.0f;
		cameraController->setPositionSpeed( cameraPositionSpeedValue );
	}
	else if( Global::isPushed( 'K' ) ) {
		float cameraPositionSpeedValue = cameraController->getPositionSpeed() - deltaTime*2.0f;
		cameraController->setPositionSpeed( cameraPositionSpeedValue );
	}
	else if( Global::isPushed( 'O' ) ) {
		float cameraTargetSpeedValue = cameraController->getTargetSpeed() + deltaTime*2.0f;
		cameraController->setTargetSpeed( cameraTargetSpeedValue );
	}
	else if( Global::isPushed( 'L' ) ) {
		float cameraTargetSpeedValue = cameraController->getTargetSpeed() - deltaTime*2.0f;
		cameraController->setTargetSpeed( cameraTargetSpeedValue );
	}
	
}

void TestBoilerState::update( float deltaTime ) {

	updateCameraValues( deltaTime );

	// Update camera 
	Global::camera->update(deltaTime);

	// Update frustum
	Global::frustum.update();
	Global::backSideFrustum.update();

	Player *player = Player::getPlayer();
	// Update Player
	player->refreshPreviousTransformMatrix();
	player->update(deltaTime);

	player->updateAnimation(deltaTime);

	// Update Planet Atmospheres positions
	PlanetAtmosphereManager::getPlanetAtmosphereManager()->update();

	// Update Particles
	ParticlesManager::getParticlesManager()->update(deltaTime);

	// Update Space
	PolarAnglesPartitioner *polarAnglesPartitioner = PolarAnglesPartitioner::getPolarAnglesPartitioner();
	timeoutUpdateSpace.update(deltaTime);
	if(timeoutUpdateSpace.isFinished() || firstUpdateSpace) {
		firstUpdateSpace=false;
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

}

void TestBoilerState::preRender( ) {
}

void TestBoilerState::renderWorld3D( ) {
	Global::renderStateEnableZTest( );
	Global::renderStateEnableZWriting( );

	// Draw player
	Player::getPlayer()->render();

	// Draw Scene
	PlanetManager::getPlanetManager()->render();
	
	Global::renderStateEnableAlphaTest( );
	Global::renderStateEnableAlphaBlending( );
	StaticObjectManager::getStaticObjectManager()->render();
	Global::renderStateDisableAlphaBlending( );
	Global::renderStateDisableAlphaTest( );

	Global::renderStateDisableZWriting( );
	Global::renderStateDisableZTest( );
}

void TestBoilerState::renderWorld2D( ) {
	Global::renderStateEnableAlphaBlending( );
	Global::renderStateEnableZTest( );

	ParticlesManager::getParticlesManager()->render();

	if( Global::enabledRenderTexture ) {
		// Render To Texture to do the air lava effect
		//Global::renderStateEnableAlphaBlending( );
		//if( !Global::isPushed( 'V' ) )
			lavaDistorsion.render( );
	}

	Global::renderStateEnableAdditiveAlphaBlending();
	Static2DObjectManager::getStatic2DObjectManager()->render();
	PlanetAtmosphereManager::getPlanetAtmosphereManager()->render();	

	Global::renderStateDisableZTest( );
	Global::renderStateDisableAlphaBlending( );
}

void TestBoilerState::renderPostProcessed2D( ) {
}

void TestBoilerState::renderGameUI( ) {
	Global::renderStateEnableAlphaBlending( );
/*
	uiNumberRender.print(0.6f, 0.9f, 0.04f, 0.04f, "9012345678 : \n%0.0f", Game::fps);
	timeOutNumberRender.printClosing(0.6f, 0.6f, 0.04f, 0.04f, "9012345678\n%0.0f", Game::fps);
	alphabetRender.print(0.02f, 0.1f, 0.04f, 0.04f, "abcdefghijklmnopqrstuvwxyz\nABCDEFGHIJKLMNOPQRSTUVWXYZ\n0123456789 %s\n+-!?,.=:", "Hola (adios) j.");
	xboxButtonsRender.print(0.5f, 0.3f, 0.1f, 0.1f, "abxyrlmns");
	padButtonsRender.print(0.5f, 0.05f, 0.1f, 0.1f, "0");
	padButtonsRender.print(0.5f, 0.1f, 0.1f, 0.1f, "1");
	padButtonsRender.print(0.5f, 0.15f, 0.1f, 0.1f, "2");
	padButtonsRender.print(0.5f, 0.2f, 0.1f, 0.1f, "3");
	padButtonsRender.print(0.5f, 0.25f, 0.1f, 0.1f, "4");
	padButtonsRender.print(0.5f, 0.3f, 0.1f, 0.1f, "5");
	padButtonsRender.print(0.5f, 0.35f, 0.1f, 0.1f, "6");
	padButtonsRender.print(0.5f, 0.4f, 0.1f, 0.1f, "7");
	padButtonsRender.print(0.5f, 0.45f, 0.1f, 0.1f, "8");
	padButtonsRender.print(0.5f, 0.5f, 0.1f, 0.1f, "9");
	padButtonsRender.print(0.5f, 0.55f, 0.1f, 0.1f, "10");
	padButtonsRender.print(0.5f, 0.6f, 0.1f, 0.1f, "11");
	padButtonsRender.print(0.5f, 0.65f, 0.1f, 0.1f, "12");
	padButtonsRender.print(0.5f, 0.7f, 0.1f, 0.1f, "13");
	padButtonsRender.print(0.5f, 0.75f, 0.1f, 0.1f, "14");
	padButtonsRender.print(0.5f, 0.8f, 0.1f, 0.1f, "15");
	padButtonsRender.print(0.5f, 0.85f, 0.1f, 0.1f, "16");

	*/
	Global::renderStateDisableAlphaBlending( );
}

void TestBoilerState::postRender( ) {
	// Print FPS
	Global::print(0, 0, 0xffff0000, "Frames per second %0.2f", Game::fps);

	Global::activeCamera3D( );
	SSPlayerCameraController * cameraController = (SSPlayerCameraController *) Global::camera->getCameraController();
	Global::print(0, 20, 0xffff0000, "Camera Distance %0.2f", cameraController->getDistance() );
	Global::print(0, 30, 0xffff0000, "Camera Position Speed %0.2f", cameraController->getPositionSpeed());
	Global::print(0, 40, 0xffff0000, "Camera Target Speed %0.2f", cameraController->getTargetSpeed());
	Global::activeCameraOrthogonal( );
}

void TestBoilerState::renderFrame( ) {
	Global::renderTextureQuad.render( );
}

void TestBoilerState::onLoad( ) {
	PlanetManager * planetManager = PlanetManager::getPlanetManager();
	planetManager->setCurrentPlanet( "boiler" );
	const Planet *planet = planetManager->getCurrentPlanet();
	const D3DXVECTOR3 &planetPosition = planet->getPosition();

	D3DXVECTOR3 cameraInitialPosition( planetPosition+D3DXVECTOR3(0.0f,0.0f,60.0f) );
	D3DXVECTOR3 cameraLookAt( planetPosition+D3DXVECTOR3(0.0f,0.0f,45.0f) );
	Global::camera->setUpVector( D3DXVECTOR3(0,1,0) );
	Global::camera->lookAt( cameraInitialPosition, cameraLookAt );
	Global::camera->loadSSPlayerCameraController();
	SSPlayerCameraController * cameraController = (SSPlayerCameraController *) Global::camera->getCameraController();
	cameraController->setDistance( 25.0f );
	cameraController->setPositionSpeed( 0.94f );
	cameraController->setTargetSpeed( 2.15f );

	Player *player = Player::getPlayer();
	player->setPosition( planetPosition+D3DXVECTOR3(0.0f,0.0f,45.0f) );

	firstUpdateSpace=true;
	timeoutUpdateSpace.restart();

	if( Game::configOptions.quality == "high" )
		Global::enabledRenderTexture = true;
	else
		Global::enabledRenderTexture = false;
}

void TestBoilerState::onUnload( ) {
}

void TestBoilerState::loadObjects( ) {
	// LAVA DISTORSION OBJECT
	D3DXMATRIX lavaDistorsiontransformMatrix;
	D3DXMatrixIdentity( &lavaDistorsiontransformMatrix );
	lavaDistorsion.setTransformMatrix( lavaDistorsiontransformMatrix );

	const Planet *boilerPlanet = PlanetManager::getPlanetManager()->getPlanet( "boiler" );
	lavaDistorsion.setPosition( boilerPlanet->getPosition( ) );

	IMesh *mesh = MeshFactory::getMeshFactory( )->createMesh( "lava_distorsion.mesh" );
	assert( mesh );
	lavaDistorsion.setMesh( mesh );

	lavaDistorsion.SETMATERIAL( Global::renderTexture.getTexture(), "shader.fx", "render_texture_tech" );

	// String Render
	uiNumberRender.create( );
	timeOutNumberRender.create( );
	alphabetRender.create( );
	xboxButtonsRender.create( );
	padButtonsRender.create( );
}

void TestBoilerState::reloadShader( ) {
	lavaDistorsion.reloadShader( );
	uiNumberRender.reloadShader( );
	timeOutNumberRender.reloadShader( );
	alphabetRender.reloadShader( );
	xboxButtonsRender.reloadShader( );
	padButtonsRender.reloadShader( );
}
