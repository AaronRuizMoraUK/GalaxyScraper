#include "PlayerAnimation.h"
#include "Global/GlobalFunctions.h"
#include "Global/Definitions.h"
#include "Managers/PlanetManager.h"
#include "Managers/ParticlesManager.h"
#include "Managers/LogicManager/LogicManager.h"
#include "Managers/SoundManager.h"
#include "Object/Planet/Planet.h"
#include "Object/CollisionableObject/AnimatedObject/AnimationGraph/AnimationRelation.h"
#include "cal3d/cal3d.h"

std::string PlayerAnimation::controllerType = "player_animation";

PlayerAnimation::PlayerAnimation( const D3DXVECTOR3 &pInitialPosition,
								 const D3DXQUATERNION &pInitialRotation, const D3DXVECTOR3 &pDirection, 
								 float pAmplitude, float pFrequency )
								 : IAnimationController()
								 , jumpTime(0)
								 , variation(0)
								 , jumpDurationTime(0.6f)
								 , kickTime(0)
								 , kickDurationTime(0.1f)
								 , position(pInitialPosition)
								 , rotation(pInitialRotation)
								 , initialPosition(pInitialPosition)
								 , direction(pDirection)
								 , amplitude(pAmplitude)
								 , frequency(pFrequency)
								 , acceleration(20.0f), deceleration(-35.0f)
								 , isKicking(false)
								 , alreadyPlayedKickSound(false)
								 , maxSpeed(7.0f)
								 , jumpPrevPosition(0,0,0)
								 , runTime(0)
								 , hitTime(0)
								 , firstMomentInJump( true )
{
}

void PlayerAnimation::updateGraph(AnimatedObject &object, float deltaTime )
{
	CalModel *calModel = object.getCalModel();
	if(calModel) {

		const AnimationRelation *animationRelation = object.getAnimationRelation();

		if (animationRelation != NULL)
		{	
			bool actionCanceled = false;

			const std::string &sourceCycle = animationRelation->getSourceCycle();
			const std::string &sourceAction = animationRelation->getSourceAction();

			const std::string &targetCycle = animationRelation->getTargetCycle();
			const std::string &targetAction = animationRelation->getTargetAction();

			if (animationRelation->getCancelPreviousAction())
			{
				if (! sourceAction.empty())
				{
					int id = object.getCalIdAnim(sourceAction);
					calModel->getMixer()->removeAction(id);
					actionCanceled = true;
				}
			}

			if (animationRelation->getCancelPreviousCycle())
			{
				if (! sourceCycle.empty())
				{
					int id = object.getCalIdAnim(sourceCycle);
					calModel->getMixer()->clearCycle(id, animationRelation->getClearTime());
				}
			}

			if (!targetCycle.empty())
			{
				int id = object.getCalIdAnim(targetCycle);
				calModel->getMixer()->blendCycle(id, 1, animationRelation->getBlendTime());
			}

			if (!targetAction.empty())
			{
				if (object.getCurrentAction().empty() || actionCanceled)
				{
					int id = object.getCalIdAnim(targetAction);
					calModel->getMixer()->executeAction(id, animationRelation->getBlendTime(), 0.2f, 1, false);
				}
			}

			object.moveAnimationState();
		}

		AnimationGraph * animationGraph = object.getAnimationGraph();
		if (animationGraph->isExecutingAction() && object.getCalModel()->getMixer()->getAnimationActionList().empty()) {
			const std::string &cycleName = animationGraph->getNextCycle();
			const std::string &actionName = animationGraph->getNextAction();

			assert( !cycleName.empty() || !actionName.empty() );

			animationGraph->setRelationToState(cycleName, actionName, true);
		}


		float animTimeFactor;

		if (object.getCurrentSpeed()!= 0)
			animTimeFactor = object.getSpeedMultiplier()*object.getCurrentSpeed()/(1.0f+object.getCurrentSpeed());
		else
			animTimeFactor = object.getSpeedMultiplier();

		// Set time factors for cycle and actions
		calModel->getMixer()->setTimeFactor(animTimeFactor);
		calModel->getMixer()->setActionsTimeFactor(animTimeFactor);
		
		// Update animations
		calModel->update(deltaTime, object.getDrawn());

		// Reset actions time factor and speed multiplier
		calModel->getMixer()->setActionsTimeFactor(1.0f);
		object.setSpeedMultiplier(1.0f);
	}
}

bool PlayerAnimation::isPlayerKicking() const
{
	//return (isKicking) && kickTime > 0.111f && kickTime < 0.15f;
	//return (isKicking) && kickTime > 0.25f && kickTime < 0.3f;
	return (isKicking) && kickTime > 0.1f && kickTime < 0.15f;
}


void PlayerAnimation::update(AnimatedObject &object, float deltaTime ) {
	updateGraph(object, deltaTime);

	/*

	// WALK
	if(object.getState() & 2) {
	// Max speed
	float max=maxSpeed;
	if( (object.getState() & 1) && !(objec0t.getState() & 4) )
	max /= 2;
	else if( (object.getState() & 4) && jumpTime<D3DX_PI )
	max *= 1.5f;

	// Accelerate inertia
	object.setCurrentSpeed(object.getCurrentSpeed() + acceleration*deltaTime);
	if(object.getCurrentSpeed()>max)
	object.setCurrentSpeed(max);
	}
	else {
	if(!(object.getState() & 4)){
	// Decelerate inertia
	object.setCurrentSpeed(object.getCurrentSpeed() + deceleration*deltaTime);

	if(object.getCurrentSpeed()<0)
	object.setCurrentSpeed(0);
	}
	}


	movePlayer(object, deltaTime);
	jumpPlayer(object, deltaTime);
	kickPlayer(object, deltaTime);

	*/
	// Play sounds
	const std::string &current_cycle = object.getCurrentCycle();
	const std::string &current_action = object.getCurrentAction();

	if (current_action == "kick")
	{
		isKicking = true;
		if (isPlayerKicking() && !alreadyPlayedKickSound && Global::samplesEnabled)
		{
			alreadyPlayedKickSound = true;
			SoundManager *soundManager = SoundManager::getSoundManager();
			//soundManager->playSample("kick");
			soundManager->playSample("player_kicking");
		}
	}
	else
	{
		isKicking = alreadyPlayedKickSound = false;
		kickTime = 0.0f;
	}

	if (current_cycle == "jumping")
	{
		if (jumpTime == 0.0f) 
		{
			ParticlesManager::getParticlesManager()->addParticle("initJump", &object, true);
			if( Global::samplesEnabled )
			{
				SoundManager *soundManager = SoundManager::getSoundManager();

				int randNum = floor(Global::obtainRandom( 1.0f, 2.5f ));
				randNum = Global::clamp( randNum, 1, 2 );

				char sound[ 128 ];
				sprintf_s(sound, sizeof(sound), "player_jumping_%d", randNum);

				soundManager->playSample(sound);
			}
		}
	}

	// Update animations
	float max=maxSpeed;
	bool moving = false;

	if(current_cycle == "idle")
	{
		object.setCurrentSpeed(object.getCurrentSpeed() + deceleration*deltaTime);

		if(object.getCurrentSpeed()<0)
			object.setCurrentSpeed(0);
	}
	else if (current_cycle == "walk")
	{
		max /= 2;

		object.setCurrentSpeed(object.getCurrentSpeed() + acceleration*deltaTime);

		if(object.getCurrentSpeed()>max)
			object.setCurrentSpeed(max);

		object.setSpeedMultiplier(1.4f);

		movePlayer(object, deltaTime);
	}
	else if (current_cycle == "jog")
	{
		// max *= 1.5f;
		object.setCurrentSpeed(object.getCurrentSpeed() + acceleration*deltaTime);

		if(object.getCurrentSpeed()>max)
			object.setCurrentSpeed(max);

		object.setSpeedMultiplier(1.4f);

		movePlayer(object, deltaTime);
	}
	else if (current_cycle == "jumping")
	{
		max *= 1.5f;
		if (object.getCurrentSpeed() > 0)
			object.setCurrentSpeed(object.getCurrentSpeed() + acceleration*deltaTime);

		if(object.getCurrentSpeed()>max)
			object.setCurrentSpeed(max);

		static D3DXVECTOR3 frontJump(0.0f,0.0f,0.0f);
		if( firstMomentInJump ) {
			frontJump = object.getLocalFrontVector( );
			firstMomentInJump = false;
		}

		//movePlayerInJump(object, frontJump, deltaTime);
		movePlayer(object, deltaTime);
		jumpPlayer(object, deltaTime);
	}
	else if (current_cycle == "thrown")
	{

		if( Global::playerDeepingInAss )
		{
			// make player get deeper in the ass
			deepInAss(object, deltaTime);
		}
		else
		{
			// make player jump outside
			//movePlayer( object, deltaTime );
			//jumpPlayer( object, deltaTime );
			moveExpulsionPlayer(object, deltaTime);
			jumpAssExpulsionPlayer(object, deltaTime);
		}
	}

	// Controlling Player Run Particles
	if (current_cycle == "jog")
	{
		if (runTime == 0.0f)
			ParticlesManager::getParticlesManager()->addParticle("playerRun", &object, false, false);
		
		runTime+=deltaTime;
	}
	else
	{
		runTime = 0.0f;
	}

	// Controlling Player Hit Particles
	if (current_action == "hit")
	{
		if (hitTime == 0.0f)
			ParticlesManager::getParticlesManager()->addParticle("playerHit", &object, false);
		
		hitTime+=deltaTime;
	}
	else
	{
		hitTime = 0.0f;
	}

	if (current_action == "hit")
	{
		hit(object, 4.0f, deltaTime);
		object.setSpeedMultiplier(2.0f);
	}
	else if (current_action == "kick")
	{
		object.setSpeedMultiplier(2.0f);
		kickPlayer(object, deltaTime);
	}
	else if (current_action == "prepare_jump")
	{
		object.setSpeedMultiplier(20.0f);
		if (object.getCurrentSpeed() > 0)
			object.setCurrentSpeed(object.getCurrentSpeed() + acceleration*deltaTime);

		if(object.getCurrentSpeed()>max)
			object.setCurrentSpeed(max);

		movePlayer(object, deltaTime);
		firstMomentInJump = true;
	}
	else if (current_action == "finish_jump")
	{
		if (object.getCurrentSpeed() > 0)
			object.setCurrentSpeed(object.getCurrentSpeed() + 0.5f * acceleration*deltaTime);

		if(object.getCurrentSpeed()>max)
			object.setCurrentSpeed(max);

		object.setSpeedMultiplier(10.0f);

		movePlayer(object, deltaTime);
	}
	else if (current_action == "burn")
	{
		object.setSpeedMultiplier(1.8f);
	}

	// Move player
	// movePlayer(object, deltaTime);

	// JUMP -- time in angle, max time is PI
	//static bool first = true;

	// jumpPlayer(object, deltaTime);


	// KICK -- time in seconds, 0.1 sec
	// kickPlayer(object, deltaTime);
}

void PlayerAnimation::kickPlayer(AnimatedObject &object, float deltaTime)
{	
	kickTime+=deltaTime;

	const MaterialVector &objectMaterialVector = object.getMaterialVector();
	MaterialVector::const_iterator it = objectMaterialVector.begin();

	while ( it != objectMaterialVector.end() ) {
		(*it)->enableAdditionalColor(0xFFFFFFFF);
		++it;
	}
}

void PlayerAnimation::jumpPlayer(AnimatedObject &object, float deltaTime)
{
	//static float variation = 0.0f;
	const Planet *planet = PlanetManager::getPlanetManager()->getCurrentPlanet();
	const D3DXVECTOR3 &planetCenter = planet->getCenter();
	float distance = 0.0f;
	float height = 0.0f;
	if(variation!=0.0f) {
		distance = Global::distanceSqrt( planetCenter+variation*object.getLocalUpVector(), planetCenter );
		height = planet->getHeight(object.getAlpha(), object.getDelta());
	}
	if( (variation!=0.0f && distance<=height) /*|| jumpTime >= D3DX_PI*3.0f/2.0f*/ /*jumpTime >= D3DX_PI*/ ) {
		jumpTime=0.0f;
		object.setRelationToState(ACTION, "finish_jump", false, true);
		ParticlesManager::getParticlesManager()->addParticle("finishJump", &object, true);
		variation=0.0f;
		//object.setCurrentSpeed(0.2f * object.getCurrentSpeed()); // After jump player has less speed
		return;
	}
	if( ((object.getCurrentCycle() == "jumping") || (object.getCurrentCycle() == "thrown")) && deltaTime!=0.0f) {
//	if( (object.getCurrentCycle() == "jumping") && deltaTime!=0.0f) {
		jumpTime = jumpTime + (D3DX_PI/jumpDurationTime)*deltaTime;
		//float nextStepVariation = amplitude*sin( frequency*jumpTime );
		// Function y = -((-amplitude/(D3DX_PI/2.0f)^2))*(x+D3DX_PI/2.0f)^2+amplitude)
		float a = -4*amplitude/(D3DX_PI*D3DX_PI);
		float x = frequency*jumpTime-D3DX_PI/2.0f;
		float nextStepVariation = a*x*x + amplitude;

		/*if(speed!=0)
		jumpPrevPosition = object.getPosition();
		else if (first) {
		jumpPrevPosition = object.getPosition();
		first=false;
		}*/
		static float jumpDistance = 0.0f;
		if(variation==0.0f)
			jumpDistance = Global::distanceSqrt( object.getPosition(), planetCenter );

		//const D3DXVECTOR3 & currentDirection = object.getLocalUpVector();
		//position = jumpPrevPosition + nextStepVariation*currentDirection;
		//object.setPosition(position); // TODO
		variation = (jumpDistance + nextStepVariation);
		position = planetCenter + variation*object.getLocalUpVector();
		object.setPosition(position);
	}
}

void PlayerAnimation::jumpAssExpulsionPlayer(AnimatedObject &object, float deltaTime)
{
	static float variation2 = 0.0f;
	const Planet *planet = PlanetManager::getPlanetManager()->getCurrentPlanet();
	const D3DXVECTOR3 &planetCenter = planet->getCenter();
	float distance = 0.0f;
	float height = 0.0f;
	if(variation2!=0.0f) {
		distance = Global::distanceSqrt( planetCenter+variation2*object.getLocalUpVector(), planetCenter );
		height = planet->getHeight(object.getAlpha(), object.getDelta());
	}
	if( variation2 != 0.0f && distance<=height && jumpTime>D3DX_PI/2.0f )
	{
		jumpTime=0.0f;
		object.setRelationToState(ACTION, "finish_jump", false, true);
		Global::takePlayerControlOutUpdatingAnimation = false;
		ParticlesManager::getParticlesManager()->addParticle("finishJump", &object, true);
		variation2=0.0f;
		//object.setCurrentSpeed(0.2f * object.getCurrentSpeed()); // After jump player has less speed
		return;
	}
	if( (object.getCurrentCycle() == "thrown") && deltaTime!=0.0f) 
	{
		jumpTime = jumpTime + (D3DX_PI/jumpDurationTime)*deltaTime;
		//float nextStepVariation = amplitude*sin( frequency*jumpTime );
		// Function y = -((-amplitude/(D3DX_PI/2.0f)^2))*(x+D3DX_PI/2.0f)^2+amplitude)
		float a = -4*amplitude/(D3DX_PI*D3DX_PI);
		float x = frequency*jumpTime-D3DX_PI/2.0f;
		float nextStepVariation = a*x*x + amplitude;

		/*if(speed!=0)
		jumpPrevPosition = object.getPosition();
		else if (first) {
		jumpPrevPosition = object.getPosition();
		first=false;
		}*/
		static float jumpDistance = 0.0f;
		if(variation2==0.0f)
			jumpDistance = Global::distanceSqrt( object.getPosition(), planetCenter );

		//const D3DXVECTOR3 & currentDirection = object.getLocalUpVector();
		//position = jumpPrevPosition + nextStepVariation*currentDirection;
		//object.setPosition(position); // TODO
		variation2 = (jumpDistance + nextStepVariation);
		position = planetCenter + variation2*object.getLocalUpVector();
		object.setPosition(position);
	}
}


void PlayerAnimation::moveExpulsionPlayer(AnimatedObject &object, float deltaTime)
{
	// New position (not a point in the sphere) with inertia
	const D3DXVECTOR3 &position = object.getPosition();
	const D3DXVECTOR3 &localFrontVector = object.getLocalFrontVector();
	//D3DXVECTOR3 newPosition = position + localFrontVector*object.getCurrentSpeed()*deltaTime;
	D3DXVECTOR3 newPosition = position + localFrontVector*7.0f*deltaTime;

	// Current planet center and radius
	const Planet * planet = PlanetManager::getPlanetManager()->getCurrentPlanet();
	const D3DXVECTOR3 &center = planet->getCenter();
	const float radius = Global::distanceSqrt(position, center);

	// Intersection point between line and sphere
	float t = 0;
	float denominator = pow(newPosition.x-center.x, 2) + pow(newPosition.y-center.y, 2) + pow(newPosition.z-center.z, 2);
	if (denominator!=0)
		t = radius / sqrt(denominator);
	// New position (point in the sphere)
	newPosition.x = center.x + t*(newPosition.x-center.x);
	newPosition.y = center.y + t*(newPosition.y-center.y);
	newPosition.z = center.z + t*(newPosition.z-center.z);

	// Obtain Players's new up vector
	//D3DXVECTOR3 newLocalUpVector(newPosition.x-center.x, newPosition.y-center.y, newPosition.z-center.z);
	D3DXVECTOR3 newLocalUpVector = newPosition - center;
	Global::normalizeVector3(newLocalUpVector, newLocalUpVector);

	// Calculate the final position with height
	// Necesary  to be here because fix the height
	float alpha = 0.0f;
	float delta = 0.0f;
	planet->getPolarAngles(newPosition, alpha, delta);
	float height = planet->getHeight(alpha, delta);
	float heightOld = planet->getHeight(object.getAlpha(), object.getDelta());
	object.setAlpha(alpha);
	object.setDelta(delta);
	if(height==0.0f)
		height=planet->getRadius();
	D3DXVECTOR3 finalPosition = center + (0.8f*heightOld+0.2f*height)*newLocalUpVector;
	if(finalPosition != position) { // Set final position if exist differences
		//finalPosition = 0.1f*newPosition+0.9f*finalPosition;
		object.setPosition( finalPosition );
	}

	const D3DXVECTOR3 &oldLocalUpVector = object.getLocalUpVector();
	D3DXVECTOR3 upDifference = newLocalUpVector - oldLocalUpVector;
	float difference = D3DXVec3Length( &upDifference );
	if( abs(difference) < 0.00001 )
		return;

	// Obtain Player's new left vector
	D3DXVECTOR3 newLocalLeftVector;
	D3DXVec3Cross(&newLocalLeftVector, &newLocalUpVector, &localFrontVector);
	Global::normalizeVector3(newLocalLeftVector, newLocalLeftVector);

	// Obtain Player's new front vector
	D3DXVECTOR3 newLocalFrontVector;
	D3DXVec3Cross(&newLocalFrontVector, &newLocalLeftVector, &newLocalUpVector);

	// Set the new transform matrix
	const D3DXMATRIX &transform = object.getTransformMatrix();
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
	object.setTransformMatrix(newTransform);
	object.setPosition(finalPosition);
}

void PlayerAnimation::deepInAss( AnimatedObject &object, float deltaTime )
{
	const float speedDeepingDown = 6.0f;
	const D3DXVECTOR3 &objPos = object.getPosition();

	if( deltaTime != 0 ) //&& distance > height )
	{
		D3DXVECTOR3 position = objPos - object.getLocalUpVector()*deltaTime*speedDeepingDown;
		object.setPosition( position );
	}
}

void PlayerAnimation::movePlayer(AnimatedObject &object, float deltaTime) {

	// New position (not a point in the sphere) with inertia
	const D3DXVECTOR3 &position = object.getPosition();
	const D3DXVECTOR3 &localFrontVector = object.getLocalFrontVector();
	D3DXVECTOR3 newPosition = position + localFrontVector*object.getCurrentSpeed()*deltaTime;

	// Current planet center and radius
	const Planet * planet = PlanetManager::getPlanetManager()->getCurrentPlanet();
	const D3DXVECTOR3 &center = planet->getCenter();
	const float radius = Global::distanceSqrt(position, center);

	// Intersection point between line and sphere
	float t = 0;
	float denominator = pow(newPosition.x-center.x, 2) + pow(newPosition.y-center.y, 2) + pow(newPosition.z-center.z, 2);
	if (denominator!=0)
		t = radius / sqrt(denominator);
	// New position (point in the sphere)
	newPosition.x = center.x + t*(newPosition.x-center.x);
	newPosition.y = center.y + t*(newPosition.y-center.y);
	newPosition.z = center.z + t*(newPosition.z-center.z);

	// Obtain Players's new up vector
	D3DXVECTOR3 newLocalUpVector(newPosition.x-center.x, newPosition.y-center.y, newPosition.z-center.z);
	Global::normalizeVector3(newLocalUpVector, newLocalUpVector);

	// Calculate the final position with height
	// Necesary  to be here because fix the height
	float alpha = 0.0f;
	float delta = 0.0f;
	planet->getPolarAngles(newPosition, alpha, delta);
	float height = planet->getHeight(alpha, delta);
	float heightOld = planet->getHeight(object.getAlpha(), object.getDelta());
	object.setAlpha(alpha);
	object.setDelta(delta);
	if(height==0.0f)
		height=planet->getRadius();
	D3DXVECTOR3 finalPosition = center + (0.8f*heightOld+0.2f*height)*newLocalUpVector;
	if(finalPosition != position) { // Set final position if exist differences
		//finalPosition = 0.1f*newPosition+0.9f*finalPosition;
		object.setPosition( finalPosition );
	}

	const D3DXVECTOR3 &oldLocalUpVector = object.getLocalUpVector();
	D3DXVECTOR3 upDifference = newLocalUpVector - oldLocalUpVector;
	float difference = D3DXVec3Length( &upDifference );
	if( abs(difference) < 0.00001 )
		return;

	// Obtain Player's new left vector
	D3DXVECTOR3 newLocalLeftVector;
	D3DXVec3Cross(&newLocalLeftVector, &newLocalUpVector, &localFrontVector);
	Global::normalizeVector3(newLocalLeftVector, newLocalLeftVector);

	// Obtain Player's new front vector
	D3DXVECTOR3 newLocalFrontVector;
	D3DXVec3Cross(&newLocalFrontVector, &newLocalLeftVector, &newLocalUpVector);

	// Set the new transform matrix
	const D3DXMATRIX &transform = object.getTransformMatrix();
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

	//Global::fixMatrix(newTransform, newTransform);
	/*bool isFixedMatrix1 = Global::isFixedMatrix(transform);
	if (isFixedMatrix1)
	OutputDebugString("old OK!\n");
	else
	OutputDebugString("old SHIT!\n");

	//Global::fixMatrix(newTransform, newTransform);
	bool isFixedMatrix2 = Global::isFixedMatrix(newTransform);
	if (isFixedMatrix2)
	OutputDebugString("New OK!\n");
	else
	OutputDebugString("New SHIT!\n");*/

	// Set new transform matrix and new position
	object.setTransformMatrix(newTransform);
	object.setPosition(finalPosition);
}

void PlayerAnimation::movePlayerInJump(AnimatedObject &object, const D3DXVECTOR3 &frontJump, float deltaTime) {

	// New position (not a point in the sphere) with inertia
	const D3DXVECTOR3 &position = object.getPosition();
	const D3DXVECTOR3 &localFrontVector = frontJump;
	D3DXVECTOR3 newPosition = position + localFrontVector*object.getCurrentSpeed()*deltaTime;

	// Current planet center and radius
	const Planet * planet = PlanetManager::getPlanetManager()->getCurrentPlanet();
	const D3DXVECTOR3 &center = planet->getCenter();
	const float radius = Global::distanceSqrt(position, center);

	// Intersection point between line and sphere
	float t = 0;
	float denominator = pow(newPosition.x-center.x, 2) + pow(newPosition.y-center.y, 2) + pow(newPosition.z-center.z, 2);
	if (denominator!=0)
		t = radius / sqrt(denominator);
	// New position (point in the sphere)
	newPosition.x = center.x + t*(newPosition.x-center.x);
	newPosition.y = center.y + t*(newPosition.y-center.y);
	newPosition.z = center.z + t*(newPosition.z-center.z);

	// Obtain Players's new up vector
	D3DXVECTOR3 newLocalUpVector(newPosition.x-center.x, newPosition.y-center.y, newPosition.z-center.z);
	Global::normalizeVector3(newLocalUpVector, newLocalUpVector);

	// Calculate the final position with height
	// Necesary  to be here because fix the height
	float alpha = 0.0f;
	float delta = 0.0f;
	planet->getPolarAngles(newPosition, alpha, delta);
	float height = planet->getHeight(alpha, delta);
	float heightOld = planet->getHeight(object.getAlpha(), object.getDelta());
	object.setAlpha(alpha);
	object.setDelta(delta);
	if(height==0.0f)
		height=planet->getRadius();
	D3DXVECTOR3 finalPosition = center + (0.8f*heightOld+0.2f*height)*newLocalUpVector;
	if(finalPosition != position) { // Set final position if exist differences
		//finalPosition = 0.1f*newPosition+0.9f*finalPosition;
		object.setPosition( finalPosition );
	}

	const D3DXVECTOR3 &oldLocalUpVector = object.getLocalUpVector();
	D3DXVECTOR3 upDifference = newLocalUpVector - oldLocalUpVector;
	float difference = D3DXVec3Length( &upDifference );
	if( abs(difference) < 0.00001 )
		return;

	// Obtain Player's new left vector
	D3DXVECTOR3 newLocalLeftVector;
	D3DXVec3Cross(&newLocalLeftVector, &newLocalUpVector, &object.getLocalFrontVector( ));
	Global::normalizeVector3(newLocalLeftVector, newLocalLeftVector);

	// Obtain Player's new front vector
	D3DXVECTOR3 newLocalFrontVector;
	D3DXVec3Cross(&newLocalFrontVector, &newLocalLeftVector, &newLocalUpVector);

	// Set the new transform matrix
	const D3DXMATRIX &transform = object.getTransformMatrix();
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

	//Global::fixMatrix(newTransform, newTransform);
	/*bool isFixedMatrix1 = Global::isFixedMatrix(transform);
	if (isFixedMatrix1)
	OutputDebugString("old OK!\n");
	else
	OutputDebugString("old SHIT!\n");

	//Global::fixMatrix(newTransform, newTransform);
	bool isFixedMatrix2 = Global::isFixedMatrix(newTransform);
	if (isFixedMatrix2)
	OutputDebugString("New OK!\n");
	else
	OutputDebugString("New SHIT!\n");*/

	// Set new transform matrix and new position
	object.setTransformMatrix(newTransform);
	object.setPosition(finalPosition);
}

void PlayerAnimation::hitRecieved(AnimatedObject &object, const D3DXVECTOR3 &newFront)
{
	hitDirection = newFront;
	object.setRelationToState(ACTION, "hit", false, true);
	
	if( Global::samplesEnabled )
	{
		SoundManager *soundManager = SoundManager::getSoundManager();

		int randNum = floor(Global::obtainRandom( 1.0f, 2.5f ));
		randNum = Global::clamp( randNum, 1, 2 );

		char sound[ 128 ];
		sprintf_s(sound, sizeof(sound), "player_hit_received_%d", randNum);

		soundManager->playSample(sound);
	}
}

void PlayerAnimation::hit (AnimatedObject &object, float speed, float deltaTime) 
{
	// New position (not a point in the sphere) with inertia
	const D3DXVECTOR3 &position = object.getPosition();
	D3DXVECTOR3 localFrontVector = hitDirection;
	D3DXVECTOR3 newPosition = position + localFrontVector*speed*deltaTime;

	// Current planet center and radius
	const Planet * planet = PlanetManager::getPlanetManager()->getCurrentPlanet();
	const D3DXVECTOR3 &center = planet->getCenter();
	const float radius = Global::distanceSqrt(position, center);

	// Intersection point between line and sphere
	float t = 0;
	float denominator = pow(newPosition.x-center.x, 2) + pow(newPosition.y-center.y, 2) + pow(newPosition.z-center.z, 2);
	if (denominator!=0)
		t = radius / sqrt(denominator);
	// New position (point in the sphere)
	newPosition.x = center.x + t*(newPosition.x-center.x);
	newPosition.y = center.y + t*(newPosition.y-center.y);
	newPosition.z = center.z + t*(newPosition.z-center.z);

	D3DXVECTOR3 localUp = newPosition - center;
	Global::normalizeVector3(localUp, localUp);

	D3DXVECTOR3 localLeft;
	D3DXVec3Cross(&localLeft, &localUp, &object.getLocalFrontVector());
	Global::normalizeVector3(localLeft, localLeft);

	D3DXVECTOR3 localFront;
	D3DXVec3Cross(&localFront, &localLeft, &localUp);

	// Rotation
	D3DXMATRIX transformMatrix;
	D3DXMatrixIdentity(&transformMatrix);
	transformMatrix(0,0) = localLeft.x;
	transformMatrix(0,1) = localLeft.y;
	transformMatrix(0,2) = localLeft.z;
	transformMatrix(1,0) = localUp.x;
	transformMatrix(1,1) = localUp.y;
	transformMatrix(1,2) = localUp.z;
	transformMatrix(2,0) = localFront.x;
	transformMatrix(2,1) = localFront.y;
	transformMatrix(2,2) = localFront.z;

	object.setTransformMatrix(transformMatrix);

	// Calculate the final position with height
	float alpha = 0.0f;
	float delta = 0.0f;
	planet->getPolarAngles(newPosition, alpha, delta);
	float height = planet->getHeight(alpha, delta);
	float heightOld = planet->getHeight(object.getAlpha(), object.getDelta());
	object.setAlpha(alpha);
	object.setDelta(delta);
	if(height==0.0f)
		height=planet->getRadius();
	D3DXVECTOR3 finalPosition = center + (0.8f*heightOld+0.2f*height)*localUp;
	//object.setPosition( 0.7f*newPosition+0.3f*finalPosition );
	object.setPosition( finalPosition );
}

void PlayerAnimation::collisionDetected( AnimatedObject &object, const D3DXVECTOR3 &normal, D3DXVECTOR3 &additionalMovementInOut )
{
	const D3DXMATRIX &previousTransformMatrix = object.getPreviousTransformMatrix();
	D3DXVECTOR3	previousPosition( previousTransformMatrix(3,0), previousTransformMatrix(3,1), previousTransformMatrix(3,2) );

	D3DXVECTOR3 positionVector = object.getPosition() - previousPosition;
	//Global::normalizeVector3(positionVector, positionVector);
	float positionNormalDot = D3DXVec3Dot(&positionVector, &normal);

	D3DXVECTOR3 movement(0.0f, 0.0f, 0.0f);

	// If previous and current position are differents
	// and position vector and normal dot product is less than 0,
	// then correct position
	if( object.getPosition()!=previousPosition && positionNormalDot<0.0f )
	{
		float e = -0.5f;

		movement = (1.0f+e) * positionNormalDot * normal;
		// Obtain new position
		D3DXVECTOR3 newPosition = previousPosition + ( (positionVector - movement) + additionalMovementInOut );
		correctPosition(object, newPosition);
	}
	else if ( additionalMovementInOut!=D3DXVECTOR3(0.0f, 0.0f, 0.0f) ) {
		// Obtain new position
		D3DXVECTOR3 newPosition = previousPosition + additionalMovementInOut;
		correctPosition(object, newPosition);
	}

	// Save the additional movement to return it
	additionalMovementInOut = movement;
}

void PlayerAnimation::collisionDetected(AnimatedObject& object, const D3DXVECTOR3& normal)
{
	const D3DXMATRIX& previousTransformMatrix = object.getPreviousTransformMatrix();
	D3DXVECTOR3	previousPosition(previousTransformMatrix(3, 0), previousTransformMatrix(3, 1), previousTransformMatrix(3, 2));

	D3DXVECTOR3 positionVector = object.getPosition() - previousPosition;
	//Global::normalizeVector3(positionVector, positionVector);
	float positionNormalDot = D3DXVec3Dot(&positionVector, &normal);

	D3DXVECTOR3 movement(0.0f, 0.0f, 0.0f);

	// If previous and current position are differents
	// and position vector and normal dot product is less than 0,
	// then correct position
	if (object.getPosition() != previousPosition && positionNormalDot < 0.0f)
	{
		float e = -0.5f;

		movement = (1.0f + e) * positionNormalDot * normal;
		// Obtain new position
		D3DXVECTOR3 newPosition = previousPosition + (positionVector - movement);
		correctPosition(object, newPosition);
	}
}

void PlayerAnimation::correctPosition( AnimatedObject &object, const D3DXVECTOR3 &newPosition ) {
	// Get planet center
	const Planet * planet = PlanetManager::getPlanetManager()->getCurrentPlanet();
	const D3DXVECTOR3 &center = planet->getCenter();

	// Obtain Players's new up vector
	D3DXVECTOR3 newLocalUpVector(newPosition.x-center.x, newPosition.y-center.y, newPosition.z-center.z);
	Global::normalizeVector3(newLocalUpVector, newLocalUpVector);

	// Obtain Player's new left vector
	D3DXVECTOR3 newLocalLeftVector;
	D3DXVec3Cross(&newLocalLeftVector, &newLocalUpVector, &object.getLocalFrontVector());
	Global::normalizeVector3(newLocalLeftVector, newLocalLeftVector);

	// Obtain Player's new front vector
	D3DXVECTOR3 newLocalFrontVector;
	D3DXVec3Cross(&newLocalFrontVector, &newLocalLeftVector, &newLocalUpVector);

	// If object is jumping, do not get his heightmap height
	D3DXVECTOR3 finalPosition;
	if( isJumping( object ) ) {
		finalPosition = newPosition;
	}
	else {
		// Calculate the final position with height
		// Necesary  to be here because fix the height
		float alpha = 0.0f;
		float delta = 0.0f;
		planet->getPolarAngles(newPosition, alpha, delta);
		float height = planet->getHeight(alpha, delta);
		finalPosition = center + height*newLocalUpVector;
	}

	// Set the new transform matrix
	const D3DXMATRIX &transform = object.getTransformMatrix();
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

	object.setTransformMatrix(newTransform);
	object.setPosition(finalPosition);
}

void PlayerAnimation::solidCollisionDetected( AnimatedObject &object, const D3DXVECTOR3 &normal ) {
	const D3DXMATRIX &previousTransformMatrix = object.getPreviousTransformMatrix();
	D3DXVECTOR3	previousPosition( previousTransformMatrix(3,0), previousTransformMatrix(3,1), previousTransformMatrix(3,2) );

	D3DXVECTOR3 positionVector = object.getPosition() - previousPosition;
	//Global::normalizeVector3(positionVector, positionVector);
	float positionNormalDot = D3DXVec3Dot(&positionVector, &normal);

	D3DXVECTOR3 movement(0.0f, 0.0f, 0.0f);

	// If previous and current position are differents
	// and position vector and normal dot product is less than 0,
	// then correct position
	if( object.getPosition()!=previousPosition && positionNormalDot<0.0f )
	{
		float e = 0.3f;

		movement = (1.0f+e) * positionNormalDot * normal;
		// Obtain new position
		D3DXVECTOR3 newPosition = previousPosition + (positionVector - movement);
		correctPosition(object, newPosition);
	}
}

void PlayerAnimation::fireballCollisionDetected( AnimatedObject &object ) {
	//MessageBox( NULL, "Collision Player-Fireball", "COLLISION", MB_ICONERROR | MB_OK );
	LogicManager::getLogicManager()->createUserEvent("on_player_fired");
}

bool PlayerAnimation::isJumping(AnimatedObject &object) const {
	const std::string &current_cycle = object.getCurrentCycle();

	return (current_cycle == "jumping");
}
