#include "CanonBallBehaviour.h"
#include "Object/CollisionableObject/AnimatedObject/Player/Player.h"
#include "AnimationControllers/FireballAnimation/FireballAnimation.h"
#include "Managers/ParticlesManager.h"
#include "Managers/ParticlesManager.h"
#include "Managers/PlanetManager.h"
#include "Managers/SoundManager.h"
#include "Global/GlobalVariables.h"

std::string CanonBallBehaviour::controllerType = "canon_behaviour";

CanonBallBehaviour::CanonBallBehaviour(float pTimeSecuence, float pTimeMovement ) 
: IAIController()
, state(C_WAITING_TO_MOVE)
{
	timeSequence.init(pTimeSecuence);
	timeMovement.init(pTimeMovement);
	timeMoving.init(1.0f);

	restartData();
}

bool CanonBallBehaviour::checkDistance(AnimatedObject &object)
{

	FireballAnimation *fa = (FireballAnimation *) object.getAnimationController();

//	if (!fa->getIsFalling()) return false;

	const Planet *currentPlanet = PlanetManager::getPlanetManager()->getPlanet("boiler");
	const D3DXVECTOR3 &planetCenter = currentPlanet->getPosition();

	float ballToCenterDistance = Global::distanceSqrt( object.getPosition(), planetCenter );

	if( ballToCenterDistance > 35.0f) 
	{
	//	ParticlesManager::getParticlesManager()->addParticle("temporal_lava_bubbles", &object, true);
		return true;
	}

	return false;
}

void CanonBallBehaviour::update(AnimatedObject &object, float deltaTime)
{
	timeSequence.update(deltaTime);
	timeMovement.update(deltaTime);
	timeMoving.update(deltaTime);

	switch (state)
	{
	case C_MOVING:
		if (checkDistance(object))
		{
			restart(object);
			state = C_WAITING_TO_RESTART;
		}
		break;
	case C_WAITING_TO_MOVE:
		break;
	case C_WAITING_TO_RESTART:
		state = C_WAITING_TO_MOVE;
		break;
	default:
		assert( !"This should not happend" );
		return;
		break;
	}
}

void CanonBallBehaviour::setObjectMoving(AnimatedObject &object)
{
	restart(object);
	FireballAnimation *fa = (FireballAnimation *) object.getAnimationController();
	assert( fa );
	fa->setCurrentAnimationState(C_MOVING);

	state = C_MOVING;

	if( Global::samplesEnabled )
	{	
		SoundManager *soundManager = SoundManager::getSoundManager();
		soundManager->playSample("fireball_thrown");
	}
}

void CanonBallBehaviour::restartData()
{
	timeSequence.restart();
	timeMovement.restart();
	timeMoving.restart();
}


void CanonBallBehaviour::restart(AnimatedObject &object)
{
	restartData();	

	state = C_WAITING_TO_MOVE;

	FireballAnimation *fa = (FireballAnimation *) object.getAnimationController();
	assert( fa );
	fa->resetPosition(object);

	fa->setCurrentAnimationState(C_WAITING_TO_MOVE);
}


void CanonBallBehaviour::initTimeouts(float pTimeSequence, float pTimeMovement)
{
	timeSequence.init(pTimeSequence);
	timeMovement.init(pTimeMovement);
	timeMoving.init(2.0f);
}

