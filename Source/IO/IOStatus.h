#ifndef IO_STATUS_H
#define IO_STATUS_H

#include "Directx/DirectInput.h"
#include "Directx/DirectXInput.h"
#include "IO/DigitalAction/DigitalAction.h"
#include "IO/AnalogAction/AnalogAction.h"
#include "Util/Timeout/Timeout.h"
#include "XMLParser/XMLParser.h"
#include <sstream>


/**
* Input/Output status. This stores the status of
* the input devices in digital and analog actions.
*/

//NOTE: The DirectInput gamepads only works if they are plugged
//      before the game runs.

typedef enum ActionsEnum {JUMP, KICK, WALK, PAUSE, NUM_ACTIONS} Actions;

class IOStatus : public CXMLParser
{
public:
	DigitalAction jump;
	DigitalAction kick;
	DigitalAction start;
	DigitalAction selectOption;
	DigitalAction enterOptions;
	DigitalAction exitOptions;

	DigitalAction walkSlow;  // Only from keyboard

	AnalogAction walk;

protected:
	// Digital actions for analog actions
	DigitalAction walkUp;    // Only from keyboard
	DigitalAction walkDown;  // Only from keyboard
	DigitalAction walkRight; // Only from keyboard
	DigitalAction walkLeft;  // Only from keyboard

private:
	// Used to write in xml
	std::ostringstream outputStringStream;
	bool isWriteXMLEnabled;

public:
	IOStatus();
	~IOStatus() {
		OutputDebugString("Destroying IO status...\n");
		releaseDirectInput();
	};

	void initiate(HWND hWnd);

	void update(float deltaTime=(1.0f/60.0f));

	void onStartElement (const std::string &elem, MKeyValue &atts);
	void onEndElement (const std::string &elem);
	void writeXML( );

	bool detectButtonOnInput( InputDevices input, std::string &buttonPressed ) const;
	void setButton( InputDevices input, Actions action, const std::string &buttonPressed );

private:
	static DeviceInputCreator deviceInputCreator;
	static DeviceInput gamePad;
	GamePadXBOX360 gamePadsXBOX360[MAX_CONTROLLERS_XBOX360];

	GamePadXBOX360Data gamePadXBOX360Data;
	GamePadData gamePadData;
	
	int numGamePadsXBOX360Connected;
	int numGamePadsConnected;

	DigitalAction *currentControl; // This is used just for reading XML

	Timeout timeoutCheckGamePadXBOX360;

private:
	HRESULT createDirectInput(HWND hWnd);
	HRESULT createDeviceInputGamePad(HWND hWnd);
	void releaseDirectInput();

	void updateGamePadsState();
	void readGamePadData();

	void updateGamePadsXBOX360State(float deltaTime=(1.0f/60.0f));
	void readGamePadXBOX360Data();

	void processData(float deltaTime=(1.0f/60.0f));
	void updateDigitalAction(DigitalAction &digitalAction, float deltaTime=(1.0f/60.0f));
	
	friend BOOL CALLBACK EnumGamePadsCallback( const DIDEVICEINSTANCE* pdidInstance, VOID* pContext );
	friend BOOL CALLBACK ScaleAxisGamePadsCallback( const DIDEVICEOBJECTINSTANCE* pdidoi, VOID* pContext );
};

#endif //IO_STATUS_H
