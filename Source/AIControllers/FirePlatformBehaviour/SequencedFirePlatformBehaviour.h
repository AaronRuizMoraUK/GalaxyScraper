#ifndef SEQUENCED_FIRE_PLATFORM_BEHAVIOUR_H
#define SEQUENCED_FIRE_PLATFORM_BEHAVIOUR_H

#include "AIControllers/FirePlatformBehaviour/FirePlatformBehaviour.h"
#include "Util/Timeout/Timeout.h"

class SequencedFirePlatformBehaviour : public FirePlatformBehaviour
{
protected:
	static std::string controllerType;
	
	Timeout timeFiring;
	Timeout timeStoped;
	bool firstTime;

	float seqFireTime;
	float seqStopTime;
	
public:

	SequencedFirePlatformBehaviour( float pSeqFireTime, 
									float pSeqStopTime );
	~SequencedFirePlatformBehaviour() 
	{
		OutputDebugString("Destroying SequencedFirePlatformBehaviour...\n");
	};
	
	const std::string & getControllerType () const { return controllerType; };
	void update(AnimatedObject &object, float deltaTime=(1.0f/60.0f));
		
private:
	
};

#endif //SEQUENCED_FIRE_PLATFORM_BEHAVIOUR_H
