#include "FirePlatformsManager.h"
#include "Managers/DataLocatorManager.h"
#include "Managers/PlanetManager.h"
#include "AIControllers/FirePlatformBehaviour/FirePlatformBehaviour.h"
#include "AIControllers/FirePlatformBehaviour/FixedFirePlatformBehaviour.h" 
#include "AIControllers/FirePlatformBehaviour/SequencedFirePlatformBehaviour.h"
#include <fstream>
#include <cassert>

FirePlatformsManager * FirePlatformsManager::firePlatformsManager = NULL;

FirePlatformsManager * FirePlatformsManager::getFirePlatformsManager() 
{
	if(firePlatformsManager == NULL)
		firePlatformsManager = new FirePlatformsManager();

	return firePlatformsManager;
}

FirePlatformsManager::FirePlatformsManager()
: currentParsed(NULL)
, currentParsedElementId(0)
{
	init();
}

void FirePlatformsManager::init()
{
	DataLocatorManager *dataLocatorManager = DataLocatorManager::getDataLocatorManager();
	const std::string &pathToXml = dataLocatorManager->getPath("xml");

	std::string xmlFile = pathToXml + "/boiler/boiler_fire_platforms.xml";
	bool ok = xmlParseFile(xmlFile);
	assert(ok);
}

void FirePlatformsManager::onEndElement (const std::string &elem)
{
	if( elem == "fire_platform" )
	{
		(*this)[currentParsedElementId] = currentParsed;
		currentParsed = NULL;
	}
}

void FirePlatformsManager::onStartElement (const std::string &elem, MKeyValue &atts) 
{
	if( currentParsed ) {
		createElement( elem, atts );
		return;
	}

	if( elem == "fire_platform" )
	{
		currentParsed = new IntelligentObject();
		currentParsedElementId = atts.getInt("platform_id", 0);
	}
}

void FirePlatformsManager::createElement(const std::string &elem, MKeyValue &atts) 
{ 
	if( elem == "localization" )
	{

		bool useMatrix = atts.getBool("use_matrix", false);
		
		std::string position=atts["transform"];
		D3DXMATRIX matrix;
		D3DXMatrixIdentity(&matrix);
		sscanf_s(position.c_str(),"%f %f %f %f %f %f %f %f %f %f %f %f",
			&matrix(0,0), &matrix(0,1), &matrix(0,2),
			&matrix(1,0), &matrix(1,1), &matrix(1,2),
			&matrix(2,0), &matrix(2,1), &matrix(2,2),
			&matrix(3,0), &matrix(3,1), &matrix(3,2));

		// Boiler planet center and radius
		const Planet *planet = PlanetManager::getPlanetManager()->getPlanet("boiler");
		const D3DXVECTOR3 &center = planet->getCenter();
		
		currentParsed->setTransformMatrix(matrix);
		currentParsed->setPosition(currentParsed->getPosition() + center);

		if( !useMatrix )
		{
			float radius = planet->getRadius();

			D3DXVECTOR3 pos = currentParsed->getPosition();
			const D3DXVECTOR3 &localFrontVector = currentParsed->getLocalFrontVector();

			// Intersection point between line and sphere
			float t = 0;
			float denominator = pow(pos.x-center.x, 2) + pow(pos.y-center.y, 2) + pow(pos.z-center.z, 2);
			if (denominator!=0)
				t = radius / sqrt(denominator);
			// New position (point in the sphere)
			pos.x = center.x + t*(pos.x-center.x);
			pos.y = center.y + t*(pos.y-center.y);
			pos.z = center.z + t*(pos.z-center.z);

			// Obtain Object's new up vector
			D3DXVECTOR3 newLocalUpVector(pos.x-center.x, pos.y-center.y, pos.z-center.z);
			Global::normalizeVector3(newLocalUpVector, newLocalUpVector);

			// Obtain Player's new left vector
			D3DXVECTOR3 newLocalLeftVector;
			D3DXVec3Cross(&newLocalLeftVector, &newLocalUpVector, &localFrontVector);
			Global::normalizeVector3(newLocalLeftVector, newLocalLeftVector);

			const D3DXVECTOR3 &oldLocalUpVector = currentParsed->getLocalUpVector();
			D3DXVECTOR3 upDifference = newLocalUpVector - oldLocalUpVector;
			float difference = D3DXVec3Length( &upDifference );
			if( abs(difference) < 0.00001 )
				return;

			// Obtain Player's new front vector
			D3DXVECTOR3 newLocalFrontVector;
			D3DXVec3Cross(&newLocalFrontVector, &newLocalLeftVector, &newLocalUpVector);

			// Set the new transform matrix
			const D3DXMATRIX &transform = currentParsed->getTransformMatrix();
			D3DXMATRIX newTransform = transform;
			newTransform(0,0) = newLocalLeftVector.x;
			newTransform(0,1) = newLocalLeftVector.y;
			newTransform(0,2) = newLocalLeftVector.z;
			newTransform(1,0) = newLocalUpVector.x;
			newTransform(1,1) = newLocalUpVector.y;
			newTransform(1,2) = newLocalUpVector.z;
			newTransform(2,0) = newLocalFrontVector.x;
			newTransform(2,1) = newLocalFrontVector.y;
			newTransform(2,2) = newLocalFrontVector.z;

			// Set new transform matrix and new position
			currentParsed->setTransformMatrix(newTransform);
			currentParsed->setPosition(pos);
		}
	}
	else if( elem == "ai_controller" )
	{
		IAIController *aiController = NULL;

		std::string aiControllerType = atts["platform_type"];
		// common fields
		std::string particleType = atts["particle_type"];


		if( aiControllerType=="fixed" )
		{
			// fixed fire platform specific fields
			std::string platformType = atts["platform_type"];

			aiController = new FixedFirePlatformBehaviour( );
		}
		else if( aiControllerType=="sequenced" )
		{
			// sequenced fire platform specific fields

			float seqFireTime = atts.getFloat( "seq_fire_time", 0.0f );
			float seqStopTime = atts.getFloat( "seq_stop_time", 0.0f );

			aiController = new SequencedFirePlatformBehaviour( seqFireTime, seqStopTime);

		}
		else {
			assert( !"Not type of animation constroller allowed in FîrePlatformsManager" );
			return;
		}

		if( aiController )
		{
			FirePlatformBehaviour *fpb = (FirePlatformBehaviour *) aiController;
			fpb->setParticle(particleType);
			currentParsed->setAIController(aiController);
		}
	}
}

void FirePlatformsManager::updateAI(float deltaTime) 
{
	iterator it = begin();
	while( it != end() )
	{
		(it->second)->updateAI(deltaTime);
		++it;
	}
}

void FirePlatformsManager::startSequence( int platformId )
{
	iterator it = find(platformId);
	if( it == end() )
	{
		assert( !"[startSequence]::Platform Id not existing in FirePlatformsManager" );
	}

	IntelligentObject *platform = it->second;
	FirePlatformBehaviour *fp = (FirePlatformBehaviour *) platform->getAIController();
	fp->startSequence(*platform);
}

void FirePlatformsManager::stopSequence( int platformId )
{
	iterator it = find(platformId);
	if( it == end() )
	{
		assert( !"[stopSequence]::Platform Id not existing in FirePlatformsManager" );
	}

	IntelligentObject *platform = it->second;
	FirePlatformBehaviour *fp = (FirePlatformBehaviour *) platform->getAIController();
	fp->stopSequence(*platform);
}

int FirePlatformsManager::getPlatformState( int platformId )
{
	iterator it = find(platformId);
	if(it == end()) {
		assert( !"[getPlatformState]::Plaftorm Id not existing in FirePlatformsManager" );
		return NULL;
	}

	IntelligentObject *platform = it->second;
	return platform->getAIController()->getCurrentAIState();
}

bool FirePlatformsManager::isPlatformFiring( int platformId )
{
	iterator it = find(platformId);
	if(it == end()) {
		assert( !"[isPlatformFiring]::Plaftorm Id not existing in FirePlatformsManager" );
		return NULL;
	}

	IntelligentObject *platform = it->second;
	FirePlatformBehaviourState platformState = (FirePlatformBehaviourState) platform->getAIController()->getCurrentAIState();
	return (platformState == FIRING);
}

void FirePlatformsManager::cleanUp() 
{
	iterator it = begin();
	while(it != end())
	{	
		IntelligentObject *platform = (it->second);
		if(platform)
			delete platform, platform=NULL;
		++it;
	}
	clear();	
}
