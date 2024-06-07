#include "AssBehaviour.h"
#include "Global/GlobalFunctions.h"
#include "Object/CollisionableObject/AnimatedObject/Player/Player.h"
#include "Managers/IuranusManagers/IuranusPlanetElementsManager.h"

std::string AssBehaviour::controllerType = "ass_behaviour";

AssBehaviour::AssBehaviour(unsigned int numSlots) 
: IAISlotController()
, state(IDLE_1)
, firstTime(true)
{
	loadSlots(numSlots);
}

void AssBehaviour::update(AnimatedObject &object, float deltaTime)
{
	switch (state)
	{
	case IDLE_1:
		{
			if(firstTime)
			{
				float randTimeout = Global::obtainRandom(4.0f,6.0f);
				timeout.init(randTimeout);
				firstTime = false;
			}
			else
			{
				if(timeout.isFinished())
				{
					float nextAction = Global::obtainRandom(0.0f,2.0f);
					if( nextAction <= 1.0f ) state = IDLE_2;
					else state = IDLE_1;
					firstTime = true;
				}
				else 
					timeout.update(deltaTime);
			}	
		}
		break;
	case IDLE_2:
		{
			if(firstTime)
			{
				float randTimeout = Global::obtainRandom(4.0f,6.0f);
				timeout.init(randTimeout);
				firstTime = false;
			}
			else
			{
				if(timeout.isFinished())
				{
					float nextAction = Global::obtainRandom(0.0f,2.0f);
					if( nextAction <= 1.0f ) state = IDLE_2;
					else state = IDLE_1;
					firstTime = true;
				}
				else 
					timeout.update(deltaTime);
			}	
		}
		break;
	case LITTLE_FART:
		break;
	case MID_FART:
		break;
	case STRONG_FART:
		break;
	default:
		state = IDLE_1;
		break;
	}
}

bool AssBehaviour::checkPlayerInRadius(AnimatedObject &object) const
{
	Player *p = Player::getPlayer();
	const D3DXVECTOR3 &posPlayer = p->getPosition();
	const D3DXVECTOR3 &posAss = object.getPosition();
	float assRadius = IuranusPlanetElementsManager::getIuranusPlanetElementsManager()->getElementRadius("ass");
	float distance = Global::distanceSqrt(posPlayer, posAss);

	return (distance < assRadius);
}

Slot* AssBehaviour::getFreeSlot() const
{
	Slots::const_iterator it = slots.begin();
	while( it != slots.end() )
	{
		if( (*it)->isFree() ) 
			return *it;
		it++;
	}
	
	return NULL;
}

unsigned int AssBehaviour::getSlotIndex(Slot *s) const
{
	
	Slots::const_iterator it = slots.begin();
	unsigned int index = 0;
	while( it != slots.end() )
	{
		if( (*it) == s )
			return index;
		index++;
		it++;
	}
	
	return -1;
}

Slot * AssBehaviour::getSlotByIndex(unsigned int i) const
{
	Slot *slot;
	try
	{
		slot = slots.at(i);
	} catch ( std::out_of_range o )
	{
		slot = NULL;
	}
	return slot;
}

void AssBehaviour::loadSlots(unsigned int numSlots)
{
	// 1.3 porque sino la posición de la boca + la del slot no da la superficie
	// pq la boca no está mapeada al 0.0 por arriba, sinó que tiene el pivote en el centro
	D3DXVECTOR3 slotLocalPosition(0.0f,-1.9157f,0.0f);
	for(unsigned int i=0;i<numSlots;++i)
	{
		Slot *s = new Slot(slotLocalPosition);
		slots.push_back(s);
	}
}
