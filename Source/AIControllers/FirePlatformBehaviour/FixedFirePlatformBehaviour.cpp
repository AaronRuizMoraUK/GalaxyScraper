#include "FixedFirePlatformBehaviour.h"
#include "Managers/ParticlesManager.h"
#include "Managers/SoundManager.h"
#include "Global/GlobalVariables.h"

std::string FixedFirePlatformBehaviour::controllerType = "fixed_fire_platform_behaviour";

FixedFirePlatformBehaviour::FixedFirePlatformBehaviour( ) 
: FirePlatformBehaviour( )
{
}

void FixedFirePlatformBehaviour::startSequence(IntelligentObject &object)
{
	if( platformState == STOPED )
	{
		ParticlesManager *pm = ParticlesManager::getParticlesManager();
		pm->addParticle( particle, &object, false );
		if( particle == "fire_platform" )
			pm->addParticle( "fire_platform_base", &object, false);
		platformState = FIRING;
	}
}
