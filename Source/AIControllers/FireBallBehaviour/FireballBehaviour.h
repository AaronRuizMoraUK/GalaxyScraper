#ifndef FIREBALL_BEHAVIOUR_H
#define FIREBALL_BEHAVIOUR_H

#include "AIControllers/IAIController.h"
#include "Util/Timeout/Timeout.h"

enum FireBallBehaviourState
{
	MOVING, WAITING_TO_MOVE, WAITING_TO_RESTART
};

/**
* Artificial intelligent controller that implement
* the behaviour of boiler planet boilers
*/

class FireBallBehaviour : public IAIController
{
protected:
	
	FireBallBehaviourState state;
	static std::string controllerType;

	Timeout timeSequence;
	Timeout timeMovement;
	Timeout timeMoving;

	//ParticlesManager *pm = ParticlesManager::getParticlesManager();
	//pm->addParticle("fire_platform", platform, false);

public:

	FireBallBehaviour(float pTimeSecuence, float pTimeMovement);
	~FireBallBehaviour() 
	{
		OutputDebugString("Destroying FireBallBehaviour...\n");
	};

	void initTimeouts(float pTimeSequence, float pTimeMovement);
	void setSpeeds(float pVerticalSpeed, float fHorizontalSpeed);
	
	void update(AnimatedObject &object, float deltaTime=(1.0f/60.0f));
	void collisionDetected( IntelligentObject &object ) {};
	const std::string & getControllerType () const { return controllerType; };
	int getCurrentAIState() const { return state; };
	void setCurrentAIState(int pState) {state=(FireBallBehaviourState)pState;} ;

private:
	bool checkDistance(AnimatedObject &object);
	void restart(AnimatedObject &object);
	void restartData();
	void setObjectMoving(AnimatedObject &object);
	
	void move(AnimatedObject &object, float deltaTime);
	
};

#endif //FIREBALL_BEHAVIOUR_H
