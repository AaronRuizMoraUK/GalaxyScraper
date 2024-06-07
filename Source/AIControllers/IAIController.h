#ifndef IAI_CONTROLLER_H
#define IAI_CONTROLLER_H

#include "Object/CollisionableObject/AnimatedObject/AnimatedObject.h"
#include <string>

class IntelligentObject;

/**
* Interface of artificial intelligent controller for
* intelligent objects
*/

class IAIController
{
public:
	IAIController() {};
	virtual ~IAIController() {
		OutputDebugString("Destroying AI controller...\n");
	};

	virtual void update(AnimatedObject &object, float deltaTime=(1.0f/60.0f))=0;
	virtual int getCurrentAIState() const = 0;
	virtual void setCurrentAIState(int pState) = 0;

	virtual const std::string & getControllerType () const = 0;

	virtual void collisionDetected( IntelligentObject &object ) {};
};

#include "Object/CollisionableObject/AnimatedObject/IntelligentObject/IntelligentObject.h"

#endif //IAI_CONTROLLER_H
