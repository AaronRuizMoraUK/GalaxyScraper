#ifndef IPLANET_LOGIC_H
#define IPLANET_LOGIC_H

#include <Windows.h>

/**
* Interface for the logic of planets
*/

class IPlanetLogic
{
protected:
	static int numberOfContinues;
	static int numberOfContinuesLeft;

public:
	IPlanetLogic() {};
	virtual ~IPlanetLogic() {
		OutputDebugString("Destroying IPlanetLogic...\n");
	};

	virtual void init() = 0;
	virtual void resetCounters() = 0;
	virtual void checkVictoryAchieved() = 0;

	void decrementContinues();
	void resetContinues( int continues=numberOfContinues );
	int getNumberOfContinuesLeft() const { return numberOfContinuesLeft; };
};

#endif //IPLANET_LOGIC_H
