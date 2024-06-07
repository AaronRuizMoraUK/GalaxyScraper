#ifndef BOILER_STATE_H
#define BOILER_STATE_H

#include <Windows.h>
#include "GameState/IGameState.h"
#include "Util/Timeout/Timeout.h"
#include "UI/BoilerUI.h"

/**
* Boiler game state
*/

class BoilerState : public IGameState
{
protected:
	static std::string name;

	float flickeringTempus;
	float flickeringTime;
	//Timeout flickeringTimeout;
	//Timeout recoverPlayerControlOnDeath;

	bool die;

public:
	// Lava distorsion object (use render to texture)
	Object lavaDistorsion; // Initialized in function initGlobals in Game.cpp

	// UI
	BoilerUI boilerUI;
	bool doOnLoad;

	static const float boilerCameraDistance; 
	static const float boilerCameraPositionSpeed; 
	static const float boilerCameraTargetSpeed; 

	const static float cameraVibrationTime;
	static float cameraVibrationCurrentTime;
	static bool doAnyVibration;

	bool drawAtmospheres;

private:
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

	unsigned long updateCollisionTimeStart;
	unsigned long updateCollisionTimeEnd;

	unsigned long updateLogicTimeStart;
	unsigned long updateLogicTimeEnd;

	unsigned long renderPlayerTimeStart;
	unsigned long renderPlayerTimeEnd;

	unsigned long renderPlanetsTimeStart;
	unsigned long renderPlanetsTimeEnd;

	unsigned long renderLeversTimeStart;
	unsigned long renderLeversTimeEnd;

	unsigned long renderFireballTimeStart;
	unsigned long renderFireballTimeEnd;

	unsigned long renderStaticObjectsTimeStart;
	unsigned long renderStaticObjectsTimeEnd;

	unsigned long renderParticlesTimeStart;
	unsigned long renderParticlesTimeEnd;

	unsigned long renderLavaDistorsionTimeStart;
	unsigned long renderLavaDistorsionTimeEnd;

	unsigned long renderAtmospheresTimeStart;
	unsigned long renderAtmospheresTimeEnd;

	unsigned long renderUITimeStart;
	unsigned long renderUITimeEnd;

	float updateDeltaTime;

public:
	BoilerState();
	~BoilerState()
	{
		OutputDebugString("Destroying BoilerState...\n");
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

private:
	bool mustRenderPlayer( );
	void updateFlickering( float deltaTime=(1.0f/60.0f) );
};

#endif // BOILER_STATE_H
