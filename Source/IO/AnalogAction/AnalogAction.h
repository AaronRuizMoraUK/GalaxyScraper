#ifndef ANALOG_ACTION_H
#define ANALOG_ACTION_H

#include "Directx/Directx.h"

/**
* This is an analog action (actions with analog pad).
*/

class AnalogAction
{
protected:
	D3DXVECTOR2 direction;

	bool upIsPressedNow;
	bool upWasPressed;
	bool downIsPressedNow;
	bool downWasPressed;
	bool rightIsPressedNow;
	bool rightWasPressed;
	bool leftIsPressedNow;
	bool leftWasPressed;

public:
	AnalogAction() : direction(0,0) {};
	~AnalogAction() {
		OutputDebugString("Destroying Analog Action...\n");
	};

	const D3DXVECTOR2 & getDirection() const {return direction;};
	bool isPressedUp() const;
	bool isPressedDown() const;
	bool isPressedRight() const;
	bool isPressedLeft() const;
	bool isReleasedUp() const;
	bool isReleasedDown() const;
	bool isReleasedRight() const;
	bool isReleasedLeft() const;
	bool becomesPressedUp() const;
	bool becomesPressedDown() const;
	bool becomesPressedRight() const;
	bool becomesPressedLeft() const;
	bool becomesReleasedUp() const;
	bool becomesReleasedDown() const;
	bool becomesReleasedRight() const;
	bool becomesReleasedLeft() const;

	void update(D3DXVECTOR2 &pDirection);

private:
	bool checkUp() const;
	bool checkDown() const;
	bool checkRight() const;
	bool checkLeft() const;
};

#endif //ANALOG_ACTION_H
