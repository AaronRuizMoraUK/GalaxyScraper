#ifndef TEST_ROA_STATE_H
#define TEST_ROA_STATE_H

#include "IGameState.h"
#include "AnimationControllers/KeyFramed/RigidAnimationResource.h"
#include "Expat/XMLParser.h"

/**
* Test ROA state
*/

class TestROAState : public IGameState, public CXMLParser
{
protected:
	static std::string name;

	// Rigid Animations Resources
	RigidAnimationResource	*nextAnimationResource;
	RigidAnimationResource	*previousAnimationResource;

public:
	TestROAState();
	~TestROAState();

	void update( float deltaTime=(1.0f/60.0f) );
	void onLoad( );
	void onUnload( );
	const std::string & getName( ) const { return name; };

	void onStartElement (const std::string &elem, MKeyValue &atts);

protected:
	void loadObjects( );

	void preRender( );
	void renderWorld3D( );
	void renderWorld2D( );
	void renderPostProcessed2D( );
	void renderGameUI( );
	void postRender( );

	void renderFrame( );

private:
	void updatePlayerController( float deltaTime=(1.0f/60.0f) );
};

#endif // TEST_ROA_STATE_H
