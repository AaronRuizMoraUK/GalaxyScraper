#include "IPlanetLogic.h"

int IPlanetLogic::numberOfContinues = 5;
int IPlanetLogic::numberOfContinuesLeft = IPlanetLogic::numberOfContinues;

void IPlanetLogic::decrementContinues() { 
	if( numberOfContinuesLeft>0 )
		--numberOfContinuesLeft;
}

void IPlanetLogic::resetContinues( int continues ) {

	numberOfContinuesLeft = numberOfContinues = continues;
}
