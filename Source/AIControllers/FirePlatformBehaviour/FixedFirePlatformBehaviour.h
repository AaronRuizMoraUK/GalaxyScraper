#ifndef FIXED_FIRE_PLATFORM_BEHAVIOUR_H
#define FIXED_FIRE_PLATFORM_BEHAVIOUR_H

#include "AIControllers/FirePlatformBehaviour/FirePlatformBehaviour.h"

class FixedFirePlatformBehaviour : public FirePlatformBehaviour
{
protected:
	static std::string controllerType;

public:

	FixedFirePlatformBehaviour( );
	~FixedFirePlatformBehaviour() 
	{
		OutputDebugString("Destroying FixedFirePlatformBehaviour...\n");
	};

	const std::string & getControllerType () const { return controllerType; };
	
	void startSequence(IntelligentObject &object);
	
private:
	
};

#endif //FIXED_FIRE_PLATFORM_BEHAVIOUR_H
