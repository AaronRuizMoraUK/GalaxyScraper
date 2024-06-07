#ifndef CAL_CORE_MODEL_FACTORY_H
#define CAL_CORE_MODEL_FACTORY_H

#include <Windows.h>
#include <map>
#include <string>
#include "cal3d/cal3d.h"
#include "Expat/XMLParser.h"
#include "Util/MapComparers/MapComparers.h"

/**
* This struct has cal3d core information and all the ids
* of the animations and mesh
*/

typedef struct CalAnimCoreModelStruct {
	// Cal3D core model
	CalCoreModel *calCoreModel;

	// Cal3D mesh Id
	int meshId;

	// Cal3D animations Id
	std::map<std::string, int> *idAnimMap;
} CalAnimCoreModel;

typedef std::map<std::string, CalAnimCoreModel *, LessString> CalAnimCoreModelMap;

/**
* Factory of cal core models.
*
* This is a singleton.
*/

class CalCoreModelFactory : public CXMLParser 
{
protected:
	std::string pathToCal;
	CalAnimCoreModelMap calAnimCoreModelMap;

public:
	~CalCoreModelFactory();
	
	static CalCoreModelFactory * getCalCoreModelFactory();

	CalAnimCoreModel * createCalAnimCoreModel(const std::string &filename);

	// CXMLParser Interface
	void onStartElement (const std::string &elem, MKeyValue &atts);
	void onEndElement (const std::string &elem);

private:
	static CalCoreModelFactory * calCoreModelFactory;
	CalCoreModelFactory();

	CalAnimCoreModel *currentCalAnimCoreModel; // For xml reading
};

#endif //CAL_CORE_MODEL_FACTORY_H
