#ifndef STATIC_OBJECT_MANAGER_H
#define STATIC_OBJECT_MANAGER_H

#include <vector>
#include "Object/CollisionableObject/CollisionableObject.h"
#include "XMLParser/XMLParser.h"

/**
* Manager of static objects.
*
* It is a singleton.
*/

class StaticObjectManager : public std::vector<CollisionableObject *>, public CXMLParser
{
protected:
	CollisionableObject *currentParsed;

public:
	// Pointer to boiler's boiler for collisions
	CollisionableObject *boiler_boiler;
	
public:
	~StaticObjectManager() {
		OutputDebugString("Destroying StaticObjectManager...\n");
		cleanUp();
	};
	void cleanUp();

	static StaticObjectManager * getStaticObjectManager();

//	void read(const char *fileName);
	void render() const;

	void onStartElement (const std::string &elem, MKeyValue &atts);
	void onEndElement (const std::string &elem);

	void reloadShader();

private:
	void init();
	void createElement(const std::string &elem, MKeyValue &atts);
	static StaticObjectManager * staticObjectManager;
	StaticObjectManager();
	/*{
		read("./data/static_object_instances.txt");
	};
	*/

	//IMesh * readMesh(const char *filename);

};

#endif //STATIC_OBJECT_MANAGER_H
