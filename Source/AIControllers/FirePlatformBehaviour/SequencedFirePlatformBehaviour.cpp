#include "SequencedFirePlatformBehaviour.h"
#include "Managers/ParticlesManager.h"

std::string SequencedFirePlatformBehaviour::controllerType = "sequenced_fire_platform_behaviour";

SequencedFirePlatformBehaviour::SequencedFirePlatformBehaviour( float pSeqFireTime, 
																float pSeqStopTime )
: FirePlatformBehaviour( )
, seqFireTime(pSeqFireTime)
, seqStopTime(pSeqStopTime)
, firstTime(true)
{
}

void SequencedFirePlatformBehaviour::update(AnimatedObject &object, float deltaTime)
{
	switch ( platformState )
	{
		case STOPED:
			{
				if( firstTime )
				{
					firstTime = false;
					timeStoped.init( seqStopTime );
				}
				else
				{
					if( timeStoped.isFinished() )
					{
						platformState = FIRING;
						firstTime = true;
					}
					else
						timeStoped.update( deltaTime );
				}
			}
			break;
		case FIRING:
			{
				if( firstTime )
				{
					firstTime = false;
					timeFiring.init( seqFireTime );

					ParticlesManager *pm = ParticlesManager::getParticlesManager();
					pm->addParticle( particle, &object, false );
				}
				else
				{
					if( timeFiring.isFinished() )
					{
						firstTime = true;
						platformState = STOPED;
					}
					else
						timeFiring.update( deltaTime );
				}
			}
			break;
		default:
			assert( !"This should not happend" );
			return;
			break;
	}
}
