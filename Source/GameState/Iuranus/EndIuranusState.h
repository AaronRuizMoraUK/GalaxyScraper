#ifndef END_IURANUS_STATE_H
#define END_IURANUS_STATE_H

#include <Windows.h>
#include "GameState/IGameState.h"
#include "AnimationControllers/KeyFramed/RigidAnimationResource.h"

/**
* End of Iuranus planet game state
*/

class EndIuranusState : public IGameState
{
protected:
	static std::string name;

	// Rigid Animations Resources
	RigidAnimationResource	*thrownOutAnimationResource;

public:
	EndIuranusState() : IGameState(), thrownOutAnimationResource( NULL ) {};
	~EndIuranusState()
	{
		OutputDebugString("Destroying EndIuranusState...\n");

		// Destroy animations resources
		if(thrownOutAnimationResource)
			delete thrownOutAnimationResource, thrownOutAnimationResource = NULL;
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

#endif // END_IURANUS_STATE_H
