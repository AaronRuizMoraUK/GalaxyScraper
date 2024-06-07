#ifndef CANONBALL_BEHAVIOUR_H
#define CANONBALL_BEHAVIOUR_H

#include "AIControllers/IAIController.h"
#include "Util/Timeout/Timeout.h"

enum CanonBallBehaviourState
{
	C_MOVING, C_WAITING_TO_MOVE, C_WAITING_TO_RESTART
};

/**
* Artificial intelligent controller that implement
* the behaviour of boiler planet boilers
*/

class CanonBallBehaviour : public IAIController
{
protected:
	
	CanonBallBehaviourState state;
	static std::string controllerType;

	Timeout timeSequence;
	Timeout timeMovement;
	Timeout timeMoving;

	//ParticlesManager *pm = ParticlesManager::getParticlesManager();
	//pm->addParticle("fire_platform", platform, false);

public:

	CanonBallBehaviour(float pTimeSecuence, float pTimeMovement);
	~CanonBallBehaviour() 
	{
		OutputDebugString("Destroying CanonBallBehaviour...\n");
	};

	void initTimeouts(float pTimeSequence, float pTimeMovement);
	void setSpeeds(float pVerticalSpeed, float fHorizontalSpeed);
	
	void update(AnimatedObject &object, float deltaTime=(1.0f/60.0f));
	void collisionDetected( IntelligentObject &object ) {};
	const std::string & getControllerType () const { return controllerType; };
	int getCurrentAIState() const { return state; };
	void setCurrentAIState(int pState) {
		state=(CanonBallBehaviourState)pState;} 
	;

	void setObjectMoving(AnimatedObject &object);

private:
	bool checkDistance(AnimatedObject &object);
	void restart(AnimatedObject &object);
	void restartData();
	
	
	void move(AnimatedObject &object, float deltaTime);
	
};

#endif //CANONBALL_BEHAVIOUR_H
