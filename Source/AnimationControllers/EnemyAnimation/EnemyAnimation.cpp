#include "EnemyAnimation.h"
#include "Managers/PlanetManager.h"
#include "Managers/SoundManager.h"
#include "Object/Planet/Planet.h"
#include "AnimationControllers/EnemyAnimation/StatesDefines.h"
#include "Managers/ParticlesManager.h"
#include "AIControllers/ArachnidBehaviour/ArachnidBehaviour.h"


std::string EnemyAnimation::controllerType = "enemy_animation";

EnemyAnimation::EnemyAnimation(float pRotationSpeed,
							   float pJumpHeight)
							   : IAnimationController()
							   , positionNotUsed(0.0f, 0.0f, 0.0f)
							   , rotation(0.0f, 0.0f, 0.0f, 1.0f)
							   , acceleration(20.0f), deceleration(-35.0f)
							   , rotationSpeed(pRotationSpeed)
							   , whereToFollow(0.0f, 0.0f, 0.0f)
							   , maxSpeed(0.0f)
							   , jumpTime(0.0f)
							   //, jumpDistance(0.0f)
							   , jumpDurationTime(0.6f)
							   , amplitude(pJumpHeight)
							   , frequency(1.0f)//pFrequency)
							   //, jumpPrevPosition(0.0f, 0.0f, 0.0f)
							   , variation(0.0f)
							   , jumpDistance(0.0f)
							   , foolishedFromExpel(false)
{
}

void EnemyAnimation::updateGraph(AnimatedObject &object, float deltaTime )
{
	AnimationGraph * animationGraph = object.getAnimationGraph();

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

		if (animationGraph->isExecutingAction() && object.getCalModel()->getMixer()->getAnimationActionList().empty()) {
			const std::string &cycleName = animationGraph->getNextCycle();
			const std::string &actionName = animationGraph->getNextAction();

			assert( !cycleName.empty() || !actionName.empty() );

			animationGraph->setRelationToState(cycleName, actionName, true);
		}

		float animTimeFactor;

		float objCurrentSpeed = object.getCurrentSpeed();
		if (objCurrentSpeed != 0)
			animTimeFactor = object.getSpeedMultiplier()*objCurrentSpeed/(1.0f+objCurrentSpeed);
		else
			animTimeFactor = object.getSpeedMultiplier();

		calModel->getMixer()->setTimeFactor(animTimeFactor);
		calModel->getMixer()->setActionsTimeFactor(animTimeFactor);

		calModel->update(deltaTime, object.getDrawn());

		// Reset actions time factor and speed multiplier
		calModel->getMixer()->setActionsTimeFactor(1.0f);
		object.setSpeedMultiplier(object.getDefaultSpeedMultiplier());
	}
}

void EnemyAnimation::update(AnimatedObject &object, float deltaTime ) 
{
	updateGraph(object, deltaTime);

	const std::string &currentCycle = object.getCurrentCycle();

	if( ( currentCycle == ANIM_WALK )
		|| ( currentCycle == ANIM_SCARED ) )
	{
		// we have to move the object to the target position
		follow( object, whereToFollow, maxSpeed, deltaTime );
	} 
	else if( currentCycle == ANIM_FLY_DOWNWARDS )
	{ // attack
		// we have to move the object to the target position
		// and move it up
		moveObject(object, 5.0f, deltaTime);
		jumpFoolished( object, deltaTime, true );
	}
	else if( currentCycle == ANIM_FLY_UPWARDS )
	{ // rest of jumps
		// we have to move the object to the target position
		// and move it up

		hit( object, whereToFollow, maxSpeed, deltaTime );
		jumpFoolished( object, deltaTime );
	}
	else if( currentCycle == ANIM_VICTORY )
	{
		jump( object, deltaTime );
	}
	else if( currentCycle == ANIM_ATTACK_OK )
	{
		fallDown( object, deltaTime );
	}
	else if( currentCycle == ANIM_EMERGE )
	{
		emerge( object, deltaTime );
	}
	else if( currentCycle == ANIM_PREPARE_ATTACK )
	{
		alignObject( object, whereToFollow );
	}	

	const std::string &currentAction = object.getCurrentAction();
	if( currentAction == ANIM_FOOLISHED && currentCycle != ANIM_VICTORY )
	{
		object.setSpeedMultiplier(4.0f);
	}
}

void EnemyAnimation::emerge( AnimatedObject &object, float deltaTime )
{
	static const float speedEmerge = 0.18f;

	const Planet *planet = PlanetManager::getPlanetManager()->getCurrentPlanet();
	const D3DXVECTOR3 &planetCenter = planet->getCenter();
	const D3DXVECTOR3 &objPos = object.getPosition();
	float distance = Global::distanceSqrt(objPos , planetCenter );
	float height = planet->getHeight( object.getAlpha(), object.getDelta() );

	if( deltaTime != 0 && distance < height )
	{	
		D3DXVECTOR3 position = objPos + (object.getLocalUpVector()*deltaTime*speedEmerge);
		object.setPosition( position );
	}
	else if( distance >= height )
	{
		object.setRelationToState( CYCLE, ANIM_IDLE, false, false );
	}
}

void EnemyAnimation::fallDown( AnimatedObject &object, float deltaTime )
{
	static const float speedFallDown = 4.0f;

	const Planet *planet = PlanetManager::getPlanetManager()->getCurrentPlanet();
	const D3DXVECTOR3 &planetCenter = planet->getCenter();
	const D3DXVECTOR3 &objPos = object.getPosition();
	float distance = Global::distanceSqrt( objPos , planetCenter );
	float height = planet->getHeight( object.getAlpha(), object.getDelta() );

	if( deltaTime != 0 && distance > height )
	{
		D3DXVECTOR3 position = objPos - object.getLocalUpVector()*deltaTime*speedFallDown;
		object.setPosition( position );
	}
	else if( distance <= height )
	{
		// here, the jump has finished
		//		object.setRelationToState( CYCLE, ANIM_IDLE, false, false );
		object.setRelationToState( ACTION, ANIM_FOOLISHED, false, true );
	}

	// We have cut the jump, so initiate its variables
	jumpTime=0.0f;
	variation=0.0f;
}

void EnemyAnimation::jump(AnimatedObject &object, float deltaTime, bool attack)
{
	const Planet *planet = PlanetManager::getPlanetManager()->getCurrentPlanet();
	const D3DXVECTOR3 &planetCenter = planet->getCenter();
	float distance = 0.0f;
	float height = 0.0f;
	if( variation!=0.0f ) 
	{
		distance = Global::distanceSqrt( planetCenter+variation*object.getLocalUpVector(), planetCenter );
		height = planet->getHeight(object.getAlpha(), object.getDelta());
	}
	// The jump has finished if distance to center planet y less-equal than heightmap and
	// enemy is down in the jump (jumpTime>D3DX_PI/2.0f). This avoids enemies stop in jump
	// when they are elevating.
	if( variation!=0.0f && distance<=height && jumpTime>D3DX_PI/2.0f )
	{
		// here, the jump has finished
		jumpTime=0.0f;
		variation=0.0f;

		ParticlesManager *pm = ParticlesManager::getParticlesManager();
	//	pm->addParticle("enemyFoolished", &object, 6);

		if( attack ) 
			object.setRelationToState( ACTION, ANIM_ATTACK_FAILED, false, true );
		else
			object.setRelationToState( ACTION, ANIM_FOOLISHED, false, true );
			
		//object.setCurrentSpeed(0.0f);
		return;
	}
	if( deltaTime!=0 ) 
	{
		assert(jumpDurationTime!=0.0f);
		jumpTime = jumpTime + (D3DX_PI/jumpDurationTime) * deltaTime;

		float a = -4*amplitude/(D3DX_PI*D3DX_PI);
		float x = frequency*jumpTime-D3DX_PI/2.0f;
		float nextStepVariation = a*x*x + amplitude;

		//static float jumpDistance = 0.0f;
		if(variation==0.0f)
			jumpDistance = Global::distanceSqrt( object.getPosition(), planetCenter );

		variation = (jumpDistance + nextStepVariation);
		D3DXVECTOR3 position = planetCenter + variation*object.getLocalUpVector();
		object.setPosition(position);
	}
}

void EnemyAnimation::jumpFoolished(AnimatedObject &object, float deltaTime, bool attack)
{
	const Planet *planet = PlanetManager::getPlanetManager()->getCurrentPlanet();
	const D3DXVECTOR3 &planetCenter = planet->getCenter();
	float distance = 0.0f;
	float height = 0.0f;
	if( variation!=0.0f ) 
	{
		distance = Global::distanceSqrt( planetCenter+variation*object.getLocalUpVector(), planetCenter );
		height = planet->getHeight(object.getAlpha(), object.getDelta());
	}
	// The jump has finished if distance to center planet y less-equal than heightmap and
	// enemy is down in the jump (jumpTime>D3DX_PI/2.0f). This avoids enemies stop in jump
	// when they are elevating.
	if( variation!=0.0f && distance<=height && jumpTime>D3DX_PI/2.0f )
	{
		// here, the jump has finished
		jumpTime=0.0f;
		variation=0.0f;

		if( attack ) 
		{
			object.setRelationToState( ACTION, ANIM_ATTACK_FAILED, false, true );
		}
		else
		{
			if (!foolishedFromExpel)
			{
				ParticlesManager *pm = ParticlesManager::getParticlesManager();
				pm->addParticle("enemyFoolished", &object, false);
			}

			object.setRelationToState( ACTION, ANIM_FOOLISHED, false, true );
		}
			
		//object.setCurrentSpeed(0.0f);
		return;
	}
	if( deltaTime!=0 ) 
	{
		assert(jumpDurationTime!=0.0f);
		jumpTime = jumpTime + (D3DX_PI/jumpDurationTime) * deltaTime;

		float a = -4*amplitude/(D3DX_PI*D3DX_PI);
		float x = frequency*jumpTime-D3DX_PI/2.0f;
		float nextStepVariation = a*x*x + amplitude;

		//static float jumpDistance = 0.0f;
		if(variation==0.0f)
			jumpDistance = Global::distanceSqrt( object.getPosition(), planetCenter );

		variation = (jumpDistance + nextStepVariation);
		D3DXVECTOR3 position = planetCenter + variation*object.getLocalUpVector();
		object.setPosition(position);
	}
}

void EnemyAnimation::follow( AnimatedObject &object, const D3DXVECTOR3 &aim, float maxSpeed, float deltaTime ) 
{
	directObject(object, aim, deltaTime);

	if(maxSpeed >= object.getCurrentSpeed() ) // Accelerate inertia
	{
		object.setCurrentSpeed(object.getCurrentSpeed() + acceleration*deltaTime);
		if(object.getCurrentSpeed()>maxSpeed)
			object.setCurrentSpeed(maxSpeed);
	}
	else // Deceleration inertia
	{
		object.setCurrentSpeed(object.getCurrentSpeed() + deceleration*deltaTime);
		if(object.getCurrentSpeed()<0)
			object.setCurrentSpeed(0);
	}

	moveObject(object, object.getCurrentSpeed(), deltaTime);
}

void EnemyAnimation::directObject( AnimatedObject &object, const D3DXVECTOR3 &aim, float deltaTime ) 
{
	const Planet * planet = PlanetManager::getPlanetManager()->getCurrentPlanet();
	const D3DXVECTOR3 &center = planet->getCenter();
	assert( aim != center && aim != D3DXVECTOR3(0,0,0) );

	// D parameter of plane ecuation with up with normal
	const D3DXVECTOR3 &position = object.getPosition();
	const D3DXVECTOR3 &up = object.getLocalUpVector();
	float D = - (up.x*position.x + up.y*position.y + up.z*position.z);

	// t value
	float t = -(up.x*aim.x + up.y*aim.y + up.z*aim.z + D) / (up.x*up.x + up.y*up.y + up.z*up.z);

	// Point in plane
	D3DXVECTOR3 pointPlane;
	pointPlane.x = aim.x + t*up.x;
	pointPlane.y = aim.y + t*up.y;
	pointPlane.z = aim.z + t*up.z;

	// Direction of local front vector
	//assert( pointPlane != position );
	if( pointPlane == position )
		return;
	D3DXVECTOR3 localFrontVector = pointPlane - position;
	Global::normalizeVector3(localFrontVector, localFrontVector);

	D3DXVECTOR3 delta = localFrontVector - object.getLocalFrontVector();

	localFrontVector = object.getLocalFrontVector() + delta*rotationSpeed*deltaTime;
	Global::normalizeVector3(localFrontVector, localFrontVector);

	// Obtain Player's new left vector
	D3DXVECTOR3 newLocalLeftVector;
	D3DXVec3Cross(&newLocalLeftVector, &up, &localFrontVector);
	Global::normalizeVector3(newLocalLeftVector, newLocalLeftVector);

	// Obtain Player's new front vector
	D3DXVECTOR3 newLocalFrontVector;
	D3DXVec3Cross(&newLocalFrontVector, &newLocalLeftVector, &up);
	Global::normalizeVector3(newLocalFrontVector, newLocalFrontVector);

	// Set the new transform matrix
	const D3DXMATRIX &transform = object.getTransformMatrix();
	D3DXMATRIX newTransform = transform;
	newTransform(0,0) = newLocalLeftVector.x;
	newTransform(0,1) = newLocalLeftVector.y;
	newTransform(0,2) = newLocalLeftVector.z;
	newTransform(2,0) = newLocalFrontVector.x;
	newTransform(2,1) = newLocalFrontVector.y;
	newTransform(2,2) = newLocalFrontVector.z;

	// Set new transform matrix and new position
	object.setTransformMatrix(newTransform);
}

void EnemyAnimation::alignObject( AnimatedObject &object, const D3DXVECTOR3 &aim ) 
{
	const D3DXVECTOR3 &position = object.getPosition();

	assert( aim != position && aim != D3DXVECTOR3(0,0,0) );
	D3DXVECTOR3 newFront = aim - position;
	Global::normalizeVector3( newFront, newFront );

	const Planet * planet = PlanetManager::getPlanetManager()->getCurrentPlanet();
	const D3DXVECTOR3 &planetCenter = planet->getCenter();

	D3DXVECTOR3 newUp = position - planetCenter;
	Global::normalizeVector3( newUp, newUp );

	D3DXVECTOR3 newLeft;
	D3DXVec3Cross(&newLeft, &newUp, &newFront);
	Global::normalizeVector3( newLeft, newLeft );

	D3DXVec3Cross(&newFront, &newLeft, &newUp);

	// Set the new transform matrix
	const D3DXMATRIX &transform = object.getTransformMatrix();
	D3DXMATRIX newTransform = transform;
	newTransform(0,0) = newLeft.x;
	newTransform(0,1) = newLeft.y;
	newTransform(0,2) = newLeft.z;
	newTransform(2,0) = newFront.x;
	newTransform(2,1) = newFront.y;
	newTransform(2,2) = newFront.z;

	// Set new transform matrix and new position
	object.setTransformMatrix(newTransform);
}


void EnemyAnimation::moveObject( AnimatedObject &object, float speed, float deltaTime ) 
{

	// New position (not a point in the sphere) with inertia
	const D3DXVECTOR3 &position = object.getPosition();
	const D3DXVECTOR3 &localFrontVector = object.getLocalFrontVector();
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

	// Obtain Object's new up vector
	assert( newPosition != center );
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
		//finalPosition = 0.7f*newPosition+0.3f*finalPosition;
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

void EnemyAnimation::collisionDetected( AnimatedObject &object, const D3DXVECTOR3 &normal, D3DXVECTOR3 &additionalMovementInOut )
{
	const D3DXMATRIX &previousTransformMatrix = object.getPreviousTransformMatrix();
	D3DXVECTOR3	previousPosition( previousTransformMatrix(3,0), previousTransformMatrix(3,1), previousTransformMatrix(3,2) );

	D3DXVECTOR3 positionVector = object.getPosition() - previousPosition;
	//Global::normalizeVector3(positionVector, positionVector);
	float positionNormalDot = D3DXVec3Dot(&positionVector, &normal);

	// If previous and current position are differents
	// and position vector and normal dot product is less than 0,
	// then correct position
	if( Global::distanceSqrt(object.getPosition(), previousPosition) > 0.001f &&
		positionNormalDot<0.0f )
	{
		float e = 0.0f;

		D3DXVECTOR3 movement = (1.0f+e) * positionNormalDot * normal;
		// Obtain new position
		D3DXVECTOR3 newPosition = previousPosition + ( (positionVector - movement) + additionalMovementInOut );
		correctPosition(object, newPosition);
	}
	else if ( additionalMovementInOut!=D3DXVECTOR3(0.0f, 0.0f, 0.0f) ) 
	{
		// Obtain new position
		D3DXVECTOR3 newPosition = previousPosition + additionalMovementInOut;
		correctPosition(object, newPosition);
	}

	// EnemyAnimation do not leave additional movement
	additionalMovementInOut=D3DXVECTOR3(0.0f, 0.0f, 0.0f);
}

void EnemyAnimation::correctPosition( AnimatedObject &object, const D3DXVECTOR3 &newPosition ) 
{
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

bool EnemyAnimation::isJumping(AnimatedObject &object) {
	const std::string &currentCycle = object.getCurrentCycle();

	if( currentCycle == ANIM_FLY_DOWNWARDS ||
		currentCycle == ANIM_FLY_UPWARDS ||
		currentCycle == ANIM_VICTORY ||
		currentCycle == ANIM_ATTACK_OK )
		return true;
	else
		return false;
}

void EnemyAnimation::hit (AnimatedObject &object, const D3DXVECTOR3 &aim, float speed, float deltaTime) 
{
	const Planet * planet = PlanetManager::getPlanetManager()->getCurrentPlanet();
	const D3DXVECTOR3 &center = planet->getCenter();
	assert( aim != center && aim != D3DXVECTOR3(0,0,0) );

	// New position (not a point in the sphere) with inertia
	const D3DXVECTOR3 &position = object.getPosition();
	assert( aim != position );
	D3DXVECTOR3 localFrontVector = aim - position;
	Global::normalizeVector3(localFrontVector, localFrontVector);
	D3DXVECTOR3 newPosition = position + localFrontVector*speed*deltaTime;

	// Current planet center and radius
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
