#ifndef DATA_LOCATOR_MANAGER_H
#define DATA_LOCATOR_MANAGER_H

#include <Windows.h>
#include <map>
#include "Util/MapComparers/MapComparers.h"
#include "Expat/XMLParser.h"

/**
* Manager of data.
* It is a singleton.
*/

class DataLocatorManager : public std::map<std::string, std::string, LessString>, public CXMLParser
{
public:
	~DataLocatorManager() {
		OutputDebugString("Destroying DataLocatorManager...\n");
		cleanUp();
	};
	void cleanUp();

	static DataLocatorManager * getDataLocatorManager();

	void onStartElement (const std::string &elem, MKeyValue &atts);

	const std::string & getPath(const std::string &elem);

private:
	static DataLocatorManager * dataLocatorManager;
	DataLocatorManager();

	void init();
};

#endif //DATA_LOCATOR_MANAGER_H
