#ifndef IURANUS_UI_H
#define IURANUS_UI_H

#include "Object/Object.h"
#include "Util/StringRender/UINumberRender.h"
#include "Util/StringRender/TimeOutNumberRender.h"
//#include "Util/StringRender/AlphabetRender.h"
#include "Util/Fade/TextureFade.h"
#include "Util/Timeout/Timeout.h"

/**
* This class make the User Interface for
* Iuranus planet
*/

class IuranusUI
{
protected:
	Object timeLeftPicture;
	UINumberRender timeLeft;
	int planetTimeOut;

	Object numEnemiesLeftPicture;
	UINumberRender numEnemiesLeft;

	TimeOutNumberRender timeOutNumberRender;
	int timeOut;

	static const float showWelcomeMsgTime;
	static const float showWelcomeMsgFadeTime;
	float welcomeMessageAlpha;
	Timeout timeoutShowWelcomeMsg;
	//AlphabetRender welcomeMessage;
	TextureFade welcomeMessage;

public:
	IuranusUI( );
	~IuranusUI( );

	void create( );

	void showWelcomeMsg( );

	void update( float deltaTime=(1.0f/60.0f) );
	void render( );

	void reloadShader( );
};

#endif // IURANUS_UI_H
