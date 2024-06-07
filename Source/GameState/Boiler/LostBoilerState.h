#ifndef LOST_BOILER_STATE_H
#define LOST_BOILER_STATE_H

#include <Windows.h>
#include "GameState/IGameState.h"
#include "Object/Object.h"
#include "Util/StringRender/AlphabetRender.h"

/**
* Player lost in Boiler game state
*/

class LostBoilerState : public IGameState
{
protected:
	static std::string name;

	Object arrow;
	AlphabetRender alphabetRender;

	// Flowers pictures
	Object flowerUp;

	bool enter_lost;
	bool exit_lost;

	bool onRetry; // If true the selection icon is on Retry, else is on Back To Menu
	const static D3DXVECTOR3 onRetryPosition;
	static D3DXVECTOR3 onBackToMenuPosition;

private:
	const static float cameraDistanceNomal;
	const static float cameraPositionSpeedNomal;
	const static float cameraTargetSpeedNomal;
	
	const static float cameraDistanceLose;
	const static float cameraPositionSpeedLose;
	const static float cameraTargetSpeedLose;

public:
	LostBoilerState()
		: IGameState()
		, enter_lost(true)
		, exit_lost(false)
		, onRetry(true)
	{
	};

	~LostBoilerState()
	{
		OutputDebugString("Destroying LostBoilerState...\n");
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

#endif // LOST_BOILER_STATE_H
