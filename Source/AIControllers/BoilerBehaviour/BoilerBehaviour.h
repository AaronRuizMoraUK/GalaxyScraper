#ifndef BOILER_BEHAVIOUR_H
#define BOILER_BEHAVIOUR_H

#include "AIControllers/IAIController.h"
#include "Managers/BoilerManagers/FirePlatformsManager.h"
#include "Util/Timeout/Timeout.h"
#include "Managers/SoundManager.h"
#include "Global/GlobalVariables.h"
#include <vector>
#include <map>

const enum BoilerBehaviourState
{
	WAITING, SEQUENCING, DEATH//IDLE_1, IDLE_2, LITTLE_FART, MID_FART, STRONG_FART
};

const enum FlameZoneState
{
	ZONE_WAITING, ZONE_INIT, ZONE_STOP, ZONE_DEATH
};

typedef struct FlameZoneStruct 
{
	bool firstTime;
	int flameZoneId;
	int boilerFirePlatId;					// fire_platform used in this boiler part to generate flame_thrower
	int numFlamePlatforms;					// num of platforms for generating sequenced advance
	std::vector<int> flamePlatformsIds;		// list of flame platforms id
	FlameZoneState state;
	float timeoutFlameZoneDuration;
	float timeoutFlameInterval;
	//Timeout flameZoneDuration;
	Timeout flameInterval;
	int currentFlamePlatformTreated;
	bool zonePreparedToDeath;
	
	FlameZoneStruct()
		: firstTime(true), flameZoneId(0), 
		  boilerFirePlatId(0), numFlamePlatforms(0), 
		  state(ZONE_WAITING), currentFlamePlatformTreated(0),
		  zonePreparedToDeath(false)
	{};

	~FlameZoneStruct()
	{
		flamePlatformsIds.clear();
	}

	void onStartElement (const std::string &elem, MKeyValue &atts)
	{
		if( elem == "flame_platform" )
		{
			int flamePlatId = atts.getInt("id", 0);
			assert( flamePlatId != 0 );
			flamePlatformsIds.push_back( flamePlatId );
		}
	};

	void restart()
	{
		firstTime = true;
		state = ZONE_WAITING;
		zonePreparedToDeath = false;
		currentFlamePlatformTreated = 0;
		FirePlatformsManager *fpm = FirePlatformsManager::getFirePlatformsManager();
		std::vector<int>::iterator it = flamePlatformsIds.begin();
		while(it != flamePlatformsIds.end())
		{	
			fpm->stopSequence( *it );
			++it;
		}

		fpm->stopSequence( boilerFirePlatId );
		
	};

	void update(float deltaTime=(1.0f/60.0f))
	{
		switch( state )
		{
			case ZONE_WAITING:
				{
				}
				break;
			case ZONE_INIT:
				{
					if( firstTime )
					{
						firstTime = false;
						// init boiler flame particle
						FirePlatformsManager::getFirePlatformsManager()->startSequence( boilerFirePlatId );
						flameInterval.init( timeoutFlameInterval );
					}
					else
					{
						//flameZoneDuration.update( deltaTime );
						if( flameInterval.isFinished() )
						{
							// start next flame platform fire
							if( currentFlamePlatformTreated < numFlamePlatforms )
							{
								// while no all flame platforms fired, keep on firing
								flameInterval.init( timeoutFlameInterval );
								int nextPlatId = flamePlatformsIds[currentFlamePlatformTreated];
								FirePlatformsManager::getFirePlatformsManager()->startSequence( nextPlatId );
								currentFlamePlatformTreated++;
							}
						}
						else
							flameInterval.update( deltaTime );
					}
				}
				break;
			case ZONE_STOP:
				{
					if( firstTime )
					{
						// stop all fire platform sequenceely as they where started
						currentFlamePlatformTreated = 0;
						FirePlatformsManager::getFirePlatformsManager()->stopSequence( boilerFirePlatId );
						flameInterval.init( timeoutFlameInterval );
						firstTime = false;
					}
					else
					{
						if( flameInterval.isFinished() )
						{
							if( currentFlamePlatformTreated < numFlamePlatforms )
							{
								flameInterval.init( timeoutFlameInterval );
								int nextPlatId = flamePlatformsIds[currentFlamePlatformTreated];
								FirePlatformsManager::getFirePlatformsManager()->stopSequence( nextPlatId );
								currentFlamePlatformTreated++;
							}
							else
							{
								firstTime = true;
								if( zonePreparedToDeath )
									state = ZONE_DEATH;
								else
									state = ZONE_WAITING;
								currentFlamePlatformTreated = 0;
							}
						}
						else
							flameInterval.update( deltaTime );
					}					
				}
				break;
			case ZONE_DEATH:
				{
					zonePreparedToDeath = false;
				}
				break;
			default:;
		}
	};

	void stopFlameZone()
	{
		firstTime = true;
		state = ZONE_STOP;
		zonePreparedToDeath = false;
	};

	void initFlameZone()
	{
		if( Global::samplesEnabled )
		{
			SoundManager *soundManager = SoundManager::getSoundManager();
			soundManager->playSample("start_boss_sequence");
		}
		
		firstTime = true;
		zonePreparedToDeath = false;
		currentFlamePlatformTreated = 0;
		state = ZONE_INIT;
	};
	
	bool isStarted() { return (state != ZONE_WAITING); };
	bool isDeath() { return (state == ZONE_DEATH); };
	void killFlameZone() 
	{ 
		zonePreparedToDeath = true;

		if( state == ZONE_WAITING )
		{
			state = ZONE_DEATH;
		}
		else if( state == ZONE_INIT )
		{
			state = ZONE_STOP;
			firstTime = true;
		} 
	};

} FlameZone;

/**
* Common interface Artificial intelligent controller that implement
* the behaviour of boiler planet boilers
*/

class BoilerBehaviour : public IAIController
{
protected:
	static std::string controllerType;

	std::map<int, FlameZone *> flameZones;

	BoilerBehaviourState state;
	int currentFlameZoneTreated;
	Timeout nextFlameZoneActivation;
	float nextFlameSequenceStart;
	int numFlameZones;
	bool firstTime;
	bool onlyOneLeft;
	int numFlamesZoneTreatedDeath;
	
public:

	BoilerBehaviour( std::map<int, FlameZone *> pFlameZones, float pCurrentFlameTimeSequence, int pNumFlameZones );
	~BoilerBehaviour() 
	{
		OutputDebugString("Destroying BoilerBehaviour...\n");
		cleanUp();
	};

	void cleanUp();

	const std::string & getControllerType () const { return controllerType; };
	
	void update(AnimatedObject &object, float deltaTime=(1.0f/60.0f));

	int getCurrentAIState() const { return state; };
	void setCurrentAIState(int pState) { state=(BoilerBehaviourState)pState; };

	void initFlameZone( int flameZoneId );
	void initFlamesSequence();
	void killFlameZone( int flameZoneId );

	void restart();
	
private:
	void initAllLessCurrent();
	void checkAllFlameZonesDeath();
	void sequenceOnlyOne();
};

#endif //BOILER_BEHAVIOUR_H
