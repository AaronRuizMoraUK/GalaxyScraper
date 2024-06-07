#ifndef IGAME_STATE_H
#define IGAME_STATE_H

#include <Windows.h>
#include <string>
#include "Object/Object.h"

/**
* Game state
*/

class IGameState
{

public:
	static const int numStates = 25;
	static IGameState *gameStates[numStates];
	static IGameState *currentState;

private:
	// Get time for debugging variables
	unsigned long renderTimeStart;
	unsigned long renderTimeEnd;

public:
	IGameState() {};
	virtual ~IGameState()
	{
		OutputDebugString("Destroying IGameState...\n");
	};

	void render( );
	virtual void update( float deltaTime=(1.0f/60.0f) ) = 0;
	virtual void onLoad( ) = 0;
	virtual void onUnload( ) = 0;
	virtual const std::string & getName( ) const = 0;

	void getFocus( );

	static IGameState * getState( const std::string &name );

	static void showLoadingScreen( );

	static void load();				// Used in game.cpp
	static void reloadShaders();	// Used in game.cpp

protected:
	virtual void loadObjects() {};
	virtual void reloadShader() {};

	virtual void beginRenderFrame();
	virtual void endRenderFrame();
	virtual void beginRenderTexture();
	virtual void endRenderTexture();

	virtual void preRender( ) = 0;
	virtual void renderWorld3D( ) = 0;
	virtual void renderWorld2D( ) = 0;
	virtual void renderPostProcessed2D( ) = 0;
	virtual void renderGameUI( ) = 0;
	virtual void postRender( ) = 0;

	virtual void renderFrame( ) = 0;

	bool isVisible(const Object &object);
};

#endif // IGAME_STATE_H
