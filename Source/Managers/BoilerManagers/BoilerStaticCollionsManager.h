#ifndef BOILER_STATIC_COLLISIONS_MANAGER_H
#define BOILER_STATIC_COLLISIONS_MANAGER_H

#include <Windows.h>
#include <vector>
#include "Object/CollisionableObject/CollisionableObject.h"
#include "Expat/XMLParser.h"

/**
* Manager of Static collisions with no mesh to be rendered.
* No update, no render.
*
* It is a singleton.
*/

class BoilerStaticCollisionsManager : public std::vector<CollisionableObject *>, public CXMLParser
{
protected:
	// Used for XML
	CollisionableObject *currentParsed;
	std::string currentParsedCollisionName;
	std::string currentParsedCollisionType;
	bool currentParsedFixY;

public:
	~BoilerStaticCollisionsManager() {
		OutputDebugString("Destroying BoilerStaticCollisionsManager...\n");
		cleanUp();
	};
	void cleanUp();

	void render( ) const;

	static BoilerStaticCollisionsManager * getBoilerStaticCollisionsManager();

	void onStartElement (const std::string &elem, MKeyValue &atts);
	void onEndElement (const std::string &elem);

	void reloadShader();

private:
	void init();
	void createElement(const std::string &elem, MKeyValue &atts);

	static BoilerStaticCollisionsManager * boilerStaticCollisionsManager;
	BoilerStaticCollisionsManager();
};

#endif //BOILER_STATIC_COLLISIONS_MANAGER_H
