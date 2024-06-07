#ifndef END_BOILER_STATE_H
#define END_BOILER_STATE_H

#include <Windows.h>
#include "GameState/IGameState.h"

/**
* End of Boiler planet game state
*/

class EndBoilerState : public IGameState
{
protected:
	static std::string name;

public:
	EndBoilerState() : IGameState() {};
	~EndBoilerState()
	{
		OutputDebugString("Destroying EndBoilerState...\n");
	};

	void update( float deltaTime=(1.0f/60.0f) );
	void onLoad( );
	void onUnload( );
	const std::string & getName( ) const { return name; };

protected:
	void preRender( );
	void renderWorld3D( );
	void renderWorld2D( );
	void renderPostProcessed2D( );
	void renderGameUI( );
	void postRender( );

	void renderFrame( );
};

#endif // END_BOILER_STATE_H
