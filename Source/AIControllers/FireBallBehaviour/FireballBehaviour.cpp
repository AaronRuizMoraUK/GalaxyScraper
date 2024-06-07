#include "FireBallBehaviour.h"
#include "Object/CollisionableObject/AnimatedObject/Player/Player.h"
#include "AnimationControllers/FireballAnimation/FireballAnimation.h"
#include "Managers/ParticlesManager.h"
#include "Managers/ParticlesManager.h"
#include "Managers/PlanetManager.h"
#include "Managers/SoundManager.h"
#include "Global/GlobalVariables.h"

std::string FireBallBehaviour::controllerType = "fireball_behaviour";

FireBallBehaviour::FireBallBehaviour(float pTimeSecuence, float pTimeMovement ) 
: IAIController()
, state(WAITING_TO_MOVE)
{
	timeSequence.init(pTimeSecuence);
	timeMovement.init(pTimeMovement);
	timeMoving.init(1.0f);

	restartData();
}

bool FireBallBehaviour::checkDistance(AnimatedObject &object)
{

	FireballAnimation *fa = (FireballAnimation *) object.getAnimationController();

	if (!fa->getIsFalling()) return false;

	const Planet *currentPlanet = PlanetManager::getPlanetManager()->getPlanet("boiler");
	const D3DXVECTOR3 &planetCenter = currentPlanet->getPosition();

	float ballToCenterDistance = Global::distanceSqrt( object.getPosition(), planetCenter );
	if( ballToCenterDistance < 29.0f) 
	{
		ParticlesManager::getParticlesManager()->addParticle("temporal_lava_bubbles", &object, true);
		return true;
	}

	return false;
}

void FireBallBehaviour::update(AnimatedObject &object, float deltaTime)
{
	timeSequence.update(deltaTime);
	timeMovement.update(deltaTime);
	timeMoving.update(deltaTime);

	switch (state)
	{
	case MOVING:
		if (checkDistance(object))
		{
			restart(object);
			state = WAITING_TO_RESTART;
		}
		break;
	case WAITING_TO_MOVE:
		if (timeMovement.isFinished())
		{
			setObjectMoving(object);
			state = MOVING;
		}
		break;
	case WAITING_TO_RESTART:
		if (timeSequence.isFinished())
			restart(object);
		break;
	default:
		assert( !"This should not happend" );
		return;
		break;
	}
}

void FireBallBehaviour::setObjectMoving(AnimatedObject &object)
{
	restart(object);
	FireballAnimation *fa = (FireballAnimation *) object.getAnimationController();
	assert( fa );
	fa->setCurrentAnimationState(MOVING);

	if( Global::samplesEnabled )
	{	
		SoundManager *soundManager = SoundManager::getSoundManager();
		soundManager->playSample("fireball_thrown");
	}
}

void FireBallBehaviour::restartData()
{
	timeSequence.restart();
	timeMovement.restart();
	timeMoving.restart();
}


void FireBallBehaviour::restart(AnimatedObject &object)
{
	restartData();	

	state = WAITING_TO_MOVE;

	FireballAnimation *fa = (FireballAnimation *) object.getAnimationController();
	assert( fa );
	fa->resetPosition(object);

	fa->setCurrentAnimationState(WAITING_TO_MOVE);
}


void FireBallBehaviour::initTimeouts(float pTimeSequence, float pTimeMovement)
{
	timeSequence.init(pTimeSequence);
	timeMovement.init(pTimeMovement);
	timeMoving.init(2.0f);
}

