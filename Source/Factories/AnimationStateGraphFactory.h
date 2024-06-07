#ifndef ANIMATION_STATE_GRAPH_FACTORY_H
#define ANIMATION_STATE_GRAPH_FACTORY_H

#include "Util/MapComparers/MapComparers.h"
#include "Object/CollisionableObject/AnimatedObject/AnimationGraph/AnimationState.h"

typedef std::map<std::string, AnimationStates *, LessString> AnimationStateGraphMap;

/**
* Factory of animation state graphs.
*
* This is a singleton.
*/

class AnimationStateGraphFactory : public CXMLParser 
{
protected:
	AnimationStateGraphMap animationStateGraphMap;

public:
	~AnimationStateGraphFactory();
	
	static AnimationStateGraphFactory * getAnimationStateGraphFactory();

	AnimationStates * createAnimationStateGraph(const std::string &filename);

	// CXMLParser Interface
	void onStartElement (const std::string &elem, MKeyValue &atts);
	void onEndElement (const std::string &elem);

private:
	static AnimationStateGraphFactory * animationStateGraphFactory;
	AnimationStateGraphFactory();

	AnimationStates *currentAnimationStates;
};

#endif //ANIMATION_STATE_GRAPH_FACTORY_H
