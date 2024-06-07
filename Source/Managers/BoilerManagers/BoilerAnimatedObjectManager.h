#ifndef BOILER_ANIMATED_OBJECT_MANAGER_H
#define BOILER_ANIMATED_OBJECT_MANAGER_H

#include <map>
#include "Object/CollisionableObject/AnimatedObject/AnimatedObject.h"
#include "AnimationControllers/KeyFramed/RigidAnimationResource.h"
#include "XMLParser/XMLParser.h"

/**
* Manager of Iuranus animated objects.
*
* It is a singleton.
*/

typedef enum DOOR_ANIMATIONS_ENUM { DOOR_ANIM_SHORT = 0, DOOR_ANIM_LONG, DOOR_NO_ANIM } DOOR_ANIMATIONS;

class BoilerAnimatedObjectManager : public std::map<std::string, AnimatedObject *>, public CXMLParser
{
protected:
	AnimatedObject *currentParsed;
	std::string currentParsedElementName;

	// Rigid Animations Resources
	RigidAnimationResource *doorDownShortResource;
	RigidAnimationResource *doorDownLongResource;

	const static int numberDoorAnimations = 3;
	static DOOR_ANIMATIONS doorAnimationSequence[numberDoorAnimations+1];
	int currentDoorAnimation;

	D3DXMATRIX doorInitialTransformMatrix;

public:
	~BoilerAnimatedObjectManager() {
		OutputDebugString("Destroying BoilerAnimatedObjectManager...\n");

		cleanUp();

		// Destroy animations resources
		if(doorDownShortResource)
			delete doorDownShortResource, doorDownShortResource = NULL;
		if(doorDownLongResource)
			delete doorDownLongResource, doorDownLongResource = NULL;
	};
	void cleanUp();

	static BoilerAnimatedObjectManager * getBoilerAnimatedObjectManager();

	void restart();

//	void read(const char *fileName);
	void render() const;
	void updateAnimation(float deltaTime=(1.0f/60.0f));

	void onStartElement (const std::string &elem, MKeyValue &atts);
	void onEndElement (const std::string &elem);

	void doNextDoorAnimation( );
	AnimatedObject * getAnimatedObject( const std::string &name ) const;

	void reloadShader();

private:
	void init();
	void createElement(const std::string &elem, MKeyValue &atts);

	static BoilerAnimatedObjectManager * boilerAnimatedObjectManager;
	BoilerAnimatedObjectManager();
};

#endif //BOILER_ANIMATED_OBJECT_MANAGER_H
