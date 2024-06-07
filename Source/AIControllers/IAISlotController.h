#ifndef IAI_SLOT_CONTROLLER_H
#define IAI_SLOT_CONTROLLER_H

#include "AIControllers/IAIController.h"
#include "AIControllers/AISlots/Slot.h"

/**
* Interface to add slots to artificial intelligent controller for
* certain intelligent objects that need them
*/

class IAISlotController : public IAIController
{
public:
	IAISlotController() {};
	virtual ~IAISlotController() {
		OutputDebugString("Destroying AI Slot controller...\n");
	};

	virtual Slot* getFreeSlot() const = 0;
	virtual unsigned int getSlotIndex(Slot *s) const = 0;
	virtual Slot* getSlotByIndex(unsigned int i) const = 0;
};

#endif //IAI_SLOT_CONTROLLER_H
