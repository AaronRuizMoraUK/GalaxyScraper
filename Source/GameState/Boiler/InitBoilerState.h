#ifndef INIT_BOILER_STATE_H
#define INIT_BOILER_STATE_H

#include <Windows.h>
#include "GameState/IGameState.h"
#include "AnimationControllers/KeyFramed/RigidAnimationResource.h"

/**
* Init Boiler game state
*/

class InitBoilerState : public IGameState
{
protected:
	static std::string name;

	// Rigid Animations Resources
	RigidAnimationResource	*thrownInAnimationResource;

public:
	InitBoilerState() : IGameState(), thrownInAnimationResource( NULL ) {};
	~InitBoilerState()
	{
		OutputDebugString("Destroying InitBoilerState...\n");

		// Destroy animations resources
		if(thrownInAnimationResource)
			delete thrownInAnimationResource, thrownInAnimationResource = NULL;
	};

	void update( float deltaTime=(1.0f/60.0f) );
	void onLoad( );
	void onUnload( );
	const std::string & getName( ) const { return name; };

protected:
	void loadObjects();

	void preRender( );
	void renderWorld3D( );
	void renderWorld2D( );
	void renderPostProcessed2D( );
	void renderGameUI( );
	void postRender( );

	void renderFrame( );
};

#endif // INIT_BOILER_STATE_H
