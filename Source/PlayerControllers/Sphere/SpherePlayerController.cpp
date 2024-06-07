#include "SpherePlayerController.h"
#include "Global/GlobalFunctions.h"
#include "Global/GlobalVariables.h"
#include "Managers/particlesManager.h"

SpherePlayerController::SpherePlayerController(float pCurrentAngle) : IPlayerController(), 
currentAngle(pCurrentAngle),
//rotationSpeed(10.0f),
firstMomentInJump( true ),
angleInFirstMomentInJump( 0.0f )
{
}

void SpherePlayerController::update(AnimatedObject &object, float deltaTime) {
	if(Global::ioStatus.kick.becomesPressed())
	{
		// state |= 8;
		object.setRelationToState(ACTION, "kick", true);
	}

	if(Global::ioStatus.jump.becomesPressed())
	{
		// state |= 4;
		object.setRelationToState(ACTION, "prepare_jump", false, true);
	}
	// Animation Controller Update

	// Obtain direction by IO status
	const D3DXVECTOR2 &direction = Global::ioStatus.walk.getDirection();

	// If a direction is different to (0,0) set state to walk, else idle
	if(direction!=D3DXVECTOR2(0,0)) {

		// If player is jumping, do not rotat player
		if( !( object.getCurrentAction( ) == "prepare_jump"
			|| object.getCurrentAction( ) == "finish_jump"
			|| object.getCurrentCycle( ) == "jumping" ) ) {
			firstMomentInJump = true;

			static const float rotationSpeed = 10.0f;

			// Rotate player to the direction
			rotatePlayer(object, direction, rotationSpeed, deltaTime);
		}
		else if( object.getCurrentSpeed() != 0.0f ) {
			//static const float rotationSpeedInJump = 40.0f;

			// Rotate player to the direction
			//rotatePlayer(object, direction, rotationSpeedInJump, deltaTime);

			// Get angle to fix direction for moving player in jump
			float directionAngle = atan2(direction.y, direction.x);
			if( directionAngle < 0.0f )
				directionAngle += 2.0f*D3DX_PI;

			if( firstMomentInJump ) {
				angleInFirstMomentInJump = D3DX_PI/2.0f - directionAngle;
				firstMomentInJump = false;
			}
			directionAngle += angleInFirstMomentInJump;

			// Fix direction with angle
			D3DXVECTOR2 directionFixed;
			directionFixed.x = cos( directionAngle );
			directionFixed.y = sin( directionAngle );

			// If player is jumping, correct position with direction
			movePlayerJumping( object, directionFixed, deltaTime );
		}

		if( Global::ioStatus.walkSlow.isPressed() )
			object.setRelationToState(CYCLE, "walk", false);
		else
			object.setRelationToState(CYCLE, "jog", false);
	}
	else
		object.setRelationToState(CYCLE, "idle", false);
}

void SpherePlayerController::rotatePlayer(AnimatedObject &object, const D3DXVECTOR2 &direction, float rotationSpeed, float deltaTime) {
	// Obtain new angle
	float newAngle = atan2(direction.y, direction.x);

	// Fix angles
	float currentAngleFixed=currentAngle;
	float newAngleFixed=newAngle;
	if(abs(newAngle-currentAngle)>D3DX_PI) {
		if(newAngleFixed<0)
			newAngleFixed+=2.0f*D3DX_PI;
		else
			currentAngleFixed+=2.0f*D3DX_PI;
	}

	// Angle inertia
	float deltaAngle = newAngleFixed-currentAngleFixed;
	newAngleFixed = currentAngleFixed + deltaAngle*deltaTime*rotationSpeed;

	// Obtain delta angle to rotate
	float deltaAngleRotation = newAngleFixed-currentAngleFixed;

	// Update current angle
	if(abs(newAngle-currentAngle)>D3DX_PI && newAngleFixed>D3DX_PI)
		newAngleFixed+=-2.0f*D3DX_PI;
	currentAngle = newAngleFixed;

	/*char msg[ 128 ];
	sprintf_s(msg, sizeof(msg), "deltaAngleRotation: %f \n", deltaAngleRotation);
	OutputDebugString(msg);*/

	if(deltaAngleRotation!=0.0f) {
		D3DXMATRIX rotateDeltaAngle;
		D3DXMatrixIdentity(&rotateDeltaAngle);
		const D3DXVECTOR3 &localUpVector = object.getLocalUpVector();
		D3DXMatrixRotationAxis(&rotateDeltaAngle, &localUpVector, deltaAngleRotation);

		// Save the position
		D3DXVECTOR3 position = object.getPosition();

		// Rotate transform matrix
		const D3DXMATRIX &transform = object.getTransformMatrix();
		D3DXMatrixMultiply(&rotateDeltaAngle, &transform, &rotateDeltaAngle);

		object.setTransformMatrix(rotateDeltaAngle);
		object.setPosition(position); // Set the position
	}
}

void SpherePlayerController::movePlayerJumping(AnimatedObject &object, const D3DXVECTOR2 &direction, float deltaTime) {
	const D3DXVECTOR3 &playerLocalFront = object.getLocalFrontVector( );
	const D3DXVECTOR3 &playerLocalLeft  = object.getLocalLeftVector( );

	static const float speedX = 4.0f;
	static const float speedY = 6.0f;
	//static const float inertia = 0.8f;

	D3DXVECTOR3 finalMovement( 0.0f, 0.0f, 0.0f );

	if( direction.y < 0.0f )
		finalMovement += playerLocalFront * direction.y * speedY;
	finalMovement -= playerLocalLeft * direction.x * speedX;

	//finalMovement = inertia * object.getPosition()
	//	+ (1.0f-inertia) * ( object.getPosition() + finalMovement * deltaTime );
	finalMovement = object.getPosition() + finalMovement * deltaTime;

	object.setPosition( finalMovement );
}