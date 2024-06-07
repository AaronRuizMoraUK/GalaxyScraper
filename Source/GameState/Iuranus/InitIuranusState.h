#ifndef INIT_IURANUS_STATE_H
#define INIT_IURANUS_STATE_H

#include <Windows.h>
#include "GameState/IGameState.h"

/**
* Init Iuranus game state
*/

class InitIuranusState : public IGameState
{
protected:
	static std::string name;

public:
	InitIuranusState() : IGameState() {};
	~InitIuranusState()
	{
		OutputDebugString("Destroying InitIuranusState...\n");
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

#endif // INIT_IURANUS_STATE_H
