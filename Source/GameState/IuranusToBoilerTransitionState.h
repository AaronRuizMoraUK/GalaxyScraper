#ifndef IURANUS_TO_BOILER_TRANSITION_STATE_H
#define IURANUS_TO_BOILER_TRANSITION_STATE_H

#include <Windows.h>
#include "GameState/IGameState.h"
#include "AnimationControllers/KeyFramed/RigidAnimationResource.h"

/**
* Transition from iuranus to boiler planet game state
*/

class IuranusToBoilerTransitionState : public IGameState
{
protected:
	static std::string name;

	// Rigid Animations Resources
	RigidAnimationResource *transitionPlayerAnimationResource;

	Object glowPlayer;

public:
	IuranusToBoilerTransitionState() : IGameState(), transitionPlayerAnimationResource( NULL ) {};
	~IuranusToBoilerTransitionState()
	{
		OutputDebugString("Destroying IuranusToBoilerTransitionState...\n");

		// Destroy animations resources
		if(transitionPlayerAnimationResource)
			delete transitionPlayerAnimationResource, transitionPlayerAnimationResource = NULL;
	};

	void update( float deltaTime=(1.0f/60.0f) );
	void onLoad( );
	void onUnload( );
	const std::string & getName( ) const { return name; };

protected:
	void loadObjects();
	void reloadShader();

	void preRender( );
	void renderWorld3D( );
	void renderWorld2D( );
	void renderPostProcessed2D( );
	void renderGameUI( );
	void postRender( );

	void renderFrame( );

private:
	void updateGlowPlayer( float deltaTime=(1.0f/60.0f) );
};

#endif // IURANUS_TO_BOILER_TRANSITION_STATE_H
