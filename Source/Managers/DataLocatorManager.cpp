#include "DataLocatorManager.h"

DataLocatorManager * DataLocatorManager::dataLocatorManager = NULL;

DataLocatorManager * DataLocatorManager::getDataLocatorManager() {
	if(dataLocatorManager == NULL)
		dataLocatorManager = new DataLocatorManager();

	return dataLocatorManager;
}

DataLocatorManager::DataLocatorManager()
{
	init();
}

void DataLocatorManager::init()
{
	std::string xmlFile = "./data/data_location.xml";
	bool ok = xmlParseFile(xmlFile);
	assert(ok);
}

void DataLocatorManager::onStartElement (const std::string &elem, MKeyValue &atts) 
{
	if( elem == "data" )
	{
		std::string dataType = atts["type"];
		std::string dataPath = atts["path"];

		(*this)[dataType] = dataPath;
	}
}

void DataLocatorManager::cleanUp() 
{
	clear();
}

const std::string & DataLocatorManager::getPath(const std::string &elem)
{
	const_iterator it = find(elem);
	if( it == end() ) {
		assert( !"Element not fount in DataLocatorManager" );
	}
	return it->second;
}
