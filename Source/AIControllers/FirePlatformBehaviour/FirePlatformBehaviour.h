#ifndef FIRE_PLATFORM_BEHAVIOUR_H
#define FIRE_PLATFORM_BEHAVIOUR_H

#include "AIControllers/IAIController.h"

const enum FirePlatformBehaviourState
{
	FIRING, STOPED
};

/**
* Artificial intelligent controller that implement
* the behaviour of boiler planet boilers
*/

class FirePlatformBehaviour : public IAIController
{
protected:
	FirePlatformBehaviourState platformState;
	std::string particle;
	
public:

	FirePlatformBehaviour();
	virtual ~FirePlatformBehaviour() 
	{
		OutputDebugString("Destroying FirePlatformBehaviour...\n");
	};

//	void collisionDetected( IntelligentObject &object );

	virtual void startSequence(IntelligentObject &object) {
		assert(!"Fire platforms don't have to be started here");
	};
	virtual void stopSequence(IntelligentObject &object) {
		platformState = STOPED;
	};

	void setParticle(const std::string &pParticle)
	{
		particle = pParticle;
	};
	
	virtual void update(AnimatedObject &object, float deltaTime=(1.0f/60.0f)) {};

	int getCurrentAIState() const { return platformState; };
	void setCurrentAIState(int pState) {platformState=(FirePlatformBehaviourState)pState;};

private:
	
};

#endif //FIRE_PLATFORM_BEHAVIOUR_H
