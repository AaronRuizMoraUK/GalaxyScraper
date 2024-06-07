#include "BoilerBehaviour.h"
#include "Managers/LogicManager/LogicManager.h"
#include "Managers/Static2DObjectManager.h"

//#include "Global/GlobalFunctions.h"
//#include "Object/CollisionableObject/AnimatedObject/Player/Player.h"

std::string BoilerBehaviour::controllerType = "boiler_behaviour";

BoilerBehaviour::BoilerBehaviour( std::map<int, FlameZone *> pFlameZones,  float pCurrentFlameTimeSequence, int pNumFlameZones ) 
: IAIController()
, flameZones( pFlameZones )
, state( WAITING )
, currentFlameZoneTreated( 1 )
, nextFlameSequenceStart( pCurrentFlameTimeSequence )
, numFlameZones( pNumFlameZones )
, firstTime(true)
, onlyOneLeft(false)
, numFlamesZoneTreatedDeath(0)
{
}

void BoilerBehaviour::update(AnimatedObject &object, float deltaTime)
{
	switch (state)
	{
		case WAITING:
			{
			}
			break;
		case SEQUENCING:
			{
				if( firstTime )
				{
					// Init all flame sequences, less first one
					initAllLessCurrent();
					
					nextFlameZoneActivation.init( nextFlameSequenceStart );
					firstTime = false;
				}
				else
				{
					if( nextFlameZoneActivation.isFinished() )
					{
						// start last currentFlameZone fire and stop current new one
						currentFlameZoneTreated = currentFlameZoneTreated%4 + 1;
						std::map<int, FlameZone *>::iterator it = flameZones.find( currentFlameZoneTreated );
						if( (it != flameZones.end())&& (!it->second->isDeath()) )
						{
							if( onlyOneLeft )
							{
								sequenceOnlyOne();
							}
							else
							{
								initAllLessCurrent();
							}
							nextFlameZoneActivation.init( nextFlameSequenceStart );
						}
					}
					else
					{
						nextFlameZoneActivation.update( deltaTime );

						// Check all flameZones which are started and update its behaviour
						std::map<int, FlameZone *>::iterator it = flameZones.begin();
						while(it != flameZones.end())
						{	
							FlameZone *flameZone = (it->second);
							if(	flameZone->isStarted() )
								flameZone->update( deltaTime );
							++it;
						}
					}
				}
			}
			break;
		case DEATH:
			{


			}
			break;
		default:
			state = WAITING;
	}
}

void BoilerBehaviour::cleanUp() 
{
	std::map<int, FlameZone *>::iterator it = flameZones.begin();
	while(it != flameZones.end())
	{	
		FlameZone *flameZone = (it->second);
		if( flameZone )
			delete flameZone, flameZone=NULL;
		++it;
	}
	flameZones.clear();
}

void BoilerBehaviour::restart()
{
	state = WAITING;
	currentFlameZoneTreated = 1;
	firstTime = true;
	onlyOneLeft = false;
	numFlamesZoneTreatedDeath = 0;

	std::map<int, FlameZone *>::iterator it = flameZones.begin();
	while(it != flameZones.end())
	{	
		FlameZone *flameZone = (it->second);
		flameZone->restart();
		++it;
	}
	
	FirePlatformsManager *fpm = FirePlatformsManager::getFirePlatformsManager();
	
	// restart boiler destroyed particles
	fpm->stopSequence(17);
	fpm->stopSequence(18);
	fpm->stopSequence(19);
	fpm->stopSequence(20);
	// restart boiler top fires
	fpm->stopSequence(21);
	fpm->stopSequence(22);
	fpm->stopSequence(23);
	fpm->stopSequence(24);
	fpm->stopSequence(25);
	fpm->stopSequence(26);
	fpm->stopSequence(27);

	Static2DObjectManager *stm = Static2DObjectManager::getStatic2DObjectManager();
	stm->activeGodray(1, true);
	stm->activeGodray(2, true);
	stm->activeGodray(3, true);
	stm->activeGodray(4, true);
}

void BoilerBehaviour::initFlameZone( int flameZoneId )
{
	// init BoilerBehaviour IA
	if( state == (BoilerBehaviourState) WAITING )
		state = (BoilerBehaviourState) SEQUENCING;

	// find flameZone by Id
	std::map<int, FlameZone *>::iterator it = flameZones.find( flameZoneId );
	if( it == flameZones.end() ) 
	{
		assert( !"Flame Zone object that not exist in BoilerBehaviour" );
	}
	else
	{
		FlameZone *flameZone = (FlameZone *) it->second;
		if( !flameZone->isStarted() )
		{
			flameZone->initFlameZone();
		}
	}
}

void BoilerBehaviour::killFlameZone( int flameZoneId )
{
	std::map<int, FlameZone *>::iterator it = flameZones.find( flameZoneId );
	if( it == flameZones.end() ) 
	{
		assert( !"Flame Zone object that not exist in BoilerBehaviour" );
	}
	else
	{
		FlameZone *flameZone = (FlameZone *) it->second;
		//if( !flameZone->isStarted() )
		//{
			flameZone->killFlameZone();
			numFlamesZoneTreatedDeath++;
			onlyOneLeft = (numFlamesZoneTreatedDeath + 1 == numFlameZones);
							
		//}
	}


	if( numFlamesZoneTreatedDeath == numFlameZones )
	{
		state = DEATH;
		LogicManager::getLogicManager()->createUserEvent("boiler_destroyed");
	}
	//checkAllFlameZonesDeath();
}

void BoilerBehaviour::initFlamesSequence()
{
	// init BoilerBehaviour IA
	if( state == (BoilerBehaviourState) WAITING )
		state = (BoilerBehaviourState) SEQUENCING;
	
}

void BoilerBehaviour::sequenceOnlyOne()
{
	std::map<int, FlameZone *>::iterator it = flameZones.find( currentFlameZoneTreated );
	if( it == flameZones.end() ) 
	{
		assert( !"Flame Zone object that not exist in BoilerBehaviour" );
	}
	else
	{
		FlameZone *flameZone = (FlameZone *) it->second;
		if( !flameZone->isStarted() )
			flameZone->initFlameZone();
		else if( flameZone->isStarted() )
			flameZone->stopFlameZone();
	}
}

void BoilerBehaviour::initAllLessCurrent()
{
	std::map<int, FlameZone *>::iterator it = flameZones.begin();
	while(it != flameZones.end())
	{	
		FlameZone *flameZone = (it->second);
		if( ( it->first != currentFlameZoneTreated ) && ( !flameZone->isStarted() ) )
			flameZone->initFlameZone();
		else if( ( it->first == currentFlameZoneTreated ) && ( flameZone->isStarted() ) )
			flameZone->stopFlameZone();
		++it;
	}
}

void BoilerBehaviour::checkAllFlameZonesDeath()
{
	int numFlameZonesDeath = 0;

	std::map<int, FlameZone *>::iterator it = flameZones.begin();
	while(it != flameZones.end())
	{	
		FlameZone *flameZone = (it->second);
		if( flameZone->isDeath() )
			numFlameZonesDeath++;
			
		++it;
	}

	if( numFlameZonesDeath == numFlameZones )
	{
		state = DEATH;
		LogicManager::getLogicManager()->createUserEvent("boiler_destroyed");
	}
}
