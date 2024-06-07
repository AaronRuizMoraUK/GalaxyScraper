#include <Windows.h>
#include "TestBoilerPlanetState.h"
#include "Global/GlobalFunctions.h"
#include "Global/GlobalVariables.h"
#include "Source/Game.h"
#include "Object/CollisionableObject/AnimatedObject/Player/Player.h"
#include "Managers/PlanetAtmosphereManager.h"
#include "Managers/ParticlesManager.h"
#include "Managers/BoilerManagers/BoilerFixedParticlesManager.h"
#include "Managers/CollisionManager/BoilerCollisionManager.h"
#include "Managers/PlanetManager.h"
#include "Managers/StaticObjectManager.h"
#include "Managers/Static2DObjectManager.h"
#include "CameraControllers/SSPlayer/SSPlayerCameraController.h"
#include "Managers/BoilerManagers/BoilerFirebalLManager.h"
#include "Managers/BoilerManagers/FirePlatformsManager.h"
#include "Managers/BoilerManagers/LeversManager.h"
#include "Managers/BoilerManagers/CanonsManager.h"
// #include "Managers/BoilerManagers/CanonsManager.h"
#include "Managers/LogicManager/LogicManager.h"
#include "Managers/IuranusManagers/IuranusShadowManager.h"
#include "Managers/BoilerManagers/BoilerAnimatedObjectManager.h"
#include "Managers/BoilerManagers/BoilerStaticCollionsManager.h"

std::string TestBoilerPlanetState::name = "test_boiler_planet_state";
const float TestBoilerPlanetState::boilerCameraDistance = 26.0f;
const float TestBoilerPlanetState::boilerCameraPositionSpeed = 2.34f;
const float TestBoilerPlanetState::boilerCameraTargetSpeed = 3.35f;


TestBoilerPlanetState::TestBoilerPlanetState( )
: IGameState()
, flickeringTempus(1.0f)
, flickeringTime(0.0f)
{
}

TestBoilerPlanetState::~TestBoilerPlanetState()
{
	OutputDebugString("Destroying TestBoilerPlanetState...\n");
};

void TestBoilerPlanetState::update( float deltaTime ) {

	// TODO Player falls down to lava condition
	const Planet *currentPlanet = PlanetManager::getPlanetManager()->getCurrentPlanet();
	const D3DXVECTOR3 &planetCenter = currentPlanet->getPosition();
	Player *player = Player::getPlayer( );
	float playerToCenterDistance = Global::distanceSqrt( player->getPosition( ), planetCenter );
	if( playerToCenterDistance < 23.0f && !Global::alreadyFallen ) 
	{
		LogicManager::getLogicManager()->createUserEvent("on_player_fired");
	}

	updateDeltaTime = deltaTime;

	updateTimeStart = updateCameraFrustumTimeStart = timeGetTime(); // START

	// Update camera 
	Global::camera->update(deltaTime);

	// Update frustum
	Global::frustum.update();
	Global::backSideFrustum.update();

	updatePlayerTimeStart = updateCameraFrustumTimeEnd = timeGetTime(); // END

	if( !Global::takePlayerControlOut && !Global::takePlayerControlOutUpdatingAnimation )
	{
		// Update Player
		player->refreshPreviousTransformMatrix();
		player->update(deltaTime);
	}

	if(Global::animationEnabled) {
		// Update Animation of player
		if( !Global::takePlayerControlOut )
			player->updateAnimation(deltaTime);
	}
	
	updateAITimeStart = updatePlayerTimeEnd = timeGetTime(); // END
	
	// Update AI
	BoilerFireballManager::getBoilerFireballManager()->updateAI(deltaTime);
	FirePlatformsManager::getFirePlatformsManager()->updateAI(deltaTime);

	updateAnimationTimeStart = updateAITimeEnd = timeGetTime(); // END

	// Update Animation
	BoilerAnimatedObjectManager::getBoilerAnimatedObjectManager()->updateAnimation( deltaTime );
	BoilerFireballManager::getBoilerFireballManager()->updateAnimation(deltaTime);
	LeversManager::getLeversManager()->updateAnimation(deltaTime);
	CanonsManager::getCanonsManager()->updateAnimation(deltaTime);

	updateAtmosphereParticlesTimeStart = updateAnimationTimeEnd = timeGetTime(); // END

	// Update Planet Atmospheres positions
	PlanetAtmosphereManager::getPlanetAtmosphereManager()->update();

	// Update Particles
	ParticlesManager::getParticlesManager()->update(deltaTime);
	BoilerFixedParticlesManager::getBoilerFixedParticlesManager()->update(deltaTime);

	// Update player flickering if needed
	if( Global::playerFlickering )
		updateFlickering(deltaTime);

	updateCollisionTimeStart = updateAtmosphereParticlesTimeEnd = timeGetTime(); // END

	if(Global::collisionEnabled) {
		// Detect Collisions
		BoilerCollisionManager::getBoilerCollisionManager()->detectCollisions();

		// Treat Collisions
		BoilerCollisionManager::getBoilerCollisionManager()->treatCollisions();
	}

	updateLogicTimeStart = updateCollisionTimeEnd = timeGetTime(); // END

	// Update LogicManager
	LogicManager::getLogicManager()->update(deltaTime);

	updateTimeEnd = updateLogicTimeEnd = timeGetTime(); // END

	// Update UI
	boilerUI.update( deltaTime );

	// --------------------------------------------------------------
	// CHECK CHANGE GAME STATE

	// If player wins go to end of game
	// TODO Por ahora vuelve a menu
	if ( Global::youWin ) {
		getState("end_boiler_state")->getFocus();
		return;
	}
	// If player lose restart game in iuranus planet
	else if( Global::youLose ) {
		// TODO Restart Boiler Planet (include player in initial point)
		Global::youLose = false;
		Global::takePlayerControlOut = false;

		getState("lost_boiler_state")->getFocus();
		return;
	}
	// If start was pressed go to pause boiler state
	else if ( Global::ioStatus.start.becomesPressed() ) {
		getState("pause_boiler_state")->getFocus();
		return;
	}

}

void TestBoilerPlanetState::preRender( ) {
}

void TestBoilerPlanetState::renderWorld3D( ) {
	Global::renderStateEnableZTest( );
	Global::renderStateEnableZWriting( );

	renderPlayerTimeStart = timeGetTime(); // START

	// Draw player
	if( Global::playerFlickering && mustRenderPlayer() )
		Player::getPlayer()->render();
	else if( !Global::playerFlickering )
		Player::getPlayer()->render();

	renderPlanetsTimeStart = renderPlayerTimeEnd = timeGetTime(); // END

	// Draw Scene
	PlanetManager::getPlanetManager()->render();
	
	renderLeversTimeStart = renderPlanetsTimeEnd = timeGetTime(); // END
	BoilerAnimatedObjectManager::getBoilerAnimatedObjectManager()->render( );
	CanonsManager::getCanonsManager()->render();
	LeversManager::getLeversManager()->render();
	renderFireballTimeStart = renderLeversTimeEnd = timeGetTime(); // END
	BoilerFireballManager::getBoilerFireballManager()->render();
	renderStaticObjectsTimeStart = renderFireballTimeEnd = timeGetTime(); // END

	Global::renderStateEnableAlphaTest( );
	Global::renderStateEnableAlphaBlending( );
	StaticObjectManager::getStaticObjectManager()->render();
	Global::renderStateDisableAlphaBlending( );
	Global::renderStateDisableAlphaTest( );

	renderStaticObjectsTimeEnd = timeGetTime(); // END

	BoilerStaticCollisionsManager::getBoilerStaticCollisionsManager()->render();

	Global::renderStateDisableZWriting( );
	Global::renderStateDisableZTest( );
}

void TestBoilerPlanetState::renderWorld2D( ) {
	Global::renderStateEnableAlphaBlending( );
	Global::renderStateEnableZTest( );

	// Draw Shadows
	Global::renderStateEnableAlphaTest( );
	IuranusShadowManager::getIuranusShadowManager()->render();
	Global::renderStateDisableAlphaTest( );

	renderParticlesTimeStart = timeGetTime(); // START

	ParticlesManager::getParticlesManager()->render();
	BoilerFixedParticlesManager::getBoilerFixedParticlesManager()->render();

	renderLavaDistorsionTimeStart = renderParticlesTimeEnd = timeGetTime(); // END

	if( Global::enabledRenderTexture ) {
		// Render To Texture to do the air lava effect
		//Global::renderStateEnableAlphaBlending( );
		lavaDistorsion.render( );
	}

	renderAtmospheresTimeStart = renderLavaDistorsionTimeEnd = timeGetTime(); // END

	Global::renderStateEnableAdditiveAlphaBlending();
	Static2DObjectManager::getStatic2DObjectManager()->render();
	PlanetAtmosphereManager::getPlanetAtmosphereManager()->render();

	renderAtmospheresTimeEnd = timeGetTime(); // END

	Global::renderStateDisableZTest( );
	Global::renderStateDisableAlphaBlending( );
}

void TestBoilerPlanetState::renderPostProcessed2D( ) {
}

void TestBoilerPlanetState::renderGameUI( ) {
	Global::renderStateEnableAlphaBlending();
	renderUITimeStart = timeGetTime(); // START

	boilerUI.render( );

	renderUITimeEnd = timeGetTime(); // END
	Global::renderStateDisableAlphaBlending();
}

void TestBoilerPlanetState::postRender( ) {
	
	Global::activeCamera3D( );
	SSPlayerCameraController * cameraController = (SSPlayerCameraController *) Global::camera->getCameraController();
	float cameraDistance = cameraController->getDistance( );
	float cameraPositionSpeed = cameraController->getPositionSpeed( );
	float cameraTargetSpeed = cameraController->getTargetSpeed( );

	/*
	if( Global::isPushed( 'U' ) ) {
		cameraDistance += 0.1f;
	}
	else if( Global::isPushed( 'J' ) && cameraDistance>0.0f ) {
		cameraDistance -= 0.1f;
	}

	if( Global::isPushed( 'I' ) ) {
		cameraPositionSpeed += 0.1f;
	}
	else if( Global::isPushed( 'K' ) && cameraPositionSpeed>0.0f ) {
		cameraPositionSpeed -= 0.1f;
	}

	if( Global::isPushed( 'O' ) ) {
		cameraTargetSpeed += 0.1f;
	}
	else if( Global::isPushed( 'L' ) && cameraTargetSpeed>0.0f ) {
		cameraTargetSpeed -= 0.1f;
	}
	*/

	cameraController->setDistance( cameraDistance );
	cameraController->setPositionSpeed( cameraPositionSpeed );
	cameraController->setTargetSpeed( cameraTargetSpeed );

	Global::print(0, 20, 0xffff0000, "Distance %0.2f", cameraController->getDistance());
	Global::print(0, 30, 0xffff0000, "Position Speed %0.2f", cameraController->getPositionSpeed());
	Global::print(0, 40, 0xffff0000, "Position Speed %0.2f", cameraController->getTargetSpeed());
	Global::activeCameraOrthogonal( );

	// -----------------------------------------------------------------------------------------------

	// Print FPS
	Global::print(0, 100, 0xffff0000, "Frames per second %0.2f", Game::fps);
	Global::print(0, 110, 0xffff0000, "Update Time %f", ((float)(updateTimeEnd-updateTimeStart))*0.001f);
	Global::print(0, 120, 0xffff0000, "Render Time %f", Global::renderTime);
	Global::print(0, 130, 0xffff0000, "'G' Render Texture %s", Global::enabledRenderTexture?"YES":"NO");
	Global::print(0, 140, 0xffff0000, "'T' show/hide statistics");

	if( Global::showStatistic ) {
		Global::print(0, 160, 0xffffffff, "Cycle %s", Player::getPlayer()->getCurrentCycle().c_str());
		Global::print(0, 170, 0xffffffff, "Action %s", Player::getPlayer()->getCurrentAction().c_str());
		Global::print(0, 180, 0xffffffff, "(z) Collisions   %s", Global::collisionEnabled?"enabled":"disabled");
		Global::print(0, 190, 0xffffffff, "(Z) IA              %s", Global::IAEnabled?"enabled":"disabled");
		Global::print(0, 200, 0xffffffff, "(x) Animations %s", Global::animationEnabled?"enabled":"disabled");
		Global::print(0, 210, 0xffffffff, "(X) Logic         %s", Global::logicEnabled?"enabled":"disabled");
						 
		Global::print(0, 230, 0xffffffff, "Update Time (Camera-Frustum) %f", ((float)(updateCameraFrustumTimeEnd-updateCameraFrustumTimeStart))*0.001f );
		Global::print(0, 240, 0xffffffff, "Update Time (Player) %f", ((float)(updatePlayerTimeEnd-updatePlayerTimeStart))*0.001f );
		Global::print(0, 250, 0xffffffff, "Update Time (AI) %f", ((float)(updateAITimeEnd-updateAITimeStart))*0.001f );
		Global::print(0, 260, 0xffffffff, "Update Time (Animation) %f", ((float)(updateAnimationTimeEnd-updateAnimationTimeStart))*0.001f );
		Global::print(0, 270, 0xffffffff, "Update Time (Atmosphere-Particles) %f", ((float)(updateAtmosphereParticlesTimeEnd-updateAtmosphereParticlesTimeStart))*0.001f );
		Global::print(0, 280, 0xffffffff, "Update Time (Collision) %f", ((float)(updateCollisionTimeEnd-updateCollisionTimeStart))*0.001f );
		Global::print(0, 290, 0xffffffff, "Update Time (Logic) %f", ((float)(updateLogicTimeEnd-updateLogicTimeStart))*0.001f );
						 
		Global::print(0, 310, 0xffffffff, "Render Time (Player) %f", ((float)(renderPlayerTimeEnd-renderPlayerTimeStart))*0.001f );
		Global::print(0, 320, 0xffffffff, "Render Time (Planets) %f", ((float)(renderPlanetsTimeEnd-renderPlanetsTimeStart))*0.001f );
		Global::print(0, 330, 0xffffffff, "Render Time (Levers) %f", ((float)(renderLeversTimeEnd-renderLeversTimeStart))*0.001f );
		Global::print(0, 340, 0xffffffff, "Render Time (Fireball) %f", ((float)(renderFireballTimeEnd-renderFireballTimeStart))*0.001f );
		Global::print(0, 350, 0xffffffff, "Render Time (StaticObjects) %f", ((float)(renderStaticObjectsTimeEnd-renderStaticObjectsTimeStart))*0.001f );
		Global::print(0, 360, 0xffffffff, "Render Time (Particles) %f", ((float)(renderParticlesTimeEnd-renderParticlesTimeStart))*0.001f );
		Global::print(0, 370, 0xffffffff, "Render Time (Lava Distorsion) %f", ((float)(renderLavaDistorsionTimeEnd-renderLavaDistorsionTimeStart))*0.001f );
		Global::print(0, 380, 0xffffffff, "Render Time (Atmospheres) %f", ((float)(renderAtmospheresTimeEnd-renderAtmospheresTimeStart))*0.001f );
		Global::print(0, 390, 0xffffffff, "Render Time (UI) %f", ((float)(renderUITimeEnd-renderUITimeStart))*0.001f );

		Global::print(0, 410, 0xffffffff, "Delta  Time %f - Frames By Delta Time %f", updateDeltaTime, 1.0f/updateDeltaTime );
	}
}

void TestBoilerPlanetState::renderFrame( ) {
	Global::renderTextureQuad.render( );
}

void TestBoilerPlanetState::onLoad( ) {
	PlanetManager * planetManager = PlanetManager::getPlanetManager();
	planetManager->setCurrentPlanet( "boiler" );
	const Planet *planet = planetManager->getCurrentPlanet();
	const D3DXVECTOR3 &planetPosition = planet->getPosition();

	D3DXVECTOR3 cameraInitialPosition( planetPosition+D3DXVECTOR3(0.0f,0.0f,60.0f) );
	D3DXVECTOR3 cameraLookAt( planetPosition+D3DXVECTOR3(0.0f,0.0f,33.0f) );
	Global::camera->setUpVector( D3DXVECTOR3(0,1,0) );
	Global::camera->lookAt( cameraInitialPosition, cameraLookAt );
	Global::camera->loadSSPlayerCameraController();
	SSPlayerCameraController * cameraController = (SSPlayerCameraController *) Global::camera->getCameraController();
	cameraController->setDistance( TestBoilerPlanetState::boilerCameraDistance );
	cameraController->setPositionSpeed( TestBoilerPlanetState::boilerCameraPositionSpeed ); 
	cameraController->setTargetSpeed( TestBoilerPlanetState::boilerCameraTargetSpeed );

	Player *player = Player::getPlayer();
	player->setPosition( planetPosition+D3DXVECTOR3(0.0f,0.0f,45.0f) );

	if( Game::configOptions.quality == "high" )
		Global::enabledRenderTexture = RenderToSurfaceSupported;
	else
		Global::enabledRenderTexture = false;

	Global::alreadyFallen = false;
	Global::playerFlickering = false;
	LogicManager::getLogicManager()->createUserEvent("boiler_init");
}

void TestBoilerPlanetState::onUnload( ) {
}

void TestBoilerPlanetState::loadObjects( ) {
	const IMesh *lavaDistorsionMesh = lavaDistorsion.getMesh();

	if( lavaDistorsionMesh == NULL ) {
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
	}
	else {
		assert( !"This must not happend in BoilerState" );
		return;
	}

	boilerUI.create( );
}

void TestBoilerPlanetState::reloadShader( ) 
{
	lavaDistorsion.reloadShader( );	
	boilerUI.reloadShader( );
}

bool TestBoilerPlanetState::mustRenderPlayer()
{
	return (flickeringTempus > 0.0f);
}

void TestBoilerPlanetState::updateFlickering(float deltaTime)
{
	flickeringTime += deltaTime * 20;
	flickeringTempus = sin( flickeringTime );
}