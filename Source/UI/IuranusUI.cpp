#include "IuranusUI.h"
#include "Factories/QuadFactory.h"
#include "Global/GlobalVariables.h"
#include "Global/GlobalFunctions.h"
#include "Source/Game.h"
#include "Logic/PlanetLogic/IuranusPlanetLogic/IuranusPlanetLogic.h"
#include "Managers/LogicManager/LogicTimerManager.h"
#include "Managers/SoundManager.h"
#include "GameState/IGameState.h"

const float IuranusUI::showWelcomeMsgTime = 4.0f;
const float IuranusUI::showWelcomeMsgFadeTime = 1.0f;

IuranusUI::IuranusUI( )
: planetTimeOut( 0 )
, timeOut( 0 )
, welcomeMessageAlpha( 0.0f )
{
}

IuranusUI::~IuranusUI( ) {
	OutputDebugString("Destroying IuranusUI...\n");
}

void IuranusUI::create( ) {
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

	// Number of enemies left picture object
	D3DXMATRIX numEnemiesLeftPicturetransformMatrix;
	D3DXMatrixIdentity( &numEnemiesLeftPicturetransformMatrix );
	// Set the position in the screen
	numEnemiesLeftPicturetransformMatrix(3,0) = -0.90f;
	numEnemiesLeftPicturetransformMatrix(3,1) = 0.83f;
	numEnemiesLeftPicture.setTransformMatrix( numEnemiesLeftPicturetransformMatrix );

	numEnemiesLeftPicture.SETMATERIAL("enemy_counter.dds", "shader.fx", "dummy_tech");

	IMesh * numEnemiesLeftPictureMesh = QuadFactory::getQuadFactory()->createQuad(0.09f - 0.01f, 0.09f * Game::configOptions.aspectRatio);
	assert(numEnemiesLeftPictureMesh);
	numEnemiesLeftPicture.setMesh(numEnemiesLeftPictureMesh);

	// Number of enemies left
	numEnemiesLeft.create( );

	// Time out 'go to the ass'
	timeOutNumberRender.create( );

	// Welcome message to Iuranus planet
	//welcomeMessage.create( );
	welcomeMessage.create( "welcome_iuranus.dds" );
}

void IuranusUI::update( float deltaTime ) {
	LogicTimerManager * logicTimerManager = LogicTimerManager::getLogicTimerManager();

	// Look for the planet timeout timer in LogicTimerManager
	const LogicTimer *logicTimer = logicTimerManager->getLogicTimer( "iuranus_planet_timeout" );
	if( logicTimer )
		planetTimeOut = logicTimer->getCurrentTimeLeft( );

	if( IuranusPlanetLogic::getIuranusPlanetLogic()->isVictoryActived( ) ) {
		// Look for the run to the ass timer in LogicTimerManager
		logicTimer = logicTimerManager->getLogicTimer( "victory_condition_achieved" );
		assert( logicTimer );
		timeOut = logicTimer->getCurrentTimeLeft( );
	}

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
}

void IuranusUI::render( ) {
	// Print time left
	timeLeftPicture.render( );

	IuranusPlanetLogic * iuranusPlanetLogic = IuranusPlanetLogic::getIuranusPlanetLogic( );
	bool victoryActived = iuranusPlanetLogic->isVictoryActived( );

	if( planetTimeOut > 20 ) {
		int minutes = planetTimeOut / 60;
		int seconds = planetTimeOut % 60;
		//timeLeft.print( 0.902f, 0.915f, 0.04f, 0.04f, "%d:%02d", minutes, seconds );
		timeLeft.print( 0.85f, 0.86f, 0.065f, 0.065f, "%d:%02d", minutes, seconds );
	}
	else {
		/*
		// Make time left beep and sound when planet timeout is less than 30 sec
		static float lastValue = 0.0f;
		float value = sin( 10.0f*Global::time );
		if ( value > -0.2f ) {
			int minutes = planetTimeOut / 60;
			int seconds = planetTimeOut % 60;
			timeLeft.print( 0.902f, 0.923f, 0.04f, 0.04f, "%02d:%02d", minutes, seconds );

			// This is for sound just one time (When you are playing)
			if( lastValue <= -0.2f && IGameState::currentState->getName() == "iuranus_state" ) {
				SoundManager *soundManager = SoundManager::getSoundManager();
				soundManager->playSample("enemy_foolished");
			}
		}
		lastValue = value;
		*/
		// Make time left beep and sound when planet timeout is less than 30 sec
		static float lastValue = 1.0f;
		float value = sin( 10.0f*Global::time );
		
		int minutes = planetTimeOut / 60;
		int seconds = planetTimeOut % 60;
		timeLeft.print( 0.85f - 0.02f*value, 0.86f, 0.065f + 0.01f*value, 0.065f + 0.01f*value, "%d:%02d", minutes, seconds );

		// This is for sound just one time (When you are playing)
		if ( value > 0.8f && lastValue <= 0.8f && IGameState::currentState->getName() == "iuranus_state" ) {
			SoundManager *soundManager = SoundManager::getSoundManager();
			soundManager->playSample("timeout_beep");
		}
		lastValue = value;
	}

	// Print number of enemies left
	numEnemiesLeftPicture.render( );

	/*
	int numEnemies = iuranusPlanetLogic->getNumEnemiesLeft( );
	char msg[5];
	sprintf_s( msg, sizeof( msg ), "%d", numEnemies );
	if( strlen( msg ) == 1 )
		numEnemiesLeft.print( 0.06f, 0.923f, 0.05f, 0.05f, "%s", msg );
	else
		numEnemiesLeft.print( 0.047f, 0.923f, 0.05f, 0.05f, "%s", msg );
	*/
	numEnemiesLeft.print( 0.075f, 0.86f, 0.07f, 0.07f, "%d", iuranusPlanetLogic->getNumEnemiesLeft( ) );

	// Print 'go to the ass' timeout
	if( victoryActived ) {
		timeOutNumberRender.print( /*0.37f*/ 0.3f, 0.86f, 0.06f, 0.06f, "Run to the Ass!");
		timeOutNumberRender.printClosing( 0.5f, 0.73f, 0.13f, 0.13f, "%d", timeOut );
	}

	// Show welcome message
	if( !timeoutShowWelcomeMsg.isFinished() && welcomeMessageAlpha > 0.0f ) {
		welcomeMessage.render( 0.7f * welcomeMessageAlpha );
		//welcomeMessage.print(0.3f, 0.8f, 0.1f, 0.1f, "Welcome to");
		//welcomeMessage.print(0.37f, 0.67f, 0.1f, 0.1f, "Iuranus");
	}
}

void IuranusUI::showWelcomeMsg( ) {
	timeoutShowWelcomeMsg.init( showWelcomeMsgTime );
	welcomeMessageAlpha = 0.0f;
}

void IuranusUI::reloadShader( ) {
	timeLeftPicture.reloadShader( );
	timeLeft.reloadShader( );

	numEnemiesLeftPicture.reloadShader( );
	numEnemiesLeft.reloadShader( );

	timeOutNumberRender.reloadShader( );

	welcomeMessage.reloadShader( );
}
