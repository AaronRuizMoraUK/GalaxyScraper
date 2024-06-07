#ifndef SLOT_H
#define SLOT_H

#include "Object/Object.h"

/**
* Implementation of a slot
*/

class Slot
{
protected:

	// Destiny Object slot
	Object *toWho;
	unsigned int targetSlotNumber;

	// Itself Object position
	const D3DXVECTOR3 myLocalSlotPosition;

public:

	Slot(const D3DXVECTOR3 &pMyLocalSlotPosition);
	~Slot()
	{
		OutputDebugString("Destroying Slot...\n");
	};

	bool isFree() const{ return (toWho == NULL); };
	const Object * getToWho() const { return toWho; };
	void setToWho(Object *pToWho) { toWho = pToWho; };
	unsigned int getTargetSlotNumber() const { return targetSlotNumber; };
	void setTargetSlotNumber(unsigned int pTargetSlotNumber) { targetSlotNumber = pTargetSlotNumber; };
	const D3DXVECTOR3 &getMyLocalSlotPosition() const { return myLocalSlotPosition; };
};

class Slots : public std::vector<Slot *>
{
public:
	// Default constructor
	Slots(){};

	// Override copy constructor
	Slots(const Slots &source) {
		copy(source);
	};

	// Destructor
	~Slots() {
		OutputDebugString("Destroying Slots...\n");
		clear();
	};

	// Override Operator '='
	void operator=(const Slots &source) {
		clear();
		copy(source);
	};

	// Override clear function
	void clear() {
		while( !empty() ) {
			Slot * slot = (*this)[size()-1];
			if(slot)
				delete slot, slot=NULL;
			pop_back();
		}
	}

private:
	// Copy function
	void copy(const Slots &source) {
		for(unsigned int i=0; i<source.size(); ++i) {
			Slot *slot = new Slot( *(source[i]) );
			push_back(slot);
		}
	};

};

#endif // SLOT_H
