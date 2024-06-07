#ifndef IURANUS_PLANET_ELEMENTS_MANAGER_H
#define IURANUS_PLANET_ELEMENTS_MANAGER_H

#include <map>
#include "Object/CollisionableObject/AnimatedObject/IntelligentObject/IntelligentObject.h"
#include "AIControllers/AISlots/Slot.h"
#include "Util/MapComparers/MapComparers.h"
#include "XMLParser/XMLParser.h"

// TODO
//#define MOUTH_RADIUS 3.2f
//#define ASS_RADIUS 1.1f

/**
* Manager of planet elements, like mouth or ass.
*
* It is a singleton.
*/

class IuranusPlanetElementsManager : public std::map<std::string, IntelligentObject *, LessString>, public CXMLParser
{
protected:
	/*
	std::string mouth; // TODO
	std::string ass;
	std::string ass_inside;
	std::string tongue;
	std::string land;
	*/
	IntelligentObject *currentParsed;
	std::string currentParsedElementName;

	float mouthRadius;
	float assRadius;

public:
	~IuranusPlanetElementsManager() {
		OutputDebugString("Destroying IuranusPlanetElementsManager...\n");
		cleanUp();
	};
	void cleanUp();

	static IuranusPlanetElementsManager * getIuranusPlanetElementsManager();

	void onStartElement (const std::string &elem, MKeyValue &atts);
	void onEndElement (const std::string &elem);

//	void read(const char *fileName);
	void render() const;

	void updateAI(float deltaTime=(1.0f/60.0f));
	void updateAnimation(float deltaTime=(1.0f/60.0f));

	IntelligentObject * getElement(const std::string &elem) const;
	float getElementRadius(const std::string &elem) const;
	Slot * getFreeElementSlot(const std::string &elem) const;
	unsigned int getElementSlotIndex(const std::string &elem, Slot *s) const;
	Slot * getElementSlotByIndex(const std::string &elem, unsigned int i) const;

	// TO-DO This will change when we define correctly this mess
	void animateMouth(ActionType type, const std::string &animation);
	void animateTongue(ActionType type, const std::string &animation);
	void animateAss(ActionType type, const std::string &animation);

	void reloadShader();

private:
	void init();
	void createElement(const std::string &elem, MKeyValue &atts);

	static IuranusPlanetElementsManager * iuranusPlanetElementsManager;
	IuranusPlanetElementsManager();
	/*
	: mouth("mouth"), ass("ass"), tongue("tongue"), land("land"), ass_inside("ass_inside"), mouthRadius(MOUTH_RADIUS), assRadius(ASS_RADIUS) {
		read("./data/planet_elements_instances.txt");
	};
	*/

	//IMesh * readMesh(const char *filename);

};

#endif //IURANUS_PLANET_ELEMENTS_MANAGER_H
