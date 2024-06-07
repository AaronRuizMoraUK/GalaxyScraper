#include "BoilerUI.h"
#include "Factories/QuadFactory.h"
#include "Global/GlobalFunctions.h"
#include "Global/GlobalVariables.h"
#include "Game/Game.h"
#include "Logic/PlanetLogic/BoilerPlanetLogic/BoilerPlanetLogic.h"
#include "Managers/LogicManager/LogicTimerManager.h"
#include "Managers/SoundManager.h"
#include <cassert>

const float BoilerUI::showWelcomeMsgTime = 4.0f;
const float BoilerUI::showWelcomeMsgFadeTime = 1.0f;

const D3DXVECTOR2 BoilerUI::initialFacePosition( -0.90f, 0.83f );
const float BoilerUI::faceWidth = 0.065f;
const float BoilerUI::nextFaceBias = faceWidth/2.0f + 0.10f;

const float BoilerUI::playerFiredTime = 3.0f;

BoilerUI::BoilerUI( )
: planetTimeOut( 0 )
, welcomeMessageAlpha( 0.0f )
, showFiredFace( false )
{
}

BoilerUI::~BoilerUI( ) {
	OutputDebugString("Destroying BoilerUI...\n");
}

void BoilerUI::create( ) {
	// Time left picture object
	D3DXMATRIX timeLeftPicturetransformMatrix;
	D3DXMatrixIdentity( &timeLeftPicturetransformMatrix );
	// Set the position in the screen
	timeLeftPicturetransformMatrix(3,0) = 0.87f;
	timeLeftPicturetransformMatrix(3,1) = 0.82f;
	timeLeftPicture.setTransformMatrix( timeLeftPicturetransformMatrix );

	timeLeftPicture.SETMATERIAL("seconds_counter.dds", "shader.fx", "dummy_tech");

	IMesh * timeLeftPictureMesh = QuadFactory::getQuadFactory()->createQuad(0.13f, 0.13f * Game::configOptions.aspectRatio);
	assert(timeLeftPictureMesh);
	timeLeftPicture.setMesh(timeLeftPictureMesh);

	// Time left
	timeLeft.create( );

	// Welcome message to Boiler planet
	welcomeMessage.create( "welcome_avernum.dds" );

	// Player lava falling faces
	D3DXMATRIX playerSmiletransformMatrix;
	D3DXMatrixIdentity( &timeLeftPicturetransformMatrix );
	playerSmile.setTransformMatrix( timeLeftPicturetransformMatrix );

	playerSmile.SETMATERIAL("happy_face.dds", "shader.fx", "dummy_tech");

	IMesh * playerSmileMesh = QuadFactory::getQuadFactory()->createQuad(faceWidth, faceWidth * Game::configOptions.aspectRatio);
	assert(playerSmileMesh);
	playerSmile.setMesh(playerSmileMesh);

	D3DXMATRIX playerFiredtransformMatrix;
	D3DXMatrixIdentity( &timeLeftPicturetransformMatrix );
	playerFired.setTransformMatrix( timeLeftPicturetransformMatrix );

	playerFired.SETMATERIAL("fired_face_low.dds", "shader.fx", "dummy_tech");

	IMesh *playerFiredMesh = QuadFactory::getQuadFactory()->createQuad(faceWidth, faceWidth * Game::configOptions.aspectRatio);
	assert(playerFiredMesh);
	playerFired.setMesh(playerFiredMesh);

	// Initialite lastTryingLeft with num tryings
	lastTryingLeft = BoilerPlanetLogic::getBoilerPlanetLogic()->getFiredRetries();
}

void BoilerUI::update( float deltaTime ) {
	LogicTimerManager * logicTimerManager = LogicTimerManager::getLogicTimerManager();

	// Look for the planet timeout timer in LogicTimerManager
	const LogicTimer *logicTimer = logicTimerManager->getLogicTimer( "boiler_planet_timeout" );
	if( logicTimer )
		planetTimeOut = logicTimer->getCurrentTimeLeft( );

	// Update welcome message time
	if( !timeoutShowWelcomeMsg.isFinished() ) {
		timeoutShowWelcomeMsg.update( deltaTime );

		float timeElapsed = timeoutShowWelcomeMsg.getElapsedTime();

		if( timeElapsed >= 0.0f &&
			timeElapsed <= showWelcomeMsgTime - showWelcomeMsgFadeTime ) {
			
			float fade = timeElapsed / showWelcomeMsgFadeTime;

			welcomeMessageAlpha = Global::clamp( fade, 0.0f, 1.0f );
		}
		else if ( timeElapsed > (showWelcomeMsgTime - showWelcomeMsgFadeTime) &&
			timeElapsed <= showWelcomeMsgTime) {

			float fade = (timeElapsed - (showWelcomeMsgTime - showWelcomeMsgFadeTime)) / showWelcomeMsgFadeTime;

			welcomeMessageAlpha = Global::clamp( 1.0f-fade, 0.0f, 1.0f );
		}
	}

	// Update lastTryingLeft
	BoilerPlanetLogic *boilerPlanetLogic = BoilerPlanetLogic::getBoilerPlanetLogic();
	int numRetriesLeft = boilerPlanetLogic->getFiredRetriesLeft();

	// If it is time to show fired face
	if( !playerFiredTimeout.isFinished() ) {
		// If it is the max num retries do not show an addition fired face
		if( numRetriesLeft == boilerPlanetLogic->getFiredRetries() ) {
			// Set timeout that next time will be finished
			float totalTime = playerFiredTimeout.getTotalTime( );
			playerFiredTimeout.update( totalTime );

			// reset variables
			showFiredFace = false;
			lastTryingLeft = numRetriesLeft;
		}
		else {
			// Update timeout and show fired face flickering
			playerFiredTimeout.update( deltaTime );

			static float delta = 0.0f;
			delta += deltaTime * 20.0f;
			if( sin( delta ) > 0.0f )
				showFiredFace = true;
			else
				showFiredFace = false;
		}
	}
	// Do not show fired face
	else {
		showFiredFace = false;
	}

	// If there is one retry less start timeout
	if( lastTryingLeft > numRetriesLeft ) {
		lastTryingLeft = numRetriesLeft;
		playerFiredTimeout.init( playerFiredTime );
	}
}

void BoilerUI::render( ) {
	timeLeftPicture.render( );

	if( planetTimeOut > 20 ) {
		int minutes = planetTimeOut / 60;
		int seconds = planetTimeOut % 60;
		//timeLeft.print( 0.902f, 0.915f, 0.04f, 0.04f, "%d:%02d", minutes, seconds );
		timeLeft.print( 0.85f, 0.86f, 0.065f, 0.065f, "%d:%02d", minutes, seconds );
	}
	else {
		// Make time left beep and sound when planet timeout is less than 30 sec
		static float lastValue = 1.0f;
		float value = sin( 10.0f*Global::time );
		
		int minutes = planetTimeOut / 60;
		int seconds = planetTimeOut % 60;
		timeLeft.print( 0.85f - 0.02f*value, 0.86f, 0.065f + 0.01f*value, 0.065f + 0.01f*value, "%d:%02d", minutes, seconds );

		// This is for sound just one time (When you are playing)
		if ( value > 0.8f && lastValue <= 0.8f && IGameState::currentState->getName() == "boiler_state" ) {
			SoundManager *soundManager = SoundManager::getSoundManager();
			soundManager->playSample("timeout_beep");
		}
		lastValue = value;
	}

	// Show faces just in boiler state
	const std::string &currentStateName = IGameState::currentState->getName();
	if( currentStateName == "boiler_state" ||
		currentStateName == "pause_boiler_state" ) {
		int numRetriesLeft = BoilerPlanetLogic::getBoilerPlanetLogic()->getFiredRetriesLeft();

		for( int i=0; i<numRetriesLeft; ++i ) {
			D3DXVECTOR3 pos( initialFacePosition.x + i * nextFaceBias, initialFacePosition.y, 0.0f );
			playerSmile.setPosition( pos );
			playerSmile.render();
		}

		if( showFiredFace ) {
			D3DXVECTOR3 pos( initialFacePosition.x + numRetriesLeft * nextFaceBias, initialFacePosition.y, 0.0f );
			playerFired.setPosition( pos );
			playerFired.render();
		}
	}

	// Show welcome message
	if( !timeoutShowWelcomeMsg.isFinished() && welcomeMessageAlpha > 0.0f ) {
		welcomeMessage.render( 0.7f * welcomeMessageAlpha );
	}
}

void BoilerUI::showWelcomeMsg( ) {
	timeoutShowWelcomeMsg.init( showWelcomeMsgTime );
	welcomeMessageAlpha = 0.0f;
}

void BoilerUI::reloadShader( ) {
	timeLeftPicture.reloadShader( );
	timeLeft.reloadShader( );

	welcomeMessage.reloadShader( );

	playerSmile.reloadShader( );
	playerFired.reloadShader( );
}
