#include "DigitalAction.h"
#include "Global/GlobalFunctions.h"
#include "Util/StringRender/XBOXButtonsRender.h"
#include <cassert>

std::string keyString = "";

const std::string & DigitalActionConfigKeys::getKeyboardConfigKey( ) const {
	const_iterator it = begin( );
	while( it != end( ) ) {
		if( (*it)->device == KEYBOARD /*|| (*it)->device == MOUSE*/ ) {
			keyString = (*it)->name;
			return keyString;
		}

		++it;
	}

	keyString = "";
	return keyString;
}

int DigitalActionConfigKeys::getGamePadConfigKey( ) const {
	int keyInt = -1;

	const_iterator it = begin( );
	while( it != end( ) ) {
		if( (*it)->device == GAMEPAD ) {
			keyInt = (*it)->bitMask;
			return keyInt;
		}

		++it;
	}

	return keyInt;
}

const std::string & DigitalActionConfigKeys::getXBOX360GamePadConfigKey( ) const {
	const_iterator it = begin( );
	while( it != end( ) ) {
		if( (*it)->device == XBOX360GAMEPAD ) {
			keyString = XBOXButtonsRender::getKeyString( (*it)->name );
			return keyString;
		}

		++it;
	}

	keyString = "";
	return keyString;
}

// ------------------------------------------------------------

DigitalAction::DigitalAction() : isPressedNow(false)
, wasPressed(false)
, timePressed(0.0f)
{
}

bool DigitalAction::isPressed() const {
	return isPressedNow;
}

bool DigitalAction::isReleased() const {
	return (!isPressedNow);
}

bool DigitalAction::becomesPressed() const {
	return (isPressedNow && !wasPressed);
}

bool DigitalAction::becomesReleased() const {
	return (!isPressedNow && wasPressed);
}

void DigitalAction::update(bool isPressed, float deltaTime){
	wasPressed = isPressedNow;
	isPressedNow = isPressed;

	if( isPressedNow && !wasPressed)
		timePressed=0.0f;
	else if ( isPressedNow && wasPressed)
		timePressed += deltaTime;
}

void DigitalAction::onStartElement (const std::string &elem, MKeyValue &atts, std::ostringstream &outputStringStream, bool isWriteXMLEnabled) {

	if (elem=="config") {

		if( isWriteXMLEnabled ) {
			std::string device=atts["device"];

			InputDevices inputDevice;
			if(device=="keyboard") {
				std::string key=atts["key"];
				if( key == "W" || key == "S" || key == "D" || key == "A" ||
					key == "UP" || key == "DOWN" || key == "RIGHT" || key == "LEFT" ) {
					atts.writeSingle( outputStringStream, elem.c_str() );
					return;
				}

				inputDevice = KEYBOARD;
			}
			else if(device=="gamepad")
				inputDevice = GAMEPAD;
			else if(device=="xbox360_gamepad")
				inputDevice = XBOX360GAMEPAD;
			else {
				assert( !"Device unknown in DigitalAction" );
				return;
			}

			DigitalActionConfigKeys::const_iterator it = digitalActionConfigKeys.begin( );
			while( it != digitalActionConfigKeys.end( ) ) {
				if( (*it)->device == inputDevice )
					break;
				++it;
			}
			assert( it != digitalActionConfigKeys.end( ) );

			atts.put( "key", (*it)->name.c_str( ) );

			atts.writeSingle( outputStringStream, elem.c_str() );
		}
		else {
			DigitalActionConfigKey *configKey = new DigitalActionConfigKey();

			std::string device=atts["device"];

			std::string key=atts["key"];
			if(key=="") {
				if(configKey)
					delete configKey, configKey=NULL;
				return;
			}

			configKey->name = key;
			if(device=="keyboard") {
				configKey->device=KEYBOARD;
				configKey->bitMask = Global::getKeyboardBitMask(key);
			}
			/*
			else if (device=="mouse") {
			configKey->device=MOUSE;
			configKey->bitMask = Global::getMouseBitMask(key);
			}
			*/
			else if (device=="gamepad") {
				configKey->device=GAMEPAD;
				configKey->bitMask = Global::getGamepadBitMask(key);
			}
			else if (device=="xbox360_gamepad") {
				configKey->device=XBOX360GAMEPAD;
				configKey->bitMask = Global::getXBOX360GamepadBitMask(key);
			}
			else {
				if(configKey)
					delete configKey, configKey=NULL;
				return;
			}

			if(configKey->bitMask==-1) {
				if(configKey)
					delete configKey, configKey=NULL;
				return;
			}

			/*char msg[ 128 ];
			sprintf_s(msg, sizeof(msg), "BitMask %s is 0x%02x\n", key, configKey->bitMask);
			OutputDebugString(msg);*/

			digitalActionConfigKeys.push_back(configKey);
		}
	}
}

void DigitalAction::setButton( InputDevices input, const std::string &buttonPressed ) {
	DigitalActionConfigKeys::iterator it = digitalActionConfigKeys.begin( );

	while( it != digitalActionConfigKeys.end( ) ) {
		DigitalActionConfigKey *configKey = (*it);

		if( configKey->device == input ) {
			configKey->name = buttonPressed;

			if( input == KEYBOARD ) {
				configKey->bitMask = Global::getKeyboardBitMask( buttonPressed );
			}
			else if( input == GAMEPAD ) {
				configKey->bitMask = Global::getGamepadBitMask( buttonPressed );
			}
			else if( input == XBOX360GAMEPAD ) {
				configKey->bitMask = Global::getXBOX360GamepadBitMask( buttonPressed );
			}
			else {
				assert( !"Input unknown in DigitalAction" );
				return;
			}

			return;
		}

		++it;
	}

	// If we are here means that kind of input is not defined here, so add it
	DigitalActionConfigKey *configKey = new DigitalActionConfigKey();
	configKey->device = input;
	configKey->name = buttonPressed;

	if( input == KEYBOARD ) {
		configKey->bitMask = Global::getKeyboardBitMask( buttonPressed );
	}
	else if( input == GAMEPAD ) {
		configKey->bitMask = Global::getGamepadBitMask( buttonPressed );
	}
	else if( input == XBOX360GAMEPAD ) {
		configKey->bitMask = Global::getXBOX360GamepadBitMask( buttonPressed );
	}
	else {
		assert( !"Input unknown in DigitalAction" );
		return;
	}
}
