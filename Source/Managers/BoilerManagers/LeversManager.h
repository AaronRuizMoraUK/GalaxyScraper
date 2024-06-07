#ifndef LEVERS_MANAGER_H
#define LEVERS_MANAGER_H

#include <Windows.h>
#include <map>
#include "Object/CollisionableObject/AnimatedObject/AnimatedObject.h"
#include "Expat/XMLParser.h"
#include "AnimationControllers/KeyFramed/RigidAnimationResource.h"

/**
* Manager of Boiler levers
*
* It is a singleton.
*/

class LeversManager : public std::map<int, LeverObject *>, public CXMLParser
{
protected:
	LeverObject *currentParsed;
	int currentParsedElementId;

	// Rigid Animations Resources
	RigidAnimationResource *activateLeverAnimationResource;
	
public:
	~LeversManager() 
	{
		OutputDebugString("Destroying LeversManager...\n");
		
		cleanUp();
	
		// Destroy animations resources
		if(activateLeverAnimationResource)
			delete activateLeverAnimationResource, activateLeverAnimationResource = NULL;
	};
	void cleanUp();

	void restart();

	void render() const;
	static LeversManager * getLeversManager();
	void updateAnimation(float deltaTime=(1.0f/60.0f));
	
	void onStartElement (const std::string &elem, MKeyValue &atts);
	void onEndElement (const std::string &elem);
	
	void activateLever( int leverId );
	void reloadShader();
	
private:
	void init();
	void createElement(const std::string &elem, MKeyValue &atts);

	static LeversManager * leversManager;
	LeversManager();
};

#endif //FIRE_PLATFORMS_MANAGER_H
