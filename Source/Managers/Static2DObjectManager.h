#ifndef STATIC_2D_OBJECT_MANAGER_H
#define STATIC_2D_OBJECT_MANAGER_H

#include <map>
#include "Object/Object.h"
#include "XMLParser/XMLParser.h"

/**
* Manager of static 2D objects.
*
* It is a singleton.
*/

class Static2DObjectManager : public std::map<int, Object *>, public CXMLParser
{
protected:
	Object *currentParsed;
	int currentParsedElement;
	
public:
	~Static2DObjectManager() {
		OutputDebugString("Destroying Static2DObjectManager...\n");
		cleanUp();
	};
	void cleanUp();

	static Static2DObjectManager * getStatic2DObjectManager();

//	void read(const char *fileName);
	void render() const;

	void onStartElement (const std::string &elem, MKeyValue &atts);
	void onEndElement (const std::string &elem);

	void reloadShader();

	void activeGodray( int id, bool enabled );
	Object *getStatic2DObject( int id );

private:
	void init();
	void createElement(const std::string &elem, MKeyValue &atts);
	static Static2DObjectManager * static2DObjectManager;
	Static2DObjectManager();

};

#endif //STATIC_2D_OBJECT_MANAGER_H
