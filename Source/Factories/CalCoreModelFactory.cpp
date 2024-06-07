#include "CalCoreModelFactory.h"
#include "Managers/DataLocatorManager.h"

CalCoreModelFactory * CalCoreModelFactory::calCoreModelFactory = NULL;

CalCoreModelFactory * CalCoreModelFactory::getCalCoreModelFactory() {
	if(calCoreModelFactory == NULL)
		calCoreModelFactory = new CalCoreModelFactory();

	return calCoreModelFactory;
}

CalCoreModelFactory::CalCoreModelFactory()
: currentCalAnimCoreModel(NULL) 
{

	DataLocatorManager *dataLocatorManager = DataLocatorManager::getDataLocatorManager();
	pathToCal = dataLocatorManager->getPath("cal");

	const std::string &pathToXml = dataLocatorManager->getPath("xml");
	bool ok = xmlParseFile(pathToXml + "cal_core_models.xml");
	assert( ok );
};

CalAnimCoreModel * CalCoreModelFactory::createCalAnimCoreModel(const std::string &filename) {
	CalAnimCoreModelMap::const_iterator it = calAnimCoreModelMap.find(filename);

	if(it != calAnimCoreModelMap.end())
		return it->second;

	return NULL;
}

void CalCoreModelFactory::onStartElement (const std::string &elem, MKeyValue &atts) {

	if( currentCalAnimCoreModel ) {
		if (elem=="skeleton") {
			std::string name=atts["filename"];
			bool isOk = currentCalAnimCoreModel->calCoreModel->loadCoreSkeleton(pathToCal + name);
			assert(isOk);
		}
		if (elem=="animation") {
			std::string name=atts["name"];
			std::string filename=atts["filename"];
			int animId = currentCalAnimCoreModel->calCoreModel->loadCoreAnimation(pathToCal + filename);
			if (animId == -1)
				int a = 0;
			assert( animId>=0 );

			(*(currentCalAnimCoreModel->idAnimMap))[name] = animId;
		}
		if (elem=="mesh") {
			std::string name=atts["filename"];
			currentCalAnimCoreModel->meshId = currentCalAnimCoreModel->calCoreModel->loadCoreMesh(pathToCal + name);
			assert( currentCalAnimCoreModel!=NULL && currentCalAnimCoreModel->meshId>=0 );
		}
	}
	else {
		if (elem=="core_model") {
			std::string name=atts["name"];

			// Create the CalAnimCoreModel struct
			currentCalAnimCoreModel = new CalAnimCoreModel();

			// Create CalCoreModel
			currentCalAnimCoreModel->calCoreModel = new CalCoreModel(name);

			// Init meshId
			currentCalAnimCoreModel->meshId = 0;

			// Create the map for the anims id
			currentCalAnimCoreModel->idAnimMap = new std::map<std::string, int>();

			// Add the CalAnimCoreModel struct to the map with name as key
			calAnimCoreModelMap[name] = currentCalAnimCoreModel;
		}
	}
}

void CalCoreModelFactory::onEndElement (const std::string &elem) {
	if( currentCalAnimCoreModel && elem=="core_model" )
		currentCalAnimCoreModel = NULL;
}

CalCoreModelFactory::~CalCoreModelFactory() {
	OutputDebugString("Destroying CalCoreModelFactory...\n");

	// Destroy calAnimCoreModel map
	CalAnimCoreModelMap::const_iterator it = calAnimCoreModelMap.begin();
	while(it !=  calAnimCoreModelMap.end()) {	
		CalAnimCoreModel *calAnimCoreModel = (it->second);

		if(calAnimCoreModel) {
			// Destroy the core model
			if(calAnimCoreModel->calCoreModel)
				delete calAnimCoreModel->calCoreModel, calAnimCoreModel->calCoreModel=NULL;

			// Destroy de id anim map
			if(calAnimCoreModel->idAnimMap)
				delete calAnimCoreModel->idAnimMap, calAnimCoreModel->idAnimMap=NULL;

			delete calAnimCoreModel, calAnimCoreModel=NULL;
		}
		++it;
	}
	calAnimCoreModelMap.clear();
}
