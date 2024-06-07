#include "CreditsState.h"
#include "MenuState.h"
#include "Global/GlobalVariables.h"
#include "Game/Game.h"

std::string CreditsState::name = "credits_state";

const float CreditsState::creditsTime = 22.1f;
const float CreditsState::designersTime = 7.0f;
const float CreditsState::programmersTime = 5.0f;
const float CreditsState::artistsTime = 5.0f;
const float CreditsState::soundTime = 5.0f;
const float CreditsState::fadeTime = 1.0f;

void CreditsState::update( float deltaTime ) {

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
			// Go to menu state, but telling it that not make its onLoad
			MenuState * menuState = (MenuState *) getState( "menu_state" );
			menuState->doOnLoad = false;
			menuState->getFocus( );
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
				timeElapsed <= (designersTime-fadeTime) ) {
				
				if( designersFirstTime ) {
					designersFirstTime = false;
					lastTime = timeElapsed;
				}

				float fade = (timeElapsed-lastTime) / fadeTime;

				designersAlpha = fade;
				designersAlpha = Global::clamp( designersAlpha, 0.0f, 1.0f );
			}
			else if ( timeElapsed > (designersTime-fadeTime) &&
				timeElapsed <= ((designersTime+programmersTime) - fadeTime) ) {

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
			else if ( timeElapsed > ((designersTime+programmersTime) - fadeTime) &&
				timeElapsed <= ((designersTime+programmersTime+artistsTime) - fadeTime) ) {

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
			else if ( timeElapsed > ((designersTime+programmersTime+artistsTime) - fadeTime) &&
				timeElapsed <= ((designersTime+programmersTime+artistsTime+soundTime) - fadeTime) ) {

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

void CreditsState::preRender( ) {
}

void CreditsState::renderWorld3D( ) {
	Global::renderStateEnableZTest( );
	Global::renderStateEnableZWriting( );

	// Menu State Renders
	MenuState *menuState = (MenuState *) getState( "menu_state" );
	menuState->menuPlanet.render();
	menuState->startAction.render();
	menuState->optionsAction.render();
	menuState->creditsAction.render();
	menuState->exitAction.render();

	menuState->sky.render();

	Global::renderStateDisableZWriting( );
	Global::renderStateDisableZTest( );
}

void CreditsState::renderWorld2D( ) {
}

void CreditsState::renderPostProcessed2D( ) {
	Global::renderStateEnableAlphaBlending( );

	MenuState *menuState = (MenuState *) getState( "menu_state" );
	menuState->title.render();

	Global::renderStateDisableAlphaBlending( );
}

void CreditsState::renderGameUI( ) {
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

void CreditsState::postRender( ) {
	// Print FPS
	Global::print(0,0, 0xffff0000, "Frames per second %0.2f", Game::fps);
	Global::print(0,10, 0xffff0000, "Credits Status");
}

void CreditsState::renderFrame( ) {
	Global::renderTextureQuad.render( );
}

void CreditsState::onLoad( ) {
	enter_credits = true;
	exit_credits = false;

	designersAlpha = 0.0f;
	programmersAlpha = 0.0f;
	artistsAlpha = 0.0f;
	soundAlpha = 0.0f;

	designersFirstTime = true;
	programmersFirstTime = true;
	artistsFirstTime = true;
	soundFirstTime = true;
	finalFirstTime = true;
}

void CreditsState::onUnload( ) {
}

void CreditsState::loadObjects( ) {
	designersFade.create( "credits_designers.dds" );
	programmersFade.create( "credits_programmers.dds" );
	artistsFade.create( "credits_artists.dds" );
	soundFade.create( "credits_sound.dds" );
}

void CreditsState::reloadShader( ) {
	designersFade.reloadShader( );
	programmersFade.reloadShader( );
	artistsFade.reloadShader( );
	soundFade.reloadShader( );
}
