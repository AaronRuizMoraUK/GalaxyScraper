#ifndef BOILER_UI_H
#define BOILER_UI_H

#include "Object/Object.h"
#include "Util/StringRender/UINumberRender.h"
#include "Util/Fade/TextureFade.h"
#include "Util/Timeout/Timeout.h"

/**
* This class make the User Interface for
* Boiler planet
*/

class BoilerUI
{
protected:
	Object timeLeftPicture;
	UINumberRender timeLeft;
	int planetTimeOut;

	static const float showWelcomeMsgTime;
	static const float showWelcomeMsgFadeTime;
	float welcomeMessageAlpha;
	Timeout timeoutShowWelcomeMsg;
	TextureFade welcomeMessage;

	// Player lava falling faces
	const static D3DXVECTOR2 initialFacePosition;
	const static float faceWidth;
	const static float nextFaceBias;
	int lastTryingLeft;
	Object playerSmile;
	Object playerFired;

	const static float playerFiredTime;
	Timeout playerFiredTimeout;
	bool showFiredFace;

public:
	BoilerUI( );
	~BoilerUI( );

	void create( );

	void showWelcomeMsg( );

	void update( float deltaTime=(1.0f/60.0f) );
	void render( );

	void reloadShader( );
};

#endif // BOILER_UI_H
