#include "IntelligentObject.h"
#include "AIControllers/ArachnidBehaviour/ArachnidBehaviour.h"
#include "AIControllers/MouthBehaviour/MouthBehaviour.h"
#include "AIControllers/AssBehaviour/AssBehaviour.h"
#include "AIControllers/BoilerBehaviour/BoilerBehaviour.h"
#include "AIControllers/FireBallBehaviour/FireBallBehaviour.h"
#include "AIControllers/FirePlatformBehaviour/FixedFirePlatformBehaviour.h"
#include "AIControllers/FirePlatformBehaviour/SequencedFirePlatformBehaviour.h"

IntelligentObject::IntelligentObject() : AnimatedObject(), aiController(NULL)
{
	// Init grid in space
	spaceGrid.push_back(0);
	spaceGrid.push_back(0);
}

IntelligentObject::IntelligentObject(const IntelligentObject &source) : 
AnimatedObject(source)
, aiController(NULL)
{
	copy(source);
}

void IntelligentObject::clear() {
	// Destroy AI controller
	if (aiController)
		delete aiController, aiController=NULL;
}

void IntelligentObject::copy(const IntelligentObject &source) {

	if( source.aiController != NULL ) {
		const std::string &controller_type = source.aiController->getControllerType();

		// TODO Check all its copy constructor
		if( controller_type == "arachnid_behaviour" )
			aiController = new ArachnidBehaviour( *((ArachnidBehaviour *) source.aiController) );
		else if ( controller_type == "mouth_behaviour" )
			aiController = new MouthBehaviour( *((MouthBehaviour *) source.aiController) );
		else if ( controller_type == "ass_behaviour" )
			aiController = new AssBehaviour( *((AssBehaviour *) source.aiController) );
		else if ( controller_type == "boiler_behaviour" )
			aiController = new BoilerBehaviour( *((BoilerBehaviour *) source.aiController) );
		else if ( controller_type == "fireball_behaviour" )
			aiController = new FireBallBehaviour( *((FireBallBehaviour *) source.aiController) );
		else if ( controller_type == "fixed_fire_platform_behaviour" )
			aiController = new FixedFirePlatformBehaviour( *((FixedFirePlatformBehaviour *) source.aiController) );
		else if ( controller_type == "sequenced_fire_platform_behaviour" )
			aiController = new SequencedFirePlatformBehaviour( *((SequencedFirePlatformBehaviour *) source.aiController) );
		else {
			assert( !"Not type of AI controller allowed in IntelligentObject" );
			return;
		}
	}

	// Init grid in space
	spaceGrid.push_back(0);
	spaceGrid.push_back(0);
	setSpatialGrid( source.spaceGrid );
}

void IntelligentObject::updateAI(float deltaTime) {
	if (aiController)
		aiController->update(*this, deltaTime);
}