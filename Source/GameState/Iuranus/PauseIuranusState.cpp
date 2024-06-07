#include "PauseIuranusState.h"
#include "IuranusState.h"
#include "Global/GlobalFunctions.h"
#include "Global/GlobalVariables.h"
#include "Game/Game.h"
#include "Managers/PlanetManager.h"
#include "Managers/IuranusManagers/IuranusEnemyManager.h"
#include "Managers/IuranusManagers/IuranusPlanetElementsManager.h"
#include "Managers/StaticObjectManager.h"
#include "Managers/Static2DObjectManager.h"
#include "Managers/IuranusManagers/IuranusAnimatedObjectManager.h"
#include "Managers/ParticlesManager.h"
#include "Managers/IuranusManagers/IuranusFixedParticlesManager.h"
#include "Managers/PlanetAtmosphereManager.h"
#include "Managers/IuranusManagers/IuranusShadowManager.h"
#include "Object/CollisionableObject/AnimatedObject/Player/Player.h"
#include "Factories/QuadFactory.h"

std::string PauseIuranusState::name = "pause_iuranus_state";

const D3DXVECTOR3 PauseIuranusState::onContinuePosition( -0.1f, 0.0f, 0.0f );
D3DXVECTOR3 PauseIuranusState::onBackToMenuPosition( -0.1f, -0.2f, 0.0f );

void PauseIuranusState::update( float deltaTime ) {

	if ( enter_pause ) {
		if( !Global::fade.isFinished( ) )
			Global::fade.update( deltaTime );
		else
			enter_pause = false;
	}
	else if( exit_pause ) {
		if( !Global::fade.isFinished( ) ) {
			Global::fade.update( deltaTime );
		}
		else if ( onContinue ) {
			// Go to iuranus state, but telling it that not make its onLoad
			IuranusState * iuranusState = (IuranusState *) getState( "iuranus_state" );
			iuranusState->doOnLoad = false;
			iuranusState->getFocus( );
		}
		else {
			getState( "end_iuranus_to_menu_state" )->getFocus( );
		}
	}
	else {

		if( Global::ioStatus.start.becomesPressed( ) 
			|| Global::ioStatus.selectOption.becomesPressed() ) {
			exit_pause = true;
		}
		else if (  Global::ioStatus.walk.becomesPressedUp() ) {
			onContinue = true;
			arrow.setPosition( onContinuePosition );
		}
		else if (  Global::ioStatus.walk.becomesPressedDown() ) {
			onContinue = false;
			arrow.setPosition( onBackToMenuPosition );
		}
	}
}

void PauseIuranusState::preRender( ) {
}

void PauseIuranusState::renderWorld3D( ) {
	Global::renderStateEnableZTest( );
	Global::renderStateEnableZWriting( );

	// Draw player
	Player::getPlayer()->render();

	// Draw Scene
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

void PauseIuranusState::renderWorld2D( ) {
	Global::renderStateEnableAlphaBlending( );
	Global::renderStateEnableZTest( );

	Global::renderStateEnableAlphaTest( );
	// Draw Bushes
	IuranusAnimatedObjectManager::getIuranusAnimatedObjectManager()->render();

	// Draw Shadows
	IuranusShadowManager::getIuranusShadowManager()->render();
	Global::renderStateDisableAlphaTest( );

	// Render IURANUS Particles
	ParticlesManager::getParticlesManager()->render();
	IuranusFixedParticlesManager::getIuranusFixedParticlesManager()->render();

	Global::renderStateEnableAdditiveAlphaBlending();
	Static2DObjectManager::getStatic2DObjectManager()->render();
	PlanetAtmosphereManager::getPlanetAtmosphereManager()->render();

	Global::renderStateDisableZTest( );
	Global::renderStateDisableAlphaBlending( );
}

void PauseIuranusState::renderPostProcessed2D( ) {
}

void PauseIuranusState::renderGameUI( ) {
	Global::renderStateEnableAlphaBlending();

	IuranusState * iuranusState = (IuranusState *) getState( "iuranus_state" );
	iuranusState->iuranusUI.render( );

	// Render fade medium out
	if( enter_pause )
		Global::fade.renderMediumFadeOut(0.5f, D3DXVECTOR3(0, 0.2f, 0));
	// Render fade in
	else if( exit_pause )
		Global::fade.renderFadeIn(0.5f, D3DXVECTOR3(0, 0.2f, 0));
	// Render pause UI
	else {
		Global::fade.renderMediumFadeOut(0.5f, D3DXVECTOR3(0, 0.2f, 0));

		flowerUp.render( );
		flowerDown.render( );

		alphabetRender.print(0.3f, 0.7f, 0.2f, 0.2f, "PAUSE");
		alphabetRender.print(0.5f, 0.5f, 0.05f, 0.05f, "Resume\n\n\nExit");
		arrow.render( );
	}

	Global::renderStateDisableAlphaBlending();
}

void PauseIuranusState::postRender( ) {
	// Print FPS
	Global::print(0,0, 0xffff0000, "Frames per second %0.2f", Game::fps);
	Global::print(0,10, 0xffff0000, "Pause Iuranus Status");
}

void PauseIuranusState::renderFrame( ) {
	Global::renderTextureQuad.render( );
}

void PauseIuranusState::onLoad( ) {
	// Pause sound
	//SoundManager::getSoundManager();

	enter_pause = true;
	exit_pause = false;
	onContinue = true;
	arrow.setPosition( onContinuePosition );
}

void PauseIuranusState::onUnload( ) {
	// Play sound
	//SoundManager::getSoundManager();
}

void PauseIuranusState::loadObjects( ) {
	// Arrow object
	D3DXMATRIX arrowtransformMatrix;
	D3DXMatrixIdentity( &arrowtransformMatrix );
	arrow.setTransformMatrix( arrowtransformMatrix );

	arrow.SETMATERIAL("selection_icon.dds", "shader.fx", "dummy_tech");

	IMesh * arrowMesh = QuadFactory::getQuadFactory()->createQuad(0.1f, 0.1f * Game::configOptions.aspectRatio );
	assert(arrowMesh);
	arrow.setMesh(arrowMesh);

	// Flower up
	D3DXMATRIX flowerUptransformMatrix;
	D3DXMatrixIdentity( &flowerUptransformMatrix );
	flowerUp.setTransformMatrix( flowerUptransformMatrix );
	flowerUp.setPosition( D3DXVECTOR3(-0.5f, 0.3f, 0.0f) );

	flowerUp.SETMATERIAL("flower_pause.dds", "shader.fx", "dummy_tech");

	IMesh * flowerUpMesh = QuadFactory::getQuadFactory()->createQuad(0.4f, 0.4f * Game::configOptions.aspectRatio );
	assert(flowerUpMesh);
	flowerUp.setMesh(flowerUpMesh);

	// Flower down
	D3DXMATRIX flowerDowntransformMatrix;
	D3DXMatrixRotationZ( &flowerDowntransformMatrix, D3DX_PI );
	flowerDown.setTransformMatrix( flowerDowntransformMatrix );
	flowerDown.setPosition( D3DXVECTOR3(0.5f, -0.3f, 0.0f) );

	flowerDown.SETMATERIAL("flower_pause.dds", "shader.fx", "dummy_tech");

	IMesh * flowerDownMesh = QuadFactory::getQuadFactory()->createQuad(0.4f, 0.4f * Game::configOptions.aspectRatio );
	assert(flowerDownMesh);
	flowerDown.setMesh(flowerDownMesh);

	// -------------------------------

	// Create the alphabet render
	alphabetRender.create( );

	if( Game::configOptions.aspectRatio != 4.0f/3.0f && Game::configOptions.aspectRatio != 5.0f/4.0f )
		onBackToMenuPosition = D3DXVECTOR3( -0.1f, -0.27f, 0.0f );
}

void PauseIuranusState::reloadShader( ) {
	arrow.reloadShader( );
	alphabetRender.reloadShader( );
	flowerUp.reloadShader( );
	flowerDown.reloadShader( );
}
