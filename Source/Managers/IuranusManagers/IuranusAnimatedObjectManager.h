#ifndef IURANUS_ANIMATED_OBJECT_MANAGER_H
#define IURANUS_ANIMATED_OBJECT_MANAGER_H

#include <map>
#include "Object/CollisionableObject/AnimatedObject/AnimatedObject.h"
#include "Expat/XMLParser.h"
#include "AnimationControllers/KeyFramed/RigidAnimationResource.h"

/**
* Manager of Iuranus animated objects.
*
* It is a singleton.
*/

class IuranusAnimatedObjectManager : public std::map<std::string, AnimatedObject *>, public CXMLParser
{
protected:
	AnimatedObject *currentParsed;
	std::string currentParsedElementName;

	RigidAnimationResource	*bushResource;

public:
	~IuranusAnimatedObjectManager() {
		OutputDebugString("Destroying IuranusAnimatedObjectManager...\n");
		cleanUp();

		// Destroy animations resources
		if(bushResource)
			delete bushResource, bushResource = NULL;
	};
	void cleanUp();

	static IuranusAnimatedObjectManager * getIuranusAnimatedObjectManager();

//	void read(const char *fileName);
	void render() const;
	void updateAnimation(float deltaTime=(1.0f/60.0f));

	void onStartElement (const std::string &elem, MKeyValue &atts);
	void onEndElement (const std::string &elem);

	void moveObject(const std::string &name);
	AnimatedObject * getAnimatedObject( const std::string &name ) const;

	void reloadShader();

private:
	void init();
	void createElement(const std::string &elem, MKeyValue &atts);

	static IuranusAnimatedObjectManager * iuranusAnimatedObjectManager;
	IuranusAnimatedObjectManager();
};

#endif //IURANUS_ANIMATED_OBJECT_MANAGER_H
