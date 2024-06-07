#ifndef IURANUS_STATE_H
#define IURANUS_STATE_H

#include <Windows.h>
#include "GameState/IGameState.h"
#include "Util/Timeout/Timeout.h"
#include "UI/IuranusUI.h"

/**
* Iuranus game state
*/

class IuranusState : public IGameState
{
protected:
	static std::string name;

	// Timeout for do the update of the space (0.1 seconds)
	Timeout timeoutUpdateSpace;

public:
	// UI
	IuranusUI iuranusUI;
	bool doOnLoad;

private:
	bool firstUpdateSpace;

	const static float cameraDistanceNomal;
	const static float cameraPositionSpeedNomal;
	const static float cameraTargetSpeedNomal;

	// Get time for debugging variables
	unsigned long updateTimeStart;
	unsigned long updateTimeEnd;

	unsigned long updateCameraFrustumTimeStart;
	unsigned long updateCameraFrustumTimeEnd;

	unsigned long updatePlayerTimeStart;
	unsigned long updatePlayerTimeEnd;

	unsigned long updateAITimeStart;
	unsigned long updateAITimeEnd;

	unsigned long updateAnimationTimeStart;
	unsigned long updateAnimationTimeEnd;

	unsigned long updateAtmosphereParticlesTimeStart;
	unsigned long updateAtmosphereParticlesTimeEnd;

	unsigned long updateSpaceTimeStart;
	unsigned long updateSpaceTimeEnd;

	unsigned long updateCollisionTimeStart;
	unsigned long updateCollisionTimeEnd;

	unsigned long updateLogicTimeStart;
	unsigned long updateLogicTimeEnd;

	unsigned long renderPlayerTimeStart;
	unsigned long renderPlayerTimeEnd;

	unsigned long renderPlanetsTimeStart;
	unsigned long renderPlanetsTimeEnd;

	unsigned long renderEnemiesTimeStart;
	unsigned long renderEnemiesTimeEnd;

	unsigned long renderPlanetElementsTimeStart;
	unsigned long renderPlanetElementsTimeEnd;

	unsigned long renderStaticObjectsTimeStart;
	unsigned long renderStaticObjectsTimeEnd;

	unsigned long renderAnimatedObjectsTimeStart;
	unsigned long renderAnimatedObjectsTimeEnd;

	unsigned long render2DTimeStart;
	unsigned long render2DTimeEnd;

	unsigned long renderUITimeStart;
	unsigned long renderUITimeEnd;

	float updateDeltaTime;

public:
	IuranusState()
		: IGameState()
		, timeoutUpdateSpace(0.1f)
		, doOnLoad(true)
		, firstUpdateSpace(true)
	{
	};

	~IuranusState()
	{
		OutputDebugString("Destroying IuranusState...\n");
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

#endif // IURANUS_STATE_H
