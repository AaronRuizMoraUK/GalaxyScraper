#ifndef MOUTH_BEHAVIOUR_H
#define MOUTH_BEHAVIOUR_H

#include "Object/CollisionableObject/AnimatedObject/AnimatedObject.h"
#include "AIControllers/IAISlotController.h"
#include "AIControllers/AISlots/Slot.h"
#include "Util/Timeout/Timeout.h"

const enum MouthBehaviourState
{
	IDLE1, IDLE2, EATING_ENEMY, EATING_PLAYER
};

/**
* Artificial intelligent controller that implement
* the behaviour of the mouth of iuranus planet
*/

class MouthBehaviour : public IAISlotController
{
protected:
	static std::string controllerType;

	Slots slots;
	MouthBehaviourState state;
	Timeout timeout;

public:

	MouthBehaviour(unsigned int numSlots);
	~MouthBehaviour() {
		OutputDebugString("Destroying MouthBehaviour...\n");
		// Automaticaly slots destroies itself
	};

	const std::string & getControllerType () const { return controllerType; };

	void update(AnimatedObject &object, float deltaTime=(1.0f/60.0f));
	
	Slot* getFreeSlot() const;
	unsigned int getSlotIndex(Slot *s) const;
	Slot* getSlotByIndex(unsigned int i) const;
	
	int getCurrentAIState() const { return state; };
	void setCurrentAIState(int pState) {state=(MouthBehaviourState)pState;};

private:
	bool firstTime;

	void loadSlots(unsigned int numSlots);

	void checkEnemyInRadius(/*AnimatedObject &object*/);
	
	void doIdle1(AnimatedObject &object, float deltaTime);
	void doIdle2(AnimatedObject &object, float deltaTime);
	void doEatingEnemy(AnimatedObject &object, float deltaTime);
	void doEatingPlayer(AnimatedObject &object, float deltaTime);
};

#endif //MOUTH_BEHAVIOUR_H
