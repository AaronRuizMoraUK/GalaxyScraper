#include "IGameState.h"
#include "MenuState.h"
#include "CreditsState.h"
#include "OptionsState.h"
#include "IuranusToBoilerTransitionState.h"
#include "Iuranus/InitIuranusState.h"
#include "Iuranus/IuranusState.h"
#include "Iuranus/PauseIuranusState.h"
#include "Iuranus/LostIuranusState.h"
#include "Iuranus/EndIuranusState.h"
#include "Iuranus/EndGameIuranusState.h"
#include "Iuranus/EndIuranusToMenuState.h"
#include "Boiler/InitBoilerState.h"
#include "Boiler/BoilerState.h"
#include "Boiler/PauseBoilerState.h"
#include "Boiler/LostBoilerState.h"
#include "Boiler/EndBoilerState.h"
#include "Boiler/EndGameBoilerState.h"
#include "Boiler/EndBoilerToMenuState.h"
#include "Global/GlobalVariables.h"
#include "Factories/QuadFactory.h"
#include "Game/Game.h"
#include <cassert>

#include "Logic/PlanetLogic/IuranusPlanetLogic/IuranusPlanetLogic.h"
#include "TestROAState.h"
#include "TestBoilerState.h"
#include "TestBoilerPlanetState.h"
TestROAState testROAState;
TestBoilerState testBoilerState;
TestBoilerPlanetState testBoilerPlanetState;

IGameState * IGameState::currentState = NULL;

MenuState menuState;
CreditsState creditsState;
OptionsState optionsState;
IuranusToBoilerTransitionState iuranusToBoilerTransitionState;

InitIuranusState initIuranusState;
IuranusState iuranusState;
PauseIuranusState pauseIuranusState;
LostIuranusState lostIuranusState;
EndIuranusState endIuranusState;
EndGameIuranusState endGameIuranusState;
EndIuranusToMenuState endIuranusToMenuState;

InitBoilerState initBoilerState;
BoilerState boilerState;
PauseBoilerState pauseBoilerState;
LostBoilerState lostBoilerState;
EndBoilerState endBoilerState;
EndGameBoilerState endGameBoilerState;
EndBoilerToMenuState endBoilerToMenuState;

IGameState * IGameState::gameStates[IGameState::numStates] = {&menuState,
															  &creditsState,
															  &optionsState,
															  &iuranusToBoilerTransitionState,
															  &initIuranusState,
															  &iuranusState,
															  &pauseIuranusState,
															  &lostIuranusState,
															  &endIuranusState,
															  &endGameIuranusState,
															  &endIuranusToMenuState,
															  &initBoilerState,
															  &boilerState,
															  &pauseBoilerState,
															  &lostBoilerState,
															  &endBoilerState,
															  &endGameBoilerState,
															  &endBoilerToMenuState,
															  &testROAState,
															  &testBoilerState,
															  &testBoilerPlanetState,
															  NULL};

void IGameState::showLoadingScreen( ) {
	// ---------------------------------------------
	// Create loading screen object
	Object loadingSplash;

	D3DXMATRIX transformMatrix;
	D3DXMatrixIdentity(&transformMatrix);
	loadingSplash.setTransformMatrix(transformMatrix);

	if( Game::configOptions.aspectRatio != 4.0f/3.0f && Game::configOptions.aspectRatio != 5.0f/4.0f )
		loadingSplash.SETMATERIAL("loading_splash_pan.dds", "shader.fx", "dummy_tech");
	else
		loadingSplash.SETMATERIAL("loading_splash.dds", "shader.fx", "dummy_tech");

	IMesh * mesh = QuadFactory::getQuadFactory()->createQuad(1.0f, 1.0f);
	assert(mesh);
	loadingSplash.setMesh(mesh);

	// ---------------------------------------------
	// Begin Render Frame

	// Clear the backbuffer and z-buffer to a black color
	Global::device->Clear( 0, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER|D3DCLEAR_STENCIL, D3DCOLOR_XRGB(0,0,0), 1.0f, 0 );

	// Begin the scene
	Global::device->BeginScene();

	// ---------------------------------------------
	// Render Frame

	Global::activeCameraOrthogonal( );

	loadingSplash.render( );

	// ---------------------------------------------
	// End Render Frame

	// End the scene
	Global::device->EndScene();

	// Present the backbuffer contents to the display
	Global::device->Present( NULL, NULL, NULL, NULL );
}

void IGameState::render( ) {
	renderTimeStart = timeGetTime( ); // START

	if( Global::enabledRenderTexture ) {
		// Render to Texture
		beginRenderTexture( );
			preRender( );

			renderWorld3D( ); 
			renderWorld2D( );

			Global::activeCameraOrthogonal( );

			renderPostProcessed2D( );
			renderGameUI( );

			postRender( );
		endRenderTexture( );

		// Take a screenshot
		static int screenshotNumber = 0;
		if( Global::takeScreenshot ) {
			Global::takeScreenshot = false;

			char msg[20];
			sprintf_s( msg, sizeof( msg ), "screenshot_%02d", screenshotNumber++ );
			Global::renderTexture.save( msg );
		}

		// Render texture to screen
		beginRenderFrame( );
			renderFrame( );
		endRenderFrame( );

		Global::activeCamera3D( );
	}
	else {
		beginRenderFrame( );
			preRender( );

			renderWorld3D( ); 
			renderWorld2D( );

			Global::activeCameraOrthogonal( );

			renderPostProcessed2D( );
			renderGameUI( );

			postRender( );

			if( Global::showIuranusPlanetLogicVariables ) {
				IuranusPlanetLogic *iuranusPlanetLogic = IuranusPlanetLogic::getIuranusPlanetLogic();
				Global::print(0, 160, 0xffff0000, "numBirthPoints = %d", iuranusPlanetLogic->numBirthPoints);
				Global::print(0, 170, 0xffff0000, "numEnemiesEatenForVictory = %d", iuranusPlanetLogic->numEnemiesEatenForVictory);
				Global::print(0, 180, 0xffff0000, "totalEnemiesInScreen = %d", iuranusPlanetLogic->totalEnemiesInScreen);
				Global::print(0, 190, 0xffff0000, "initialEnemiesInScreen = %d", iuranusPlanetLogic->initialEnemiesInScreen);
				Global::print(0, 200, 0xffff0000, "intervalEnemiesCreation = %f", iuranusPlanetLogic->intervalEnemiesCreation);
				Global::print(0, 210, 0xffff0000, "checkPointPercent1 = %d", iuranusPlanetLogic->checkPointPercent1);
				Global::print(0, 220, 0xffff0000, "checkPointPercent2 = %d", iuranusPlanetLogic->checkPointPercent2);
				Global::print(0, 230, 0xffff0000, "checkPointPercent3 = %d", iuranusPlanetLogic->checkPointPercent3);
				Global::print(0, 240, 0xffff0000, "failVictoryCheckPointRecovery = %f", iuranusPlanetLogic->failVictoryCheckPointRecovery);
				Global::print(0, 250, 0xffff0000, "enemiesInTheScreen = %d", iuranusPlanetLogic->enemiesInTheScreen);
				Global::print(0, 260, 0xffff0000, "numEnemiesEaten = %d", iuranusPlanetLogic->numEnemiesEaten);
				Global::print(0, 270, 0xffff0000, "checkPointActived = %s", iuranusPlanetLogic->checkPointActived?"true":"false");
				Global::print(0, 280, 0xffff0000, "victoryActived = %s", iuranusPlanetLogic->victoryActived?"true":"false");
				Global::print(0, 290, 0xffff0000, "alreadyWon = %s", iuranusPlanetLogic->alreadyWon?"true":"false");
			}
		endRenderFrame( );

		Global::activeCamera3D( );
	}

	renderTimeEnd = timeGetTime( ); // END
	Global::renderTime = ((float)(renderTimeEnd-renderTimeStart))*0.001f;
}

void IGameState::beginRenderFrame() {
	// Clear the backbuffer and z-buffer to a black color
	Global::device->Clear( 0, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER|D3DCLEAR_STENCIL, D3DCOLOR_XRGB(0,0,0), 1.0f, 0 );

	// Begin the scene
	Global::device->BeginScene();
}

void IGameState::endRenderFrame() {
	// End the scene
	Global::device->EndScene();

	// Present the backbuffer contents to the display
	Global::device->Present( NULL, NULL, NULL, NULL );
}

void IGameState::beginRenderTexture() {
	Global::renderTexture.begin( );

	// Clear the backbuffer and z-buffer to a black color
	Global::device->Clear( 0, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER|D3DCLEAR_STENCIL, D3DCOLOR_XRGB(0,0,0), 1.0f, 0 );
}

void IGameState::endRenderTexture() {
	Global::renderTexture.end( );
}

void IGameState::getFocus( ) {
	if( currentState )
		currentState->onUnload();

	currentState = this;
	currentState->onLoad();
}

IGameState * IGameState::getState( const std::string &name ) {
	for(int i=0; i<numStates; ++i) {
		IGameState * gameState = gameStates[i];
		assert(gameState);
		const std::string &gs_name = gameState->getName();

		if( name == gs_name )
			return gameState;
	}

	assert(!"State not exist");
	return NULL;
}

bool IGameState::isVisible(const Object &object) {
	const IMesh * mesh = object.getMesh();
	D3DXVECTOR3 center = mesh->getBoundingSphereCenter();
	float radius = mesh->getBoundingSphereRadius();

	const D3DXVECTOR3 centerInstance = center + object.getPosition();
	if( Global::frustum.isBSphereVisible(centerInstance, radius) ) 
		return true;
	else
		return false;
}

void IGameState::load() {
	for( int i=0; i<numStates && gameStates[i] != NULL; ++i )
		gameStates[i]->loadObjects( );
}

void IGameState::reloadShaders() {	
	for( int i=0; i<numStates && gameStates[i] != NULL; ++i )
		gameStates[i]->reloadShader( );
}
