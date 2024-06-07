#ifndef END_GAME_IURANUS_STATE_H
#define END_GAME_IURANUS_STATE_H

#include <Windows.h>
#include "GameState/IGameState.h"
#include "Object/Object.h"
#include "Util/Fade/TextureFade.h"
#include "Util/Timeout/Timeout.h"

/**
* "End of game" game state
*/

class EndGameIuranusState : public IGameState
{
protected:
	static std::string name;

	bool enter_credits;
	bool exit_credits;

	TextureFade toBeContinuedFade;
	TextureFade designersFade;
	TextureFade programmersFade;
	TextureFade artistsFade;
	TextureFade soundFade;

	float toBeContinuedAlpha;
	float designersAlpha;
	float programmersAlpha;
	float artistsAlpha;
	float soundAlpha;

	bool toBeContinuedFirstTime;
	bool designersFirstTime;
	bool programmersFirstTime;
	bool artistsFirstTime;
	bool soundFirstTime;
	bool finalFirstTime;

	const static float creditsTime;
	const static float toBeContinuedTime;
	const static float designersTime;
	const static float programmersTime;
	const static float artistsTime;
	const static float soundTime;
	const static float fadeTime;

	Timeout timeout;

public:
	EndGameIuranusState()
		: IGameState()
		, enter_credits(true)
		, exit_credits(false)
		, toBeContinuedAlpha( 0.0f )
		, designersAlpha( 0.0f )
		, programmersAlpha( 0.0f )
		, artistsAlpha( 0.0f )
		, soundAlpha( 0.0f )
		, toBeContinuedFirstTime( true )
		, designersFirstTime( true )
		, programmersFirstTime( true )
		, artistsFirstTime( true )
		, soundFirstTime( true )
		, finalFirstTime( true )
	{
	};
	~EndGameIuranusState()
	{
		OutputDebugString("Destroying EndGameIuranusState...\n");
	};

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
};

#endif // END_GAME_IURANUS_STATE_H
