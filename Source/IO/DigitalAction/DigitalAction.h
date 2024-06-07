#ifndef DIGITAL_ACTION_H
#define DIGITAL_ACTION_H

#include <Windows.h>
#include <vector>
#include "XMLParser/XMLParser.h"
#include <sstream>

enum InputDevices {KEYBOARD, /*MOUSE,*/ GAMEPAD, XBOX360GAMEPAD};

/**
* A configuration in an input device for a digital action
*/

typedef struct DigitalActionConfigKeyStruct {
	InputDevices device;
	int          bitMask;
	std::string  name;
} DigitalActionConfigKey;

class DigitalActionConfigKeys : public std::vector<DigitalActionConfigKey *> {
public:
	const std::string & getKeyboardConfigKey( ) const;
	int getGamePadConfigKey( ) const;
	const std::string & getXBOX360GamePadConfigKey( ) const;
};

/**
* This is a digital action, press a button for example.
*/

class DigitalAction
{
protected:
	bool isPressedNow;
	bool wasPressed;
	float timePressed;

	DigitalActionConfigKeys digitalActionConfigKeys;

public:
	DigitalAction();
	~DigitalAction() {
		OutputDebugString("Destroying Digital Action...\n");

		// Destroy Digital Action Config Keys
		while(!digitalActionConfigKeys.empty()) {
			unsigned int size = digitalActionConfigKeys.size()-1;
			DigitalActionConfigKey * configKey= digitalActionConfigKeys[size];
			if(configKey)
				delete configKey, configKey=NULL;
			digitalActionConfigKeys.pop_back();
		}
	};

	const DigitalActionConfigKeys & getDigitalActionConfigKeys() const {
		return digitalActionConfigKeys;
	};

	bool isPressed() const;
	bool isReleased() const;
	bool becomesPressed() const;
	bool becomesReleased() const;

	void update(bool isPressed, float deltaTime=(1.0f/60.0f));

	void onStartElement (const std::string &elem, MKeyValue &atts, std::ostringstream &outputStringStream, bool isWriteXMLEnabled=false);

	void setButton( InputDevices input, const std::string &buttonPressed );
};

#endif //DIGITAL_ACTION_H
