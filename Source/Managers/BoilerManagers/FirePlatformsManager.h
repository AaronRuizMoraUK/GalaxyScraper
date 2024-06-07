#ifndef FIRE_PLATFORMS_MANAGER_H
#define FIRE_PLATFORMS_MANAGER_H

#include <Windows.h>
#include <map>
#include "Object/CollisionableObject/AnimatedObject/IntelligentObject/IntelligentObject.h"
#include "Expat/XMLParser.h"

// Don't render neither reloadShader needed, 'cause it's printed via particles only.

/**
* Manager of Boiler fire platforms
*
* It is a singleton.
*/

class FirePlatformsManager : public std::map<int, IntelligentObject *>, public CXMLParser
{
protected:
	IntelligentObject *currentParsed;
	int currentParsedElementId;
	
public:
	~FirePlatformsManager() {
		OutputDebugString("Destroying FirePlatformsManager...\n");
		cleanUp();
	};
	void cleanUp();

	static FirePlatformsManager * getFirePlatformsManager();
	void updateAI(float deltaTime=(1.0f/60.0f));
	int getPlatformState( int platformId );
	bool isPlatformFiring( int platformId );

	void onStartElement (const std::string &elem, MKeyValue &atts);
	void onEndElement (const std::string &elem);
	
	void startSequence( int platformId );
	void stopSequence( int platformId );

private:
	void init();
	void createElement(const std::string &elem, MKeyValue &atts);

	static FirePlatformsManager * firePlatformsManager;
	FirePlatformsManager();
};

#endif //FIRE_PLATFORMS_MANAGER_H
