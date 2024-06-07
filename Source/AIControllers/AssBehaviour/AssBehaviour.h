#ifndef ASS_BEHAVIOUR_H
#define ASS_BEHAVIOUR_H

#include "Object/CollisionableObject/AnimatedObject/AnimatedObject.h"
#include "AIControllers/IAISlotController.h"
#include "AIControllers/AISlots/Slot.h"
#include "Util/Timeout/Timeout.h"

const enum AssBehaviourState
{
	IDLE_1, IDLE_2, LITTLE_FART, MID_FART, STRONG_FART
};

/**
* Artificial intelligent controller that implement
* the behaviour of the ass of iuranus planet
*/

class AssBehaviour : public IAISlotController
{
protected:
	static std::string controllerType;

	Slots slots;
	AssBehaviourState state;
	Timeout timeout;
	bool firstTime;
	
public:

	AssBehaviour(unsigned int numSlots);
	~AssBehaviour() {
		OutputDebugString("Destroying AssBehaviour...\n");

		// Automaticaly slots destroies itself
	};

	
	const std::string & getControllerType () const { return controllerType; };

	void update(AnimatedObject &object, float deltaTime=(1.0f/60.0f));

	Slot* getFreeSlot() const;
	unsigned int getSlotIndex(Slot *s) const;
	Slot* getSlotByIndex(unsigned int i) const;
	
	int getCurrentAIState() const { return state; };
	void setCurrentAIState(int pState) {state=(AssBehaviourState)pState;} ;

private:
	void loadSlots(unsigned int numSlots);
	bool checkPlayerInRadius(AnimatedObject &object) const;

	void enableAssColor(const AnimatedObject &object, DWORD color);
	void disableAssColor(const AnimatedObject &object);
};

#endif //ASS_BEHAVIOUR_H
