#ifndef CANONS_MANAGER_H
#define CANONS_MANAGER_H

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

class CanonsManager : public std::map<int, CanonObject *>, public CXMLParser
{
protected:
	CanonObject *currentParsed;
	int currentParsedElementId;

	// Rigid Animations Resources
	RigidAnimationResource *activateCanonAnimationResource;
	RigidAnimationResource *shootCanonAnimationResource;
	
public:
	~CanonsManager() 
	{
		OutputDebugString("Destroying CanonsManager...\n");
		
		cleanUp();
	
		// Destroy animations resources
		if(activateCanonAnimationResource)
			delete activateCanonAnimationResource, activateCanonAnimationResource = NULL;

		if(shootCanonAnimationResource)
			delete shootCanonAnimationResource, shootCanonAnimationResource = NULL;
	};
	void cleanUp();

	void restart();

	void render() const;
	static CanonsManager * getCanonsManager();
	void updateAnimation(float deltaTime=(1.0f/60.0f));
	
	void onStartElement (const std::string &elem, MKeyValue &atts);
	void onEndElement (const std::string &elem);
	
	void activateCanon( int leverId );
	void shootCanon( int canonId );
	void reloadShader();

	void changeAnimationController(int canonId);
	
private:
	void init();
	void createElement(const std::string &elem, MKeyValue &atts);

	static CanonsManager * canonsManager;
	CanonsManager();
};

#endif //CANONS_MANAGER_H
