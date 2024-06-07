#ifndef MENU_STATE_H
#define MENU_STATE_H

#include <Windows.h>
#include "IGameState.h"
#include "Object/CollisionableObject/AnimatedObject/AnimatedObject.h"
#include "Object/Object.h"
#include "AnimationControllers/KeyFramed/RigidAnimationResource.h"
#include "Expat/XMLParser.h"

/**
* Menu game state
*/

class MenuState : public IGameState, public CXMLParser
{
public:
	// Objects in menu state
	AnimatedObject	menuPlanet;
	Object			sky;
	Object			title;

	// Actions in menu state
	Object			startAction;
	Object			optionsAction;
	Object			creditsAction;
	Object			exitAction;

	bool doOnLoad;

protected:
	static std::string name;

	// Menu State Actions
	typedef enum MenuStateActionEnum {START, OPTIONS, CREDITS, EXIT, NUM_MENU_STATE_ACTIONS} MenuStateAction;
	MenuStateAction menuStateAction;

	// Rigid Animations Resources
	RigidAnimationResource	*nextAnimationResource;
	RigidAnimationResource	*previousAnimationResource;

private:
	// Use just for reading from xml
	bool inPlanet;
	bool inStartAction;
	bool inOptionsAction;
	bool inCreditsAction;
	bool inExitAction;
	bool inSky;
	bool inTitle;

public:
	MenuState();
	~MenuState()
	{
		OutputDebugString("Destroying MenuState...\n");

		// The animation resources of menu planet belong to MenuState, so
		// set to NULL the controller in menu planet because it won't work
		menuPlanet.setAnimationController(NULL);

		// Destroy animations resources
		if(nextAnimationResource)
			delete nextAnimationResource, nextAnimationResource = NULL;
		if(previousAnimationResource)
			delete previousAnimationResource, previousAnimationResource = NULL;
	};

	void onStartElement (const std::string &elem, MKeyValue &atts);
	void onEndElement (const std::string &elem);

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
	// Use just for reading from xml
	void createPlanet(const std::string &elem, MKeyValue &atts);
	void createAction(Object &action, const std::string &elem, MKeyValue &atts);
	void createSky(const std::string &elem, MKeyValue &atts);
	void createTitle(const std::string &elem, MKeyValue &atts);

	void updateMenuPlanet( float deltaTime=(1.0f/60.0f) );
};

#endif // MENU_STATE_H
