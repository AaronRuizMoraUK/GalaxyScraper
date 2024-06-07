#include "EndGameIuranusState.h"
#include "Global/GlobalFunctions.h"
#include "Global/GlobalVariables.h"
#include "Game/Game.h"
#include "Managers/PlanetManager.h"
#include "Managers/IuranusManagers/IuranusEnemyManager.h"
#include "Managers/StaticObjectManager.h"
#include "Managers/Static2DObjectManager.h"
#include "Managers/IuranusManagers/IuranusPlanetElementsManager.h"
#include "Managers/IuranusManagers/IuranusFixedParticlesManager.h"
#include "Managers/IuranusManagers/IuranusAnimatedObjectManager.h"
#include "Managers/IuranusManagers/IuranusShadowManager.h"
#include "Managers/PlanetAtmosphereManager.h"
#include "Managers/SoundManager.h"
#include "Object/CollisionableObject/AnimatedObject/Player/Player.h"

std::string EndGameIuranusState::name = "end_game_iuranus_state";

const float EndGameIuranusState::creditsTime = 27.1f;
const float EndGameIuranusState::toBeContinuedTime = 5.0f;
const float EndGameIuranusState::designersTime = 7.0f;
const float EndGameIuranusState::programmersTime = 5.0f;
const float EndGameIuranusState::artistsTime = 5.0f;
const float EndGameIuranusState::soundTime = 5.0f;
const float EndGameIuranusState::fadeTime = 1.0f;

void EndGameIuranusState::update( float deltaTime ) {

	if ( enter_credits ) {
		if( !Global::fade.isFinished( ) )
			Global::fade.update( deltaTime );
		else {
			enter_credits = false;
			timeout.init( creditsTime );
		}
	}
	else if( exit_credits ) {
		if( !Global::fade.isFinished( ) ) {
			Global::fade.update( deltaTime );
		}
		else {
			getState("end_iuranus_to_menu_state")->getFocus();
		}
	}
	else {
		timeout.update( deltaTime );

		if( Global::ioStatus.start.becomesPressed( ) 
			|| Global::ioStatus.selectOption.becomesPressed() 
			|| timeout.isFinished( ) ) {
			exit_credits = true;
		}
		else {
			// Update the alpha values

			float timeElapsed = timeout.getElapsedTime();
			static float lastTime = timeElapsed;

			if( timeElapsed >= 0.0f &&
				timeElapsed <= (toBeContinuedTime-fadeTime) ) {

					if( toBeContinuedFirstTime ) {
						toBeContinuedFirstTime = false;
						lastTime = timeElapsed;
					}

					float fade = (timeElapsed-lastTime) / fadeTime;

					toBeContinuedAlpha = fade;
					toBeContinuedAlpha = Global::clamp( toBeContinuedAlpha, 0.0f, 1.0f );
			}
			else if( timeElapsed > (toBeContinuedTime-fadeTime) &&
				timeElapsed <= ((toBeContinuedTime+designersTime) - fadeTime) ) {

					if( designersFirstTime ) {
						designersFirstTime = false;
						lastTime = timeElapsed;
					}

					float fade = (timeElapsed-lastTime) / fadeTime;

					toBeContinuedAlpha = 1.0f-fade;
					toBeContinuedAlpha = Global::clamp( toBeContinuedAlpha, 0.0f, 1.0f );

					designersAlpha = fade;
					designersAlpha = Global::clamp( designersAlpha, 0.0f, 1.0f );
			}
			else if ( timeElapsed > ((toBeContinuedTime+designersTime) - fadeTime) &&
				timeElapsed <= ((toBeContinuedTime+designersTime+programmersTime) - fadeTime) ) {

					if( programmersFirstTime ) {
						programmersFirstTime = false;
						lastTime = timeElapsed;
					}

					float fade = (timeElapsed-lastTime) / fadeTime;

					designersAlpha = 1.0f-fade;
					designersAlpha = Global::clamp( designersAlpha, 0.0f, 1.0f );

					programmersAlpha = fade;
					programmersAlpha = Global::clamp( programmersAlpha, 0.0f, 1.0f );
			}
			else if ( timeElapsed > ((toBeContinuedTime+designersTime+programmersTime) - fadeTime) &&
				timeElapsed <= ((toBeContinuedTime+designersTime+programmersTime+artistsTime) - fadeTime) ) {

					if( artistsFirstTime ) {
						artistsFirstTime = false;
						lastTime = timeElapsed;
					}

					float fade = (timeElapsed-lastTime) / fadeTime;

					programmersAlpha = 1.0f-fade;
					programmersAlpha = Global::clamp( programmersAlpha, 0.0f, 1.0f );

					artistsAlpha = fade;
					artistsAlpha = Global::clamp( artistsAlpha, 0.0f, 1.0f );
			}
			else if ( timeElapsed > ((toBeContinuedTime+designersTime+programmersTime+artistsTime) - fadeTime) &&
				timeElapsed <= ((toBeContinuedTime+designersTime+programmersTime+artistsTime+soundTime) - fadeTime) ) {

					if( soundFirstTime ) {
						soundFirstTime = false;
						lastTime = timeElapsed;
					}

					float fade = (timeElapsed-lastTime) / fadeTime;

					artistsAlpha = 1.0f-fade;
					artistsAlpha = Global::clamp( artistsAlpha, 0.0f, 1.0f );

					soundAlpha = fade;
					soundAlpha = Global::clamp( soundAlpha, 0.0f, 1.0f );
			}
			else {

				if( finalFirstTime ) {
					finalFirstTime = false;
					lastTime = timeElapsed;
				}

				float fade = (timeElapsed-lastTime) / fadeTime;

				soundAlpha = 1.0f-fade;
				soundAlpha = Global::clamp( soundAlpha, 0.0f, 1.0f );
			}

		}

	}
}

void EndGameIuranusState::preRender( ) {
}

void EndGameIuranusState::renderWorld3D( ) {
	Global::renderStateEnableZTest( );
	Global::renderStateEnableZWriting( );

	// Game Renders
	Player::getPlayer()->render();
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

void EndGameIuranusState::renderWorld2D( ) {
	Global::renderStateEnableAlphaBlending( );
	Global::renderStateEnableZTest( );

	Global::renderStateEnableAlphaTest( );
	// Draw Bushes
	IuranusAnimatedObjectManager::getIuranusAnimatedObjectManager()->render();

	// Draw Shadows
	IuranusShadowManager::getIuranusShadowManager()->render();
	Global::renderStateDisableAlphaTest( );

	// Render IURANUS Fixed Particles
	IuranusFixedParticlesManager::getIuranusFixedParticlesManager()->render();

	Global::renderStateEnableAdditiveAlphaBlending();
	Static2DObjectManager::getStatic2DObjectManager()->render();
	PlanetAtmosphereManager::getPlanetAtmosphereManager()->render();

	Global::renderStateDisableZTest( );
	Global::renderStateDisableAlphaBlending( );
}

void EndGameIuranusState::renderPostProcessed2D( ) {
}

void EndGameIuranusState::renderGameUI( ) {
	Global::renderStateEnableAlphaBlending();

	// Render fade medium out
	if( enter_credits )
		Global::fade.renderMediumFadeOut(0.5f, D3DXVECTOR3(0, 0.2f, 0));
	// Render fade in
	else if( exit_credits )
		Global::fade.renderFadeIn(0.5f, D3DXVECTOR3(0, 0.2f, 0));
	// Render credits
	else {
		Global::fade.renderMediumFadeOut(0.5f, D3DXVECTOR3(0, 0.2f, 0));

		if( toBeContinuedAlpha>0.0f )
			toBeContinuedFade.render( toBeContinuedAlpha );
		if( designersAlpha>0.0f )
			designersFade.render( designersAlpha );
		if( programmersAlpha>0.0f )
			programmersFade.render( programmersAlpha );
		if( artistsAlpha>0.0f )
			artistsFade.render( artistsAlpha );
		if( soundAlpha>0.0f )
			soundFade.render( soundAlpha );
	}

	Global::renderStateDisableAlphaBlending();
}

void EndGameIuranusState::postRender( ) {
	// Print FPS
	Global::print(0,0, 0xffff0000, "Frames per second %0.2f", Game::fps);
	Global::print(0,10, 0xffff0000, "End Game Iuranus Status");
}

void EndGameIuranusState::renderFrame( ) {
	Global::renderTextureQuad.render( );
}

void EndGameIuranusState::onLoad( ) {
	/*
	// Set the camera to the end game position
	D3DXVECTOR3 endPosition( 0.0f, -50.0f, 200.0f );
	D3DXVECTOR3 endTarget( 0.0f, 0.0f, 0.0f );
	Global::camera->setUpVector( D3DXVECTOR3(0,1,0) );
	Global::camera->lookAt( endPosition, endTarget );
	*/

	enter_credits = true;
	exit_credits = false;

	toBeContinuedAlpha = 0.0f;
	designersAlpha = 0.0f;
	programmersAlpha = 0.0f;
	artistsAlpha = 0.0f;
	soundAlpha = 0.0f;

	toBeContinuedFirstTime = true;
	designersFirstTime = true;
	programmersFirstTime = true;
	artistsFirstTime = true;
	soundFirstTime = true;
	finalFirstTime = true;

	SoundManager *soundManager = SoundManager::getSoundManager();
	soundManager->playStream("to_be_continued");

}

void EndGameIuranusState::onUnload( ) {
}

void EndGameIuranusState::loadObjects( ) {
	toBeContinuedFade.create( "to_be_continued.dds" );
	designersFade.create( "credits_designers.dds" );
	programmersFade.create( "credits_programmers.dds" );
	artistsFade.create( "credits_artists.dds" );
	soundFade.create( "credits_sound.dds" );
}

void EndGameIuranusState::reloadShader( ) {
	toBeContinuedFade.reloadShader( );
	designersFade.reloadShader( );
	programmersFade.reloadShader( );
	artistsFade.reloadShader( );
	soundFade.reloadShader( );
}
