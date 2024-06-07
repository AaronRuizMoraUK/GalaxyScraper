#include "BoilerState.h"
#include "Global/GlobalVariables.h"
#include "Source/Game.h"
#include "Managers/PlanetManager.h"
#include "Managers/PlanetAtmosphereManager.h"
#include "Managers/ParticlesManager.h"
#include "Managers/StaticObjectManager.h"
#include "Managers/Static2DObjectManager.h"
#include "CameraControllers/SSPlayer/SSPlayerCameraController.h"
#include "Object/CollisionableObject/AnimatedObject/Player/Player.h"
#include "Logic/PlanetLogic/IuranusPlanetLogic/IuranusPlanetLogic.h"
#include "PlayerControllers/Sphere/SpherePlayerController.h"
#include "Managers/LogicManager/LogicManager.h"
#include "Managers/BoilerManagers/BoilerFixedParticlesManager.h"
#include "Managers/BoilerManagers/FirePlatformsManager.h"
#include "Managers/BoilerManagers/BoilerAnimatedObjectManager.h"
#include "Managers/BoilerManagers/BoilerFirebalLManager.h"
#include "Managers/BoilerManagers/LeversManager.h"
#include "Managers/CollisionManager/BoilerCollisionManager.h"
#include "Managers/IuranusManagers/IuranusShadowManager.h"
#include "Managers/PlanetManager.h"
#include "Managers/BoilerManagers/BoilerStaticCollionsManager.h"
#include "Managers/BoilerManagers/CanonsManager.h"
#include "Managers/BoilerManagers/BoilerEnemyManager.h"
#include "Managers/BoilerManagers/BoilerFireballManager.h"

std::string BoilerState::name = "boiler_state";

const float BoilerState::boilerCameraDistance = 26.0f;
const float BoilerState::boilerCameraPositionSpeed = 2.34f;
const float BoilerState::boilerCameraTargetSpeed = 3.35f;

const float BoilerState::cameraVibrationTime = 0.3f;
float BoilerState::cameraVibrationCurrentTime = 0.0f;
bool BoilerState::doAnyVibration = false;

BoilerState::BoilerState( )
: IGameState()
, flickeringTempus(1.0f)
, flickeringTime(0.0f)
, doOnLoad(true)
, die( true )
, drawAtmospheres( true )
{
}

void BoilerState::update( float deltaTime ) {

	// Control camera vibration in transition from boiler to iuranus
	// cameraVibrationCurrentTime only must be set to 0.0f in initBoilerState
	cameraVibrationCurrentTime += deltaTime;
	if( doAnyVibration && cameraVibrationCurrentTime > cameraVibrationTime ) {
		doAnyVibration = false;
		Global::cameraVibrationEnabled = false;
	}
	

	// TODO Player falls down to lava condition
	const Planet *currentPlanet = PlanetManager::getPlanetManager()->getCurrentPlanet();
	const D3DXVECTOR3 &planetCenter = currentPlanet->getPosition();
	Player *player = Player::getPlayer( );
	float playerToCenterDistance = Global::distanceSqrt( player->getPosition( ), planetCenter );
	if( Global::isPushed( 'Q' ) )
		die = !die;
	if( playerToCenterDistance < 23.0f && !Global::alreadyFallen ) 
	{
		if( die )
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
	BoilerEnemyManager::getBoilerEnemyManager()->updateAI(deltaTime);

	updateAnimationTimeStart = updateAITimeEnd = timeGetTime(); // END

	// Update Animation
	BoilerAnimatedObjectManager::getBoilerAnimatedObjectManager()->updateAnimation( deltaTime );
	BoilerFireballManager::getBoilerFireballManager()->updateAnimation(deltaTime);
	BoilerEnemyManager::getBoilerEnemyManager()->updateAnimation(deltaTime);
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
	if ( Global::youWin ) {
		// AQUI CUANDO GANEMOS, ESTO HACERLO MAÑANA CUANDO PENSEMOS QUÉ HACEMOS DE FINAL
		getState("end_boiler_state")->getFocus();
		return;
	}
	// If player lose restart game in boiler planet
	// ESTO ES PERDER UN CONTINUE, hay que activar youLose
	else if( Global::youLose ) {
		// Quitamos el control al player y ponemos youLose a false para la proxima vez
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

void BoilerState::preRender( ) {
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

void BoilerState::renderWorld3D( ) {
	Global::renderStateEnableZTest( );
	Global::renderStateEnableZWriting( );

	renderPlayerTimeStart = timeGetTime(); // START

	// Draw player
	if( !Global::playerFlickering || mustRenderPlayer() )
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
	BoilerEnemyManager::getBoilerEnemyManager()->render();
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

void BoilerState::renderWorld2D( ) {
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

	if( drawAtmospheres )
		PlanetAtmosphereManager::getPlanetAtmosphereManager()->render();

	renderAtmospheresTimeEnd = timeGetTime(); // END

	Global::renderStateDisableZTest( );
	Global::renderStateDisableAlphaBlending( );
}

void BoilerState::renderPostProcessed2D( ) {
}

void BoilerState::renderGameUI( ) {
	Global::renderStateEnableAlphaBlending();

	renderUITimeStart = timeGetTime(); // START
	boilerUI.render( );
	renderUITimeEnd = timeGetTime(); // END

	Global::renderStateDisableAlphaBlending();
}

void BoilerState::postRender( ) {
	// Print FPS
	Global::print(0, 100, 0xffff0000, "Frames per second %0.2f", Game::fps);
	Global::print(0, 110, 0xffff0000, "Update Time %f", ((float)(updateTimeEnd-updateTimeStart))*0.001f);
	Global::print(0, 120, 0xffff0000, "Render Time %f", Global::renderTime);
	Global::print(0, 130, 0xffff0000, "'G' Render Texture %s", Global::enabledRenderTexture?"YES":"NO");
	Global::print(0, 140, 0xffff0000, "'Q' Player Die %s", die?"YES":"NO");
	Global::print(0, 150, 0xffff0000, "'T' show/hide statistics");

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

void BoilerState::renderFrame( ) {
	Global::renderTextureQuad.render( );
}

void BoilerState::onLoad( ) {

	if( doOnLoad ) {
		// SOLO SE HACE DESDE INIT_BOILER_STATE AQUI NO HACER NADA
		// EL QUE MANDA ES EL LUA boiler_init
		PlanetManager * planetManager = PlanetManager::getPlanetManager();
		const Planet *planet = planetManager->getCurrentPlanet();
		const D3DXVECTOR3 &planetPosition = planet->getPosition();

		D3DXVECTOR3 cameraInitialPosition( planetPosition+D3DXVECTOR3(0.0f,0.0f,60.0f) );
		Global::camera->setUpVector( D3DXVECTOR3(0,1,0) );
		Global::camera->lookAt( Global::camera->getPosition(), planetPosition );
		Global::camera->loadSSPlayerCameraController();
		SSPlayerCameraController * cameraController = (SSPlayerCameraController *) Global::camera->getCameraController();
		cameraController->setDistance( boilerCameraDistance );
		cameraController->setPositionSpeed( boilerCameraPositionSpeed ); 
		cameraController->setTargetSpeed( boilerCameraTargetSpeed );

		Player *player = Player::getPlayer();
		SpherePlayerController *player_controller = new SpherePlayerController( D3DX_PI/2.0f );
		player->setPlayerController( player_controller );
		player->setRelationToState(CYCLE, "idle", true, true);

		// Correct alpha and delta
		const D3DXVECTOR3 &playerPos = player->getPosition();
		float alpha, delta;
		alpha = delta = 0.0f;
		planet->getPolarAngles( playerPos, alpha, delta );
		player->setAlpha( alpha );
		player->setDelta( delta );

		Global::alreadyFallen = false;
		Global::playerFlickering = false;

		BoilerFireballManager::getBoilerFireballManager()->renewParticles();

		LogicManager::getLogicManager()->createUserEvent("boiler_init");
	}
	else
		doOnLoad = true;
}

void BoilerState::onUnload( ) {
	// AQUI NADA
}

void BoilerState::loadObjects( ) {
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

void BoilerState::reloadShader( ) {
	lavaDistorsion.reloadShader( );
	boilerUI.reloadShader( );
}

bool BoilerState::mustRenderPlayer()
{
	return (flickeringTempus > 0.0f);
}

void BoilerState::updateFlickering(float deltaTime)
{
	flickeringTime += deltaTime * 20;
	flickeringTempus = sin( flickeringTime );
}





// ONLOAD ANTIGUO
/*
// Set the camera for playing in Boiler planet
Global::camera->setUpVector( D3DXVECTOR3(0,1,0) );
Global::camera->loadSSPlayerCameraController();
SSPlayerCameraController * cameraController = (SSPlayerCameraController *) Global::camera->getCameraController();
cameraController->setDistance( cameraDistanceNomal );
cameraController->setPositionSpeed( cameraPositionSpeedNomal );
cameraController->setTargetSpeed( cameraTargetSpeedNomal );

SpherePlayerController *player_controller = new SpherePlayerController( D3DX_PI/2.0f );
Player::getPlayer()->setPlayerController( player_controller );

// TODO Restart the planet

// Reset variables for spatial partitioner
firstUpdateSpace=true;
timeoutUpdateSpace.restart();
*/

/*
// ANTIGUO
void BoilerState::update( float deltaTime ) {
	// TODO Player falls down to lava condition
	const Planet *currentPlanet = PlanetManager::getPlanetManager()->getCurrentPlanet();
	const D3DXVECTOR3 &planetCenter = currentPlanet->getPosition();
	Player *player = Player::getPlayer( );
	float playerToCenterDistance = Global::distanceSqrt( player->getPosition( ), planetCenter );
	if( playerToCenterDistance < 23.0f ) {
		Global::youLose = true;
		Global::takePlayerControlOut = true;
	}

	// Update camera
	Global::camera->update(deltaTime);

	// Update frustum
	Global::frustum.update();
	Global::backSideFrustum.update();

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

	// Update Planet Atmospheres positions
	PlanetAtmosphereManager::getPlanetAtmosphereManager()->update();

	// Update BOILER Particles
	ParticlesManager::getParticlesManager()->update(deltaTime);
	// TODO Faltan las fixed de boiler

	// Update Space
	PolarAnglesPartitioner *polarAnglesPartitioner = PolarAnglesPartitioner::getPolarAnglesPartitioner();
	timeoutUpdateSpace.update(deltaTime);
	if(timeoutUpdateSpace.isFinished() || firstUpdateSpace) {
		firstUpdateSpace=false;
		timeoutUpdateSpace.restart();

		// Update space
		polarAnglesPartitioner->updateSpace();
	}

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
*/