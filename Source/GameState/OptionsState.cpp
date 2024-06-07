#include "OptionsState.h"
#include "MenuState.h"
#include "Global/GlobalVariables.h"
#include "Game/Game.h"
#include "Factories/QuadFactory.h"
#include "Managers/SoundManager.h"
#include "Managers/DataLocatorManager.h"
#include "Logic/PlanetLogic/IuranusPlanetLogic/IuranusPlanetLogic.h"
#include <fstream>

std::string OptionsState::name = "options_state";

const D3DXVECTOR3 OptionsState::onExitPosition( -0.80f, 0.6f, 0.0f );
D3DXVECTOR3 OptionsState::onResolutionPosition( -0.80f, 0.4f, 0.0f );
D3DXVECTOR3 OptionsState::onQualityPosition( -0.80f, 0.2f, 0.0f );
D3DXVECTOR3 OptionsState::onMusicPosition( -0.80f, 0.0f, 0.0f );
D3DXVECTOR3 OptionsState::onSFXPosition( -0.80f, -0.2f, 0.0f );
D3DXVECTOR3 OptionsState::onButtonPosition( -0.80f, -0.4f, 0.0f );

OptionsState::OptionsState()
	: IGameState()
	, enter_options(true)
	, exit_options(false)
	, warning_changes(false)
	, pressAButtonNow( false )
	, optionPosition( EXIT )
	, quality( HIGH )
	, buttonsOptionPosition( JUMP_KEY )
	, isChangingSetting( false )
	, outputStringStream( "" )
{
};

void OptionsState::update( float deltaTime ) {

	if ( enter_options ) {
		if( !Global::fade.isFinished( ) )
			Global::fade.update( deltaTime );
		else
			enter_options = false;
	}
	else if( exit_options ) {
		if( !Global::fade.isFinished( ) ) {
			applyChanges( );
			Global::fade.update( deltaTime );
		}
		else {
			// Go to menu state, but telling it that not make its onLoad
			MenuState * menuState = (MenuState *) getState( "menu_state" );
			menuState->doOnLoad = false;
			menuState->getFocus( );
		}
	}
	else if( warning_changes ) {
		if( Global::ioStatus.start.becomesPressed( )
			|| Global::ioStatus.selectOption.becomesPressed( ) )
			exit_options = true;
	}
	else {

		if( !isChangingSetting ) {
			// We are in the first level chossing an option

			if( (Global::ioStatus.exitOptions.becomesPressed( ) 
				|| Global::ioStatus.start.becomesPressed( )
				|| Global::ioStatus.selectOption.becomesPressed( )) &&
				optionPosition == EXIT ) {
				// If resolution changes show warning msg
				if( currentOptions.resolution != Game::configOptions.resolution )
					warning_changes = true;
				else
					exit_options = true;
			}
			else if( Global::ioStatus.exitOptions.becomesPressed( ) ) {
				optionPosition = EXIT;
				arrow.setPosition( onExitPosition );
			}
			else if( Global::ioStatus.start.becomesPressed( )
				|| Global::ioStatus.selectOption.becomesPressed( ) ) {
				isChangingSetting = true; // We pass to change setting
			}
			else if (  Global::ioStatus.walk.becomesPressedUp() && optionPosition != EXIT ) {
				optionPosition = (OptionPosition)((optionPosition+NUM_OPTION_POSITIONS-1)%NUM_OPTION_POSITIONS);
				if( optionPosition == EXIT )
					arrow.setPosition( onExitPosition );
				else if( optionPosition == RESOLUTION )
					arrow.setPosition( onResolutionPosition );
				else if( optionPosition == QUALITY )
					arrow.setPosition( onQualityPosition );
				else if( optionPosition == MUSIC )
					arrow.setPosition( onMusicPosition );
				else if( optionPosition == SFX )
					arrow.setPosition( onSFXPosition );
				else if( optionPosition == BUTTONS )
					arrow.setPosition( onButtonPosition );
			}
			else if (  Global::ioStatus.walk.becomesPressedDown() && optionPosition != BUTTONS ) {
				optionPosition = (OptionPosition)((optionPosition+1)%NUM_OPTION_POSITIONS);
				if( optionPosition == EXIT )
					arrow.setPosition( onExitPosition );
				else if( optionPosition == RESOLUTION )
					arrow.setPosition( onResolutionPosition );
				else if( optionPosition == QUALITY )
					arrow.setPosition( onQualityPosition );
				else if( optionPosition == MUSIC )
					arrow.setPosition( onMusicPosition );
				else if( optionPosition == SFX )
					arrow.setPosition( onSFXPosition );
				else if( optionPosition == BUTTONS )
					arrow.setPosition( onButtonPosition );
			}

		}
		else {
			if( pressAButtonNow ) {
				// Update press button fade
				pressButtonFade.update( deltaTime );

				// We have to press a button or press ESC
				updateButtonsSettings( );
			}
			else {
				// We are in any options trying to change its value
				updateSetting( );
			}
		}

	}
}

void OptionsState::updateSetting( ) {

	// We pass to the first level again
	if( Global::ioStatus.exitOptions.becomesPressed( ) ) {
		isChangingSetting = false;
		return;
	}

	// Set the positions and rotation of the selection quad
	D3DXMATRIX scl;
	D3DXMatrixScaling( &scl, 1.0f, 1.0f/Game::configOptions.aspectRatio, 1.0f );
	D3DXMATRIX trf;
	D3DXMatrixRotationZ( &trf, 3.0f*Global::time );
	D3DXMatrixMultiply( &trf, &scl, &trf );
	D3DXMatrixScaling( &scl, 1.0f, Game::configOptions.aspectRatio, 1.0f );
	D3DXMatrixMultiply( &trf, &trf, &scl );
	selectionQuad.setTransformMatrix( trf );
	if( optionPosition != BUTTONS )
		selectionQuad.setPosition( arrow.getPosition( ) + D3DXVECTOR3(0.93f, 0.0f, 0.0f) );
	else
		udpatePositionInButtons( );

	// Update Options
	if( optionPosition == RESOLUTION ) {
		updateResolution( );
	}
	else if( optionPosition == QUALITY ) {
		updateQuality( );
	}
	else if( optionPosition == MUSIC ) {
		updateMusic( );
	}
	else if( optionPosition == SFX ) {
		updateSFX( );
	}
	else if( optionPosition == BUTTONS ) {
		// When player press start on a button, pass to "press a button now" mode
		if( Global::ioStatus.enterOptions.becomesPressed() ) {
			pressAButtonNow = true;
			return;
		}

		// Necessary for fixing problem in XInputGetKeystroke function on XBOX360GAMEPAD in IOStatus
		std::string fix;
		Global::ioStatus.detectButtonOnInput( XBOX360GAMEPAD, fix );
	}
	else {
		assert( !"Option position do not supported in OptionState" );
		return;
	}
}

void OptionsState::updateResolution( ) {
	// We pass to the first level again
	if( Global::ioStatus.start.becomesPressed( )
		|| Global::ioStatus.selectOption.becomesPressed( ) ) {
		isChangingSetting = false;
		return;
	}
	else if( Global::ioStatus.walk.becomesPressedDown() && resolutionsIt != Game::allowedResolutions.begin() ) {
		--resolutionsIt;
		currentOptions.resolution = (*resolutionsIt);
	}
	else if( Global::ioStatus.walk.becomesPressedUp() &&
		     resolutionsIt != --Game::allowedResolutions.end() &&
			 resolutionsIt != Game::allowedResolutions.end() ) {
		++resolutionsIt;
		currentOptions.resolution = (*resolutionsIt);
	}
}

void OptionsState::updateQuality( ) {
	// We pass to the first level again
	if( Global::ioStatus.start.becomesPressed( )
		|| Global::ioStatus.selectOption.becomesPressed( ) ) {
		isChangingSetting = false;
		return;
	}
	else if( Global::ioStatus.walk.becomesPressedDown() && quality != MINIMUM ) {
		quality = (Quality) (quality - 1);
		currentOptions.quality = getQualityString( quality );
	}
	else if( Global::ioStatus.walk.becomesPressedUp() && quality != HIGH ) {
		quality = (Quality) (quality + 1);
		currentOptions.quality = getQualityString( quality );
	}
}

void OptionsState::updateMusic( ) {
	// We pass to the first level again
	if( Global::ioStatus.start.becomesPressed( )
		|| Global::ioStatus.selectOption.becomesPressed( ) ) {
		isChangingSetting = false;
		return;
	}
	else if( Global::ioStatus.walk.becomesPressedUp() ) {
		SoundManager *soundManager = SoundManager::getSoundManager();
		soundManager->upStreamVolume();
		currentOptions.music = soundManager->getStreamVolume();
	}
	else if( Global::ioStatus.walk.becomesPressedDown() ) {
		SoundManager *soundManager = SoundManager::getSoundManager();
		soundManager->downStreamVolume();
		currentOptions.music = soundManager->getStreamVolume();
	}
}

void OptionsState::updateSFX( ) {
	// We pass to the first level again

	if( Global::ioStatus.start.becomesPressed( )
		|| Global::ioStatus.selectOption.becomesPressed( ) ) {
		isChangingSetting = false;
		return;
	}
	else if( Global::ioStatus.walk.becomesPressedUp() ) {
		SoundManager *soundManager = SoundManager::getSoundManager();
		soundManager->upSampleVolume();
		soundManager->playSample("enemy_running_away_from_mouth");
		currentOptions.sfx= soundManager->getSampleVolume();
	}
	else if( Global::ioStatus.walk.becomesPressedDown() ) {
		SoundManager *soundManager = SoundManager::getSoundManager();
		soundManager->downSampleVolume();
		soundManager->playSample("enemy_running_away_from_mouth");
		currentOptions.sfx= soundManager->getSampleVolume();
	}
}

void OptionsState::udpatePositionInButtons( ) {

	int x = buttonsOptionPosition % 3;
	int y = buttonsOptionPosition / 3;

	if( Global::ioStatus.walk.becomesPressedUp() ) {
		if( buttonsOptionPosition == PAUSE_360 ) {
			buttonsOptionPosition = KICK_360;
		}
		// If we are not in the first line
		else if( y != 0 ) {
			buttonsOptionPosition = (ButtonsOptionPosition) (buttonsOptionPosition - 3);
		}
	}
	else if( Global::ioStatus.walk.becomesPressedDown() ) {
		if( buttonsOptionPosition == KICK_360 ) {
			buttonsOptionPosition = PAUSE_360;
		}
		// If we are not in the last line
		else if( y != 3 && buttonsOptionPosition != WALK_KEY ) {
			buttonsOptionPosition = (ButtonsOptionPosition) (buttonsOptionPosition + 3);
		}
	}
	else if( Global::ioStatus.walk.becomesPressedRight() ) {
		// If we are not in the right line
		if( x != 2 && buttonsOptionPosition != WALK_PAD ) {
			buttonsOptionPosition = (ButtonsOptionPosition) (buttonsOptionPosition + 1);
		}
	}
	else if( Global::ioStatus.walk.becomesPressedLeft() ) {
		// If we are not in the left line
		if( x != 0 && buttonsOptionPosition != PAUSE_PAD ) {
			buttonsOptionPosition = (ButtonsOptionPosition) (buttonsOptionPosition - 1);
		}
	}

	D3DXVECTOR3 displacement( 0.31f*x, -0.11f*y, 0.0f );
	if( x == 0 )
		displacement.x -= 0.088f;
	selectionQuad.setPosition( arrow.getPosition( ) + D3DXVECTOR3(0.85f, -0.13f, 0.0f) + displacement );
}

void OptionsState::updateButtonsSettings( ) {
	// Continue rotating de selection Quad
	D3DXMATRIX scl;
	D3DXMatrixScaling( &scl, 1.0f, 1.0f/Game::configOptions.aspectRatio, 1.0f );
	D3DXMATRIX trf;
	D3DXMatrixRotationZ( &trf, 3.0f*Global::time );
	D3DXMatrixMultiply( &trf, &scl, &trf );
	D3DXMatrixScaling( &scl, 1.0f, Game::configOptions.aspectRatio, 1.0f );
	D3DXMatrixMultiply( &trf, &trf, &scl );
	D3DXVECTOR3 prevPos = selectionQuad.getPosition( );
	selectionQuad.setTransformMatrix( trf );
	selectionQuad.setPosition( prevPos );

	// We exit of the "press a button now" mode
	if( Global::ioStatus.exitOptions.becomesPressed( ) ) {
		pressAButtonNow = false;
		return;
	}

	// Obtain by the current position in button what input is
	InputDevices input;
	int valuePos = buttonsOptionPosition % 3;
	if( valuePos == 0 )
		input = KEYBOARD;
	else if( valuePos == 1 )
		input = GAMEPAD;
	else if( valuePos == 2 )
		input = XBOX360GAMEPAD;
	else {
		assert( !"Position in the buttons unknown in OptionsState" );
		return;
	}

	// Detect if any button has been pressed, and if it's so, what button
	std::string buttonPressed = "";
	bool buttonDetected = Global::ioStatus.detectButtonOnInput( input, buttonPressed );

	// If any button was pressed
	if( buttonDetected ) {
		//char msg[ 128 ];
		//sprintf_s(msg, sizeof(msg), "Pushed: %s\n", buttonPressed.c_str());
		//OutputDebugString(msg);

		// Obtain the current action
		Actions action;
		int valueAct = buttonsOptionPosition / 3;
		if( valueAct == 0 )
			action = JUMP;
		else if( valueAct == 1 )
			action = KICK;
		else if( valueAct == 2 )
			action = WALK;
		else if( valueAct == 3 )
			action = PAUSE;
		else {
			assert( !"Action in the buttons unknown in OptionsState" );
			return;
		}

		// Modify ioStatus and xml
		Global::ioStatus.setButton( input, action, buttonPressed );
		Global::ioStatus.writeXML( );

		// We exit of the "press a button now" mode
		pressAButtonNow = false;
		return;
	}

}

void OptionsState::applyChanges( ) {

	// Changes the configOptions of Game and write it in XML

	bool writeXML = false;

	// Resolution
	if( currentOptions.resolution != Game::configOptions.resolution ) {
		writeXML = true;
	}

	// Quality
	if( currentOptions.quality != Game::configOptions.quality ) {

		Game::configOptions.quality = currentOptions.quality;
		writeXML = true;

		// Change Quality
		changeQuality( );
	}

	// Volumes
	if( currentOptions.music != Game::configOptions.music ||
		currentOptions.sfx   != Game::configOptions.sfx ) {

		Game::configOptions.music = currentOptions.music;
		Game::configOptions.sfx = currentOptions.sfx;
		writeXML = true;
	}

	// Write in xml the new options
	if( writeXML ) {
		DataLocatorManager *dataLocatorManager = DataLocatorManager::getDataLocatorManager();
		const std::string &pathToXml = dataLocatorManager->getPath("xml");
		std::string configFile = pathToXml + "config.xml";

		// Insert the header in the string output stream
		outputStringStream << "<?xml version=\"1.0\" encoding=\"iso-8859-1\"?>\n";
		outputStringStream << "<config>\n";

		// Insert the attributes in the string output stream
		xmlParseFile( configFile );

		// Insert the final in the string output stream
		outputStringStream << "</config>\n";

		// Save the string output stream to a file
		std::ofstream xmlStream;
		xmlStream.open( configFile.c_str() );
		xmlStream << outputStringStream.str();
		xmlStream.close();

		// Clear the string output stream
		outputStringStream.str("");
	}
}

void OptionsState::changeQuality( ) {
	if( Game::configOptions.quality == "high" )
		Global::enabledRenderTexture = RenderToSurfaceSupported;
	else
		Global::enabledRenderTexture = false;

	// Reload Shaders
	Game::reloadShaders();

	// Change Logic
	IuranusPlanetLogic::getIuranusPlanetLogic()->setTotalEnemiesInScreen( );
}

void OptionsState::preRender( ) {
}

void OptionsState::renderWorld3D( ) {
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

void OptionsState::renderWorld2D( ) {
}

void OptionsState::renderPostProcessed2D( ) {
	Global::renderStateEnableAlphaBlending( );

	MenuState *menuState = (MenuState *) getState( "menu_state" );
	menuState->title.render();

	Global::renderStateDisableAlphaBlending( );
}

void OptionsState::renderGameUI( ) {
	Global::renderStateEnableAlphaBlending();

	// Render fade medium out
	if( enter_options )
		Global::fade.renderMediumFadeOut(0.5f, D3DXVECTOR3(0, 0.2f, 0));
	// Render fade in
	else if( exit_options )
		Global::fade.renderFadeIn(0.5f, D3DXVECTOR3(0, 0.2f, 0));
	// Warning changes
	else if( warning_changes ) {
		Global::fade.renderMediumFadeOut(0.5f, D3DXVECTOR3(0, 0.2f, 0));

		flowerChangesUp.render( );
		flowerChangesDown.render( );

		alphabetRender.print(0.23f, 0.6f, 0.05f, 0.05f, "Resolution will change");
		alphabetRender.print(0.2f, 0.55f, 0.05f, 0.05f, "when you restart the game");
		alphabetRender.print(0.38f, 0.4f, 0.05f, 0.05f, "Press Enter");
	}
	// Render options
	else {
		flowerUp.render( );
		flowerDown.render( );

		Global::fade.renderMediumFadeOut(0.5f, D3DXVECTOR3(0, 0.2f, 0));

		alphabetRender.print(0.7f, 0.95f, 0.035f, 0.036f, "enter-select\n      option");
		alphabetRender.print(0.7f, 0.88f, 0.035f, 0.036f, "  esc-exit\n      option");

		alphabetRender.print(0.155f, 0.8f, 0.06f, 0.06f, "Exit");
		alphabetRender.print(0.155f, 0.7f, 0.06f, 0.06f, "Resolution");
		alphabetRender.print(0.155f, 0.6f, 0.06f, 0.06f, "Quality");
		alphabetRender.print(0.155f, 0.5f, 0.06f, 0.06f, "Music");
		alphabetRender.print(0.155f, 0.4f, 0.06f, 0.06f, "SFX");
		alphabetRender.print(0.155f, 0.3f, 0.06f, 0.06f, "Buttons");

		if( currentOptions.resolution.rate != -1 )
			alphabetRender.print(0.6f, 0.7f, 0.05f, 0.05f, "%dx%d:%dHz", currentOptions.resolution.width, currentOptions.resolution.height, currentOptions.resolution.rate);
		else
			alphabetRender.print(0.6f, 0.7f, 0.05f, 0.05f, "%dx%d", currentOptions.resolution.width, currentOptions.resolution.height);
		alphabetRender.print(0.6f, 0.6f, 0.05f, 0.05f, "%s", currentOptions.quality.c_str());
		alphabetRender.print(0.6f, 0.5f, 0.05f, 0.05f, "%d%%", currentOptions.music);
		alphabetRender.print(0.6f, 0.4f, 0.05f, 0.05f, "%d%%", currentOptions.sfx);

		alphabetRender.print(0.45f, 0.3f, 0.035f, 0.035f, "  Key+Mouse   PAD    XBOX360");
		alphabetRender.print(0.25f, 0.23f, 0.04f, 0.04f, "Jump");
		alphabetRender.print(0.25f, 0.175f, 0.04f, 0.04f, "Kick");
		alphabetRender.print(0.25f, 0.12f, 0.04f, 0.04f, "Walk");
		alphabetRender.print(0.25f, 0.065f, 0.04f, 0.04f, "Start-Pause");

		// Obtain the config keys
		const DigitalActionConfigKeys & jumpConfigKeys = Global::ioStatus.jump.getDigitalActionConfigKeys( );
		const DigitalActionConfigKeys & kickConfigKeys = Global::ioStatus.kick.getDigitalActionConfigKeys( );
		const DigitalActionConfigKeys & walkConfigKeys = Global::ioStatus.walkSlow.getDigitalActionConfigKeys( );
		const DigitalActionConfigKeys & startConfigKeys = Global::ioStatus.start.getDigitalActionConfigKeys( );

		printfKeyKeyboard( 0.23f,  "%s", jumpConfigKeys.getKeyboardConfigKey( ).c_str());
		printfKeyKeyboard( 0.175f, "%s", kickConfigKeys.getKeyboardConfigKey( ).c_str());
		printfKeyKeyboard( 0.12f,  "%s", walkConfigKeys.getKeyboardConfigKey( ).c_str());
		printfKeyKeyboard( 0.065f, "ENTER" );

		padButtonsRender.print(0.715f, 0.23f, 0.06f, 0.06f,  "%d", jumpConfigKeys.getGamePadConfigKey( ));
		padButtonsRender.print(0.715f, 0.175f, 0.06f, 0.06f, "%d", kickConfigKeys.getGamePadConfigKey( ));
		padButtonsRender.print(0.715f, 0.12f, 0.06f, 0.06f,  "%d", walkConfigKeys.getGamePadConfigKey( ));
		padButtonsRender.print(0.715f, 0.065f, 0.06f, 0.06f, "%d", startConfigKeys.getGamePadConfigKey( ));

		xboxButtonsRender.print(0.87f, 0.23f, 0.06f, 0.06f,  "%s", jumpConfigKeys.getXBOX360GamePadConfigKey( ).c_str());
		xboxButtonsRender.print(0.87f, 0.175f, 0.06f, 0.06f, "%s", kickConfigKeys.getXBOX360GamePadConfigKey( ).c_str());
		alphabetRender.print(0.87f,    0.12f, 0.06f, 0.06f,  "-");
		xboxButtonsRender.print(0.87f, 0.065f, 0.06f, 0.06f, "%s", startConfigKeys.getXBOX360GamePadConfigKey( ).c_str());

		arrow.render( );
		if( isChangingSetting )
			selectionQuad.render( );

		if( pressAButtonNow ) {
			pressButtonFade.renderMediumFadeOut( 0.001f );
			alphabetRender.print(0.2f, 0.55f, 0.07f, 0.07f, "Press any button or\n  ESCAPE to exit" );
		}
	}

	Global::renderStateDisableAlphaBlending();
}

void OptionsState::printfKeyKeyboard( float y, const char *msg, ... ) {
	char txt[512];
	va_list ap;
	va_start(ap, msg);
	vsnprintf_s(txt, sizeof( txt ), 511, msg, ap);
	va_end (ap);

	std::string letters( txt );

	std::string::size_type lettersSize = letters.size();
	if( lettersSize == 0 )
		assert( !"Letters size incorrect in OptionsState" );

	const float size = 0.04f;
	float lettersMiddleWidth = size * (lettersSize-1.0f) / 4.0f;

	alphabetRender.print(0.56f-lettersMiddleWidth, y, size, size, "%s", letters.c_str());
}

void OptionsState::postRender( ) {
	// Print FPS
	Global::print(0,0, 0xffff0000, "Frames per second %0.2f", Game::fps);
	Global::print(0,10, 0xffff0000, "Credits Status");
}

void OptionsState::renderFrame( ) {
	Global::renderTextureQuad.render( );
}

void OptionsState::onLoad( ) {
	enter_options = true;
	exit_options = false;
	warning_changes = false;
	optionPosition = EXIT;

	pressAButtonNow = false;

	buttonsOptionPosition = JUMP_KEY;

	// Arrow position
	arrow.setPosition( onExitPosition );

	isChangingSetting = false;

	currentOptions = Game::configOptions;

	resolutionsIt = Game::allowedResolutions.find( Game::configOptions.resolution );
	assert( !(Game::configOptions.fullScreen && resolutionsIt == Game::allowedResolutions.end()) ||
			!"Wrong current resolution in OptionsState" );

	quality = getQuality( Game::configOptions.quality );
}

void OptionsState::onUnload( ) {
}

void OptionsState::loadObjects( ) {
	// Arrow object
	D3DXMATRIX arrowtransformMatrix;
	D3DXMatrixIdentity( &arrowtransformMatrix );
	arrow.setTransformMatrix( arrowtransformMatrix );

	arrow.SETMATERIAL("selection_icon.dds", "shader.fx", "dummy_tech");

	IMesh * arrowMesh = QuadFactory::getQuadFactory()->createQuad(0.1f, 0.1f * Game::configOptions.aspectRatio );
	assert(arrowMesh);
	arrow.setMesh(arrowMesh);

	// Selection Quad object
	D3DXMATRIX selectionQuadtransformMatrix;
	D3DXMatrixIdentity( &selectionQuadtransformMatrix );
	selectionQuad.setTransformMatrix( selectionQuadtransformMatrix );

	selectionQuad.SETMATERIAL("current_selection_icon.dds", "shader.fx", "dummy_tech");

	IMesh * selectionQuadMesh = QuadFactory::getQuadFactory()->createQuad(0.04f, 0.04f * Game::configOptions.aspectRatio );
	assert(selectionQuadMesh);
	selectionQuad.setMesh(selectionQuadMesh);

	// Flower up
	D3DXMATRIX flowerUptransformMatrix;
	D3DXMatrixIdentity( &flowerUptransformMatrix );
	flowerUp.setTransformMatrix( flowerUptransformMatrix );
	flowerUp.setPosition( D3DXVECTOR3(-0.3f, 0.65f, 0.0f) );

	flowerUp.SETMATERIAL("flower_options.dds", "shader.fx", "dummy_tech");

	IMesh * flowerUpMesh = QuadFactory::getQuadFactory()->createQuad(0.6f, 0.15f * Game::configOptions.aspectRatio );
	assert(flowerUpMesh);
	flowerUp.setMesh(flowerUpMesh);

	// Flower down
	D3DXMATRIX flowerDowntransformMatrix;
	D3DXMatrixRotationZ( &flowerDowntransformMatrix, D3DX_PI );
	flowerDown.setTransformMatrix( flowerDowntransformMatrix );
	flowerDown.setPosition( D3DXVECTOR3(0.3f, -0.65f, 0.0f) );

	flowerDown.SETMATERIAL("flower_options.dds", "shader.fx", "dummy_tech");

	IMesh * flowerDownMesh = QuadFactory::getQuadFactory()->createQuad(0.6f, 0.15f * Game::configOptions.aspectRatio );
	assert(flowerDownMesh);
	flowerDown.setMesh(flowerDownMesh);

	// Flower Changes up
	D3DXMATRIX flowerChangesUptransformMatrix;
	D3DXMatrixRotationZ( &flowerChangesUptransformMatrix, D3DX_PI );
	flowerChangesUp.setTransformMatrix( flowerChangesUptransformMatrix );
	flowerChangesUp.setPosition( D3DXVECTOR3(-0.3f, 0.5f, 0.0f) );

	flowerChangesUp.SETMATERIAL("flower_changes.dds", "shader.fx", "dummy_tech");

	IMesh * flowerChangesUpMesh = QuadFactory::getQuadFactory()->createQuad(0.5f, 0.125f * Game::configOptions.aspectRatio );
	assert(flowerChangesUpMesh);
	flowerChangesUp.setMesh(flowerChangesUpMesh);

	// Flower Changes down
	D3DXMATRIX flowerChangesDowntransformMatrix;
	D3DXMatrixIdentity( &flowerChangesDowntransformMatrix );
	flowerChangesDown.setTransformMatrix( flowerChangesDowntransformMatrix );
	flowerChangesDown.setPosition( D3DXVECTOR3(0.3f, -0.5f, 0.0f) );

	flowerChangesDown.SETMATERIAL("flower_changes.dds", "shader.fx", "dummy_tech");

	IMesh * flowerChangesDownMesh = QuadFactory::getQuadFactory()->createQuad(0.5f, 0.125f * Game::configOptions.aspectRatio );
	assert(flowerChangesDownMesh);
	flowerChangesDown.setMesh(flowerChangesDownMesh);

	// ---------------------------------

	alphabetRender.create( );
	xboxButtonsRender.create( );
	padButtonsRender.create( );

	pressButtonFade.create( );
}

void OptionsState::reloadShader( ) {
	arrow.reloadShader( );
	selectionQuad.reloadShader( );
	alphabetRender.reloadShader( );
	xboxButtonsRender.reloadShader( );
	padButtonsRender.reloadShader( );
	pressButtonFade.reloadShader( );
	flowerUp.reloadShader( );
	flowerDown.reloadShader( );
	flowerChangesUp.reloadShader( );
	flowerChangesDown.reloadShader( );
}

void OptionsState::onStartElement (const std::string &elem, MKeyValue &atts) {
	if( elem == "video" ) {
		atts.put( "width", currentOptions.resolution.width );
		atts.put( "height", currentOptions.resolution.height );
		atts.put( "rate", currentOptions.resolution.rate );

		atts.writeSingle(outputStringStream, elem.c_str() );
	}
	else if ( elem == "options" ) {
		atts.put( "quality", currentOptions.quality.c_str() );
		atts.put( "music", currentOptions.music );
		atts.put( "sfx", currentOptions.sfx );

		atts.writeSingle(outputStringStream, elem.c_str() );
	}
}

std::string minimum = "minimum";
std::string low = "low";
std::string medium = "medium";
std::string high = "high";
const std::string &OptionsState::getQualityString( Quality pQuality ) const {
	if( pQuality == MINIMUM )
		return minimum;
	else if( pQuality == LOW )
		return low;
	else if( pQuality == MEDIUM )
		return medium;
	else if( pQuality == HIGH )
		return high;
	else {
		assert( !"Type of quality unknown in OptionsState" );
		return high;
	}
}

OptionsState::Quality OptionsState::getQuality( const std::string &pQuality ) const {
	if( pQuality == minimum )
		return MINIMUM;
	else if( pQuality == low )
		return LOW;
	else if( pQuality == medium )
		return MEDIUM;
	else if( pQuality == high )
		return HIGH;
	else {
		assert( !"Type of quality unknown in OptionsState" );
		return HIGH;
	}
}
