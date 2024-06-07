#include "MouthBehaviour.h"
#include "Global/GlobalFunctions.h"
#include "Object/CollisionableObject/AnimatedObject/Player/Player.h"
#include "Managers/IuranusManagers/IuranusEnemyManager.h"
#include "Managers/SoundManager.h"
#include "Managers/IuranusManagers/IuranusPlanetElementsManager.h"
#include "Managers/LogicManager/LogicManager.h"

std::string MouthBehaviour::controllerType = "mouth_behaviour";

MouthBehaviour::MouthBehaviour(unsigned int numSlots) 
: IAISlotController()
, state(IDLE1)
, firstTime(true)
{
	loadSlots(numSlots);
}

void MouthBehaviour::update(AnimatedObject &object, float deltaTime)
{
	checkEnemyInRadius(/*object*/);

	switch (state)
	{
	case IDLE1:
		{
			if(firstTime)
			{
				float randTimeout = Global::obtainRandom(2.0f, 6.0f);
				timeout.init(randTimeout);
				firstTime = false;

				if( Global::samplesEnabled )
				{
					SoundManager *soundManager = SoundManager::getSoundManager();
					soundManager->playSample("mouth_idle_1");
				}
			}
			else
			{
				if(timeout.isFinished())
				{
					state = IDLE2;
					firstTime = true;
				}
				else 
					timeout.update(deltaTime);
			}
		}
		break;
	case IDLE2:
		{
			if(firstTime)
			{
				float randTimeout = Global::obtainRandom(2.0f, 6.0f);
				timeout.init(randTimeout);
				firstTime = false;

				if( Global::samplesEnabled )
				{
					SoundManager *soundManager = SoundManager::getSoundManager();
					soundManager->playSample("mouth_idle_2");
				}
			}
			else
			{
				if(timeout.isFinished())
				{
					state = IDLE1;
					firstTime = true;
				}
				else 
					timeout.update(deltaTime);
			}
		}
		break;
	// TODO: THESE 2 ARE NOT IA STATES, ARE ANIMATION
	case EATING_ENEMY:
	//	doEatingEnemy(object, deltaTime);
		break;
	case EATING_PLAYER:
	//	doEatingPlayer(object, deltaTime);
		break;
	default:
		assert( !"This should not happend" );
		return;
		break;
	}
}

void MouthBehaviour::checkEnemyInRadius(/*AnimatedObject &object*/)
{
	IuranusEnemyManager * iuranusEnemyManager = IuranusEnemyManager::getIuranusEnemyManager();
	IuranusEnemyManager::iterator it = iuranusEnemyManager->begin();

	const LogicVolumeManager &iuranusLogicVolumeManager = LogicManager::getLogicManager()->getIuranusLogicVolumeManager();
	const LogicVolume *volume = iuranusLogicVolumeManager.getElement("mouth_for_enemies");

	int numberOfEnemiesEaten = 0;
	while (it != iuranusEnemyManager->end() ) 
	{
		bool alreadyEaten = iuranusEnemyManager->enemyAlreadyEaten( *it );
		if( !alreadyEaten )
		{
			assert( *it != NULL );
			bool enemyInside = volume->checkObjectInsideVolume( *(*it) );
			if( enemyInside )
			{
				++numberOfEnemiesEaten;

				iuranusEnemyManager->addEnemyEaten( *it );
				/*
				if( Global::samplesEnabled )
				{
					SoundManager *soundManager = SoundManager::getSoundManager();
					soundManager->playSample("enemy_caught_by_mouth");
				}
				*/
				firstTime = false;				
			}
		}
		++it;
	}

	for( int i=0; i<numberOfEnemiesEaten; ++i )
		LogicManager::getLogicManager()->createUserEvent("on_iuranus_enemy_eaten");
}

Slot* MouthBehaviour::getFreeSlot() const
{

	Slots::const_iterator it = slots.begin();
	while( it != slots.end() )
	{
		if( (*it)->isFree() ) 
			return *it;
		++it;
	}

	return NULL;
}

unsigned int MouthBehaviour::getSlotIndex(Slot *s) const
{
	Slots::const_iterator it = slots.begin();
	unsigned int index = 0;
	while( it != slots.end() )
	{
		if( (*it) == s ) 
			return index;
		index++;
		++it;
	}

	return -1;
}

Slot * MouthBehaviour::getSlotByIndex(unsigned int i) const
{
	Slot *slot;
	try
	{
		slot = slots.at(i);
	} catch (std::out_of_range o)
	{
		slot = NULL;
	}
	return slot;
}

void MouthBehaviour::loadSlots(unsigned int numSlots)
{
	D3DXVECTOR3 slotLocalPosition(0.0f, 0.0f, 0.0f);
	for(unsigned int i=0; i<numSlots; ++i)
	{
		Slot *s = new Slot(slotLocalPosition);
		slots.push_back(s);
	}
}
