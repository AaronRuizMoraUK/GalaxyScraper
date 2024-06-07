#ifndef TEST_BOILER_STATE_H
#define TEST_BOILER_STATE_H

#include "IGameState.h"
#include "Util/Timeout/Timeout.h"
#include "Util/StringRender/UINumberRender.h"
#include "Util/StringRender/TimeOutNumberRender.h"
#include "Util/StringRender/AlphabetRender.h"
#include "Util/StringRender/XBOXButtonsRender.h"
#include "Util/StringRender/PadButtonsRender.h"

/**
* Test Boiler state
*/

class TestBoilerState : public IGameState
{
protected:
	static std::string name;

	// Timeout for do the update of the space (0.1 seconds)
	Timeout timeoutUpdateSpace;

	// Lava distorsion object (use render to texture)
	Object lavaDistorsion; // Initialized in function initGlobals in Game.cpp

	UINumberRender uiNumberRender;
	TimeOutNumberRender timeOutNumberRender;
	AlphabetRender alphabetRender;
	XBOXButtonsRender xboxButtonsRender;
	PadButtonsRender padButtonsRender;

private:
	bool firstUpdateSpace;

public:
	TestBoilerState();
	~TestBoilerState();

	void update( float deltaTime=(1.0f/60.0f) );
	void onLoad( );
	void onUnload( );
	const std::string & getName( ) const { return name; };

protected:
	void loadObjects( );
	void reloadShader( );

	void preRender( );
	void renderWorld3D( );
	void renderWorld2D( );
	void renderPostProcessed2D( );
	void renderGameUI( );
	void postRender( );

	void renderFrame( );

private:
	void updateCameraValues( float deltaTime=(1.0f/60.0f) );
};

#endif // TEST_BOILER_STATE_H
