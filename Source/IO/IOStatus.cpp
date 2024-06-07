#include "IOStatus.h"
#include "Global/GlobalFunctions.h"
#include "Managers/DataLocatorManager.h"
#include "Util/StringRender/XBOXButtonsRender.h"
#include <fstream>
#include <cassert>

// ------------------------------------------------------------//
// CALLBACK Functions for create device input gamepad function //
// ------------------------------------------------------------//

BOOL CALLBACK EnumGamePadsCallback(const DIDEVICEINSTANCE* pdidInstance, VOID* pContext) {

	HRESULT hr;

	// Obtain an interface to the enumerated joystick.
	hr = IOStatus::deviceInputCreator->CreateDevice(pdidInstance->guidInstance,
		&(IOStatus::gamePad), NULL);

	// If it failed, then we can't use this joystick. (Maybe the user unplugged
	// it while we were in the middle of enumerating it.)
	if (FAILED(hr))
		return DIENUM_CONTINUE;

	// Stop enumeration. Note: we're just taking the first joystick we get. You
	// could store all the enumerated joysticks and let the user pick.
	return DIENUM_STOP;
}

BOOL CALLBACK ScaleAxisGamePadsCallback(const DIDEVICEOBJECTINSTANCE* pdidoi, VOID* pContext) {

	// For axes that are returned, set the DIPROP_RANGE property for the
	// enumerated axis in order to scale THUMB_MIN_VALUE/THUMB_MAX_VALUE values.
	if (pdidoi->dwType & DIDFT_AXIS)
	{
		DIPROPRANGE diprg;
		diprg.diph.dwSize = sizeof(DIPROPRANGE);
		diprg.diph.dwHeaderSize = sizeof(DIPROPHEADER);
		diprg.diph.dwHow = DIPH_BYID;
		diprg.diph.dwObj = pdidoi->dwType; // Specify the enumerated axis
		diprg.lMin = THUMB_MIN_VALUE;
		diprg.lMax = THUMB_MAX_VALUE;

		// Set the range for the axis
		if (FAILED(IOStatus::gamePad->SetProperty(DIPROP_RANGE, &diprg.diph)))
			return DIENUM_STOP;
	}

	return DIENUM_CONTINUE;
}

// ------------------------------------------------------------//

DeviceInputCreator IOStatus::deviceInputCreator = NULL;
DeviceInput IOStatus::gamePad = NULL;

IOStatus::IOStatus(void)
: numGamePadsXBOX360Connected(0)
, numGamePadsConnected(0)
, currentControl(NULL)
, outputStringStream( "" )
, isWriteXMLEnabled( false )
{
	// XBOX360 gamepad init state
	ZeroMemory( gamePadsXBOX360, sizeof(GamePadXBOX360)*MAX_CONTROLLERS_XBOX360 );

	// Init timeout for checking XBOX360 gamepads
	timeoutCheckGamePadXBOX360.init(TIME_CHECK_XBOX360);
}

void IOStatus::initiate(HWND hWnd) {

	// Read properpies of digital actions
	DataLocatorManager *dataLocatorManager = DataLocatorManager::getDataLocatorManager();
	const std::string &pathToXml = dataLocatorManager->getPath("xml");

	bool ok = xmlParseFile(pathToXml + "controls.xml");
	assert(ok);

	// Update XBOX360 gamepads state
	updateGamePadsXBOX360State(0.0f);

	// If a XBOX360 gamepad is plugged, not create a direct input gamepad
	if( numGamePadsXBOX360Connected==0 ) {

		// If deviceInputCreator is not NULL releaseDirectInput
		if(deviceInputCreator!=NULL)
			releaseDirectInput();

		// Create direct input
		if( deviceInputCreator==NULL) {
			HRESULT hr;
			if( FAILED( hr = createDirectInput(hWnd) ) ) {
				//MessageBox( NULL, "createDeviceInputCreator() failed. GamePad is disabled.",
				//	"DirectInput", MB_ICONERROR | MB_OK );
				assert( !"createDeviceInputCreator() failed. GamePad is disabled. IOStatus." );

				releaseDirectInput();
			}
		}
		else {
			//MessageBox( NULL, "deviceInputCreator() is busy. GamePad is disabled.", 
			//	"DirectInput", MB_ICONERROR | MB_OK );
			assert( !"createDeviceInputCreator() is busy. GamePad is disabled. IOStatus." );
		}
	}
}

bool IOStatus::detectButtonOnInput( InputDevices input, std::string &buttonPressed ) const {

	if( input == KEYBOARD /*|| input == MOUSE*/ ) {

		// Check if any number is pushed
		for( BYTE i=0x30; i<=0x39; ++i ) {
			if( Global::isPushed( i ) ) {
				buttonPressed = (char) i;
				return true;
			}
		}

		// Check if any letter is pushed
		for( BYTE i=0x41; i<=0x5A; ++i ) {
			if( Global::isPushed( i ) ) {
				char letter = (char) i;

				if( letter != 'A' && letter != 'S' && letter != 'W' && letter != 'D') {
					buttonPressed = letter;
					return true;
				}
			}
		}

		if ( Global::isPushed( VK_LBUTTON ) ) {
			buttonPressed = "MOUSE1";
			return true;
		}
		else if ( Global::isPushed( VK_RBUTTON ) ) {
			buttonPressed = "MOUSE2";
			return true;
		}
		else if ( Global::isPushed( VK_MBUTTON ) ) {
			buttonPressed = "MOUSE3";
			return true;
		}
		else if ( Global::isPushed( VK_SPACE ) ) {
			buttonPressed = "SPACE";
			return true;
		}
		else if ( Global::isPushed( VK_TAB ) ) {
			buttonPressed = "TAB";
			return true;
		}
		else if ( Global::isPushed( VK_BACK ) ) {
			buttonPressed = "BACK";
			return true;
		}
		else if ( Global::isPushed( VK_LSHIFT ) ) {
			buttonPressed = "LSHIFT";
			return true;
		}
		else if ( Global::isPushed( VK_RSHIFT ) ) {
			buttonPressed = "RSHIFT";
			return true;
		}
		else if ( Global::isPushed( VK_SHIFT ) ) {
			buttonPressed = "SHIFT";
			return true;
		}
		else if ( Global::isPushed( VK_LCONTROL ) ) {
			buttonPressed = "LCONTROL";
			return true;
		}
		else if ( Global::isPushed( VK_RCONTROL ) ) {
			buttonPressed = "RCONTROL";
			return true;
		}
		else if ( Global::isPushed( VK_CONTROL ) ) {
			buttonPressed = "CONTROL";
			return true;
		}
		else if ( Global::isPushed( VK_MENU ) ) {
			buttonPressed = "ALT";
			return true;
		}

	}
	else if( input == GAMEPAD  ) {
		if( numGamePadsConnected > 0 ) {
			for( int i=0; i<16; ++i ) {
				bool pressed = (gamePadData.buttons[ i ] & 0x80) ? true : false;
				if( pressed ) {
					char str[15];
					sprintf_s(str, sizeof( str ), "BUTTON%d", i);
					buttonPressed = str;
					return true;
				}
			}
		}
	}
	else if( input == XBOX360GAMEPAD  ) {
		// If any xbox360 gamepad is connected
		if( numGamePadsXBOX360Connected > 0 ) {
			XINPUT_KEYSTROKE inputKeyStroke;
			DWORD err = XInputGetKeystroke( XUSER_INDEX_ANY, 0, &inputKeyStroke );

			if( err == ERROR_SUCCESS ) {
				const std::string &key = XBOXButtonsRender::getKeyStringFromVK( inputKeyStroke.VirtualKey );

				if( key != "" ) {
					buttonPressed = key;
					return true;
				}
			}
		}
	}
	else {
		assert( !"Type of input unknow inside IOStatus" );
		return false;
	}

	return false;
}

void IOStatus::setButton( InputDevices input, Actions action, const std::string &buttonPressed ) {

	if( action == JUMP )
		jump.setButton( input, buttonPressed );
	else if( action == KICK )
		kick.setButton( input, buttonPressed );
	else if( action == WALK )
		walkSlow.setButton( input, buttonPressed );
	else if( action == PAUSE )
		start.setButton( input, buttonPressed );
	else {
		assert( !"Action unknown in IOStatus" );
		return;
	}
}

void IOStatus::update(float deltaTime) {

	// Update XBOX360 gamepads state
	updateGamePadsXBOX360State(deltaTime);

	// Update Direct input gamepad state
	updateGamePadsState();

	// Read data from FIRST XBOX360 gamepad
	readGamePadXBOX360Data();

	// Read data from Direct input gamepad
	readGamePadData();

	// Process all data
	processData(deltaTime);
}

HRESULT IOStatus::createDirectInput(HWND hWnd) {
	releaseDirectInput();

	HRESULT hr;

	// Create a DInput object
	if( FAILED( hr = createDeviceInputCreator(deviceInputCreator) ) )
		return hr;

	// Create a gamepad device
	if( FAILED( hr = createDeviceInputGamePad(hWnd) ) )
		return hr;

	return S_OK;
}

HRESULT IOStatus::createDeviceInputGamePad(HWND hWnd) {

	HRESULT hr;

	// Device Input Creator must be initialized
	if(deviceInputCreator==NULL)
		return DIERR_NOTINITIALIZED;

	// Look for a connected gamepad we can use
	if( FAILED( hr = deviceInputCreator->EnumDevices( GAMECONTROLLERS, EnumGamePadsCallback, 
		NULL, DIEDFL_ATTACHEDONLY ) ) )
		return hr;

	// Make sure we got a gamepad
	if( gamePad == NULL ) 
		return S_OK; // Gamepad not found

	// Set the data format to "simple joystick" - a predefined data format 
	//
	// A data format specifies which controls on a device we are interested in,
	// and how they should be reported. This tells DInput that we will be
	// passing a DIJOYSTATE2 structure to IDirectInputDevice::GetDeviceState().
	if( FAILED( hr = gamePad->SetDataFormat( &TYPEINPUTGAMEPAD ) ) )
		return hr;

	// Set the cooperative level to let DInput know how this device should
	// interact with the system and with other DInput applications.
	if( FAILED( hr = gamePad->SetCooperativeLevel( hWnd, DISCL_EXCLUSIVE | DISCL_FOREGROUND ) ) )
		return hr;

	// Enumerate the joystick objects. The callback function sets the min/max
	// values property for discovered axes.
	if( FAILED( hr = gamePad->EnumObjects( ScaleAxisGamePadsCallback, 
		(VOID*)hWnd, DIDFT_ALL ) ) )
		return hr;

	return S_OK;
}

void IOStatus::updateGamePadsState() {

	// If a XBOX360 gamepad is connected, do not use direct input gamepad
	if(gamePad == NULL || numGamePadsXBOX360Connected > 0) {
		numGamePadsConnected = 0;
		return;
	}

	// Poll the device to read the current state
	HRESULT hr;
	hr = gamePad->Poll(); 
	if( FAILED(hr) )  
	{
		// DInput is telling us that the input stream has been
		// interrupted. We aren't tracking any state between polls, so
		// we don't have any special reset that needs to be done. We
		// just re-acquire and try again.
		hr = gamePad->Acquire();
		if( hr == DIERR_INPUTLOST ) {
			hr = gamePad->Acquire();
		}

		// hr may be DIERR_OTHERAPPHASPRIO or other errors.  This
		// may occur when the app is minimized or in the process of 
		// switching, so just try again later 
		numGamePadsConnected = 0;
		return; 
	}

	// A direct input gamepad is connected
	numGamePadsConnected = 1;
}

void IOStatus::readGamePadData() {

	if(numGamePadsConnected > 0) {
		// Obtain all data
		HRESULT hr;
		InputGamePad inputGamePad;
		if( FAILED( hr = gamePad->GetDeviceState( sizeof(InputGamePad), &inputGamePad ) ) ) {
			numGamePadsConnected = 0; // The device should have been acquired
		}
		else {
			// If the data are readed correctly, fill the gamePadData struct
			if( DEAD_ZONE_ON ) {
				// Zero value if thumbsticks are within the dead zone 
				if( (inputGamePad.lX < INPUT_DEADZONE
					&& inputGamePad.lX > -INPUT_DEADZONE) && 
					(inputGamePad.lY < INPUT_DEADZONE
					&& inputGamePad.lY > -INPUT_DEADZONE) ) 
				{   
					inputGamePad.lX = 0;
					inputGamePad.lY = 0;
				}

				if( (inputGamePad.lZ < INPUT_DEADZONE
					&& inputGamePad.lZ > -INPUT_DEADZONE) && 
					(inputGamePad.lRz < INPUT_DEADZONE
					&& inputGamePad.lRz > -INPUT_DEADZONE) ) 
				{   
					inputGamePad.lZ = 0;
					inputGamePad.lRz = 0;
				}
			}

			for(int i=0; i<MAXBUTTONS; ++i)
				gamePadData.buttons[i] = inputGamePad.rgbButtons[i];

			gamePadData.thumbLeft.x = inputGamePad.lX;
			gamePadData.thumbLeft.y = -inputGamePad.lY;
			gamePadData.thumbRight.x = inputGamePad.lZ;
			gamePadData.thumbRight.y = -inputGamePad.lRz;
			Global::normalize(gamePadData.thumbLeft, gamePadData.thumbLeft, -1.0f, 1.0f, THUMB_MIN_VALUE, THUMB_MAX_VALUE);
			Global::normalize(gamePadData.thumbRight, gamePadData.thumbRight, -1.0f, 1.0f, THUMB_MIN_VALUE, THUMB_MAX_VALUE);
		}
	}

}

void IOStatus::updateGamePadsXBOX360State(float deltaTime) {

	static bool initiate=true;
	if(numGamePadsXBOX360Connected==0 && !timeoutCheckGamePadXBOX360.isFinished() && !initiate) {
		timeoutCheckGamePadXBOX360.update(deltaTime);
		numGamePadsXBOX360Connected=0;
	}
	else {
		initiate=false;
		timeoutCheckGamePadXBOX360.restart();

		numGamePadsXBOX360Connected=0;

		DWORD result;
		for (DWORD i=0; i<MAX_CONTROLLERS_XBOX360; ++i)
		{
			// Simply get the state of the controller from XInput.
			result = XInputGetState( i, &gamePadsXBOX360[i].state );

			if( result==ERROR_SUCCESS ) {
				gamePadsXBOX360[i].isConnected = true;
				numGamePadsXBOX360Connected++;
			}
			else
				gamePadsXBOX360[i].isConnected = false;        
		}
	}
}

void IOStatus::readGamePadXBOX360Data() {

	if(numGamePadsXBOX360Connected > 0) {

		for (DWORD i=0; i<MAX_CONTROLLERS_XBOX360; ++i)
		{
			if( gamePadsXBOX360[i].isConnected )
			{
				// Obtain all data
				XINPUT_GAMEPAD xInputGamePad = gamePadsXBOX360[i].state.Gamepad;

				if( DEAD_ZONE_XBOX360_ON ) {
					// Zero value if thumbsticks are within the dead zone 
					if( (xInputGamePad.sThumbLX < INPUT_DEADZONE_XBOX360
						&& xInputGamePad.sThumbLX > -INPUT_DEADZONE_XBOX360) && 
						(xInputGamePad.sThumbLY < INPUT_DEADZONE_XBOX360
						&& xInputGamePad.sThumbLY > -INPUT_DEADZONE_XBOX360) ) 
					{   
						xInputGamePad.sThumbLX = 0;
						xInputGamePad.sThumbLY = 0;
					}

					if( (xInputGamePad.sThumbRX < INPUT_DEADZONE_XBOX360
						&& xInputGamePad.sThumbRX > -INPUT_DEADZONE_XBOX360) && 
						(xInputGamePad.sThumbRY < INPUT_DEADZONE_XBOX360
						&& xInputGamePad.sThumbRY > -INPUT_DEADZONE_XBOX360) ) 
					{
						xInputGamePad.sThumbRX = 0;
						xInputGamePad.sThumbRY = 0;
					}
				}

				gamePadXBOX360Data.buttons = xInputGamePad.wButtons;

				gamePadXBOX360Data.leftTrigger = xInputGamePad.bLeftTrigger;
				gamePadXBOX360Data.rightTrigger = xInputGamePad.bRightTrigger;
				Global::normalize(gamePadXBOX360Data.leftTrigger, gamePadXBOX360Data.leftTrigger, -1.0f, 1.0f, TRIGGER_XBOX360_MIN_VALUE, TRIGGER_XBOX360_MAX_VALUE);
				Global::normalize(gamePadXBOX360Data.rightTrigger, gamePadXBOX360Data.rightTrigger, -1.0f, 1.0f, TRIGGER_XBOX360_MIN_VALUE, TRIGGER_XBOX360_MAX_VALUE);

				gamePadXBOX360Data.thumbLeft.x = xInputGamePad.sThumbLX;
				gamePadXBOX360Data.thumbLeft.y = xInputGamePad.sThumbLY;
				gamePadXBOX360Data.thumbRight.x = xInputGamePad.sThumbRX;
				gamePadXBOX360Data.thumbRight.y = xInputGamePad.sThumbRY;
				Global::normalize(gamePadXBOX360Data.thumbLeft, gamePadXBOX360Data.thumbLeft, -1.0f, 1.0f, THUMB_XBOX360_MIN_VALUE, THUMB_XBOX360_MAX_VALUE);
				Global::normalize(gamePadXBOX360Data.thumbRight, gamePadXBOX360Data.thumbRight, -1.0f, 1.0f, THUMB_XBOX360_MIN_VALUE, THUMB_XBOX360_MAX_VALUE);

				// We have the data from FIRST XBOX360 gamedpad, so return.
				return;
			}
		}

	}
}

void IOStatus::processData(float deltaTime) {

	// Digital actions update
	updateDigitalAction(jump, deltaTime);
	updateDigitalAction(kick, deltaTime);
	updateDigitalAction(start, deltaTime);
	updateDigitalAction(selectOption, deltaTime);
	updateDigitalAction(enterOptions, deltaTime);
	updateDigitalAction(exitOptions, deltaTime);
	updateDigitalAction(walkSlow, deltaTime);

	updateDigitalAction(walkUp, deltaTime);
	updateDigitalAction(walkDown, deltaTime);
	updateDigitalAction(walkRight, deltaTime);
	updateDigitalAction(walkLeft, deltaTime);

	// Analog actions update
	D3DXVECTOR2 direction(0,0);

	if( walkUp.isPressed() )
		direction.y+=1.0;
	if( walkDown.isPressed() )
		direction.y+=-1.0;
	if( walkRight.isPressed() )
		direction.x+=1.0;
	if( walkLeft.isPressed() )
		direction.x+=-1.0;

	if( numGamePadsXBOX360Connected > 0 )
		direction += gamePadXBOX360Data.thumbLeft;
	if( numGamePadsConnected > 0)
		direction += gamePadData.thumbLeft;

	direction.x = Global::clamp( direction.x, -1.0f, 1.0f );
	direction.y = Global::clamp( direction.y, -1.0f, 1.0f );

	walk.update(direction);

	// Set WalkSlown action from direction length
	float length = D3DXVec2Length(&direction);
	if(length!=0.0f && length<0.95f)
		walkSlow.update( true );
}

void IOStatus::updateDigitalAction(DigitalAction &digitalAction, float deltaTime) {
	bool isPressed = false;

	const DigitalActionConfigKeys &configKeys = digitalAction.getDigitalActionConfigKeys();
	DigitalActionConfigKeys::const_iterator it = configKeys.begin();

	while( it != configKeys.end() ) {
		switch ((*it)->device) {
			case KEYBOARD:
				isPressed = isPressed || Global::isPushed((*it)->bitMask);
				break;
				/*
				case MOUSE:
				isPressed = isPressed || Global::isPushed((*it)->bitMask);
				break;
				*/
			case XBOX360GAMEPAD:
				if( numGamePadsXBOX360Connected > 0 ) {
					bool pressed = false;

					if( (*it)->bitMask == VK_PAD_LTRIGGER )
						pressed = (gamePadXBOX360Data.leftTrigger > 0.7f );
					else if( (*it)->bitMask == VK_PAD_RTRIGGER )
						pressed = (gamePadXBOX360Data.rightTrigger > 0.7f );
					else
						pressed = ( gamePadXBOX360Data.buttons & (*it)->bitMask ) ? true : false;

					isPressed = isPressed || pressed;
					// isPressed |= ( gamePadXBOX360Data.buttons & (*it)->bitMask );
				}
				break;
			case GAMEPAD:
				if( numGamePadsConnected > 0 ) {
					bool pressed = (gamePadData.buttons[ (*it)->bitMask ] & 0x80) ? true : false;
					isPressed = isPressed || pressed;
				}
				break;
			default:
				assert( !"This should not happend" );
				return;
				break;
		}
		++it;
	}

	digitalAction.update(isPressed, deltaTime);
}

void IOStatus::onStartElement (const std::string &elem, MKeyValue &atts) {

	if( currentControl ) {
		currentControl->onStartElement( elem, atts, outputStringStream, isWriteXMLEnabled );
		return;
	}

	if( elem=="control" ) {

		std::string action=atts["action"];
		if(action=="jump") {
			if( isWriteXMLEnabled )
				outputStringStream << "<!-- Jump Action -->\n";
			currentControl = &jump;
		}
		else if (action=="kick") {
			if( isWriteXMLEnabled )
				outputStringStream << "<!-- Kick Action -->\n";
			currentControl = &kick;
		}
		else if (action=="walk_slow") {
			if( isWriteXMLEnabled )
				outputStringStream << "<!-- Walk Slown Action -->\n";
			currentControl = &walkSlow;
		}
		else if (action=="walk_up") {
			if( isWriteXMLEnabled )
				outputStringStream << "<!-- Walk Up with Keyboard Action -->\n";
			currentControl = &walkUp;
		}
		else if (action=="walk_down") {
			if( isWriteXMLEnabled )
				outputStringStream << "<!-- Walk Down with Keyboard Action -->\n";
			currentControl = &walkDown;
		}
		else if (action=="walk_right") {
			if( isWriteXMLEnabled )
				outputStringStream << "<!-- Walk Right with Keyboard Action -->\n";
			currentControl = &walkRight;
		}
		else if (action=="walk_left") {
			if( isWriteXMLEnabled )
				outputStringStream << "<!-- Walk Left with Keyboard Action -->\n";
			currentControl = &walkLeft;
		}
		else if (action=="start") {
			if( isWriteXMLEnabled )
				outputStringStream << "<!-- Start -->\n";
			currentControl = &start;
		}
		else if (action=="select_option") {
			if( isWriteXMLEnabled )
				outputStringStream << "<!-- Select Option -->\n";
			currentControl = &selectOption;
		}
		else if (action=="enter_options") {
			if( isWriteXMLEnabled )
				outputStringStream << "<!-- Enter Options -->\n";
			currentControl = &enterOptions;
		}
		else if (action=="exit_options") {
			if( isWriteXMLEnabled )
				outputStringStream << "<!-- Exit Options -->\n";
			currentControl = &exitOptions;
		}
		else {
			assert( !"Digital action unknown in IOStatus" );
			return;
		}

		if( isWriteXMLEnabled )
			atts.writeStartElement( outputStringStream, elem.c_str() );
	}
}

void IOStatus::onEndElement (const std::string &elem) {
	if( currentControl && elem=="control" ) {
		if( isWriteXMLEnabled )
			outputStringStream << "</control>\n";

		currentControl = NULL; 
	}
}

void IOStatus::writeXML( ) {
	isWriteXMLEnabled = true;

	// Read properpies of digital actions
	DataLocatorManager *dataLocatorManager = DataLocatorManager::getDataLocatorManager();
	const std::string &pathToXml = dataLocatorManager->getPath("xml");
	std::string controlsFile = pathToXml + "controls.xml";

	// Insert the header in the string output stream
	outputStringStream << "<?xml version=\"1.0\" encoding=\"iso-8859-1\"?>\n";
	outputStringStream << "<controls>\n";

	bool ok = xmlParseFile( controlsFile );
	assert(ok);

	// Insert the final in the string output stream
	outputStringStream << "</controls>\n";

	// Save the string output stream to a file
	std::ofstream xmlStream;
	xmlStream.open( controlsFile.c_str() );
	xmlStream << outputStringStream.str();
	xmlStream.close();

	// Clear the string output stream
	outputStringStream.str("");

	isWriteXMLEnabled = false;
}

//-----------------------------------------------------------------------------
// Name: releaseDirectInput()
// Desc: Release the DirectInput variables.
//-----------------------------------------------------------------------------
void IOStatus::releaseDirectInput() {

	// Unacquire the device one last time just in case 
	// the app tried to exit while the device is still acquired.
	if( gamePad ) 
		gamePad->Unacquire();

	// Release any DirectInput objects
	if( gamePad )
		gamePad->Release(), gamePad=NULL;

	if( deviceInputCreator )
		deviceInputCreator->Release(), deviceInputCreator=NULL;
}
