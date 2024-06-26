#ifndef PAUSE_IURANUS_STATE_H
#define PAUSE_IURANUS_STATE_H

#include <Windows.h>
#include "GameState/IGameState.h"
#include "Object/Object.h"
#include "Util/StringRender/AlphabetRender.h"

/**
* Pause in Iuranus game state
*/

class PauseIuranusState : public IGameState
{
protected:
	static std::string name;

	Object arrow;
	AlphabetRender alphabetRender;

	// Flowers pictures
	Object flowerUp;
	Object flowerDown;

	bool enter_pause;
	bool exit_pause;

	bool onContinue; // If true the selection icon is on Continue, else is on Back To Menu
	const static D3DXVECTOR3 onContinuePosition;
	static D3DXVECTOR3 onBackToMenuPosition;

public:
	PauseIuranusState()
		: IGameState()
		, enter_pause(true)
		, exit_pause(false)
		, onContinue(true)
	{
	};

	~PauseIuranusState()
	{
		OutputDebugString("Destroying PauseIuranusState...\n");
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

#endif // PAUSE_IURANUS_STATE_H
