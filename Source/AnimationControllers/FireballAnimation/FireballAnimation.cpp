#include "FireballAnimation.h"
#include "Global/GlobalFunctions.h"
#include "Managers/PlanetManager.h"

std::string FireballAnimation::controllerType = "fireball_animation";

FireballAnimation::FireballAnimation()
: IAnimationController()
, position(0.0f, 0.0f, 0.0f)
, rotation(0.0f, 0.0f, 0.0f, 1.0f)
, initialPosition(0.0f, 0.0f, 0.0f)
, verticalSpeed(0.0f)
, horizontalSpeed(0.0f)
, state(A_WAITING_TO_MOVE)
, isFalling(false)
{
}

void FireballAnimation::update(AnimatedObject &object, float deltaTime ) {
	switch (state)
	{
	case A_MOVING:
		move(object, deltaTime);
		break;
	case A_WAITING_TO_MOVE:
		break;
	case A_WAITING_TO_RESTART:
		break;
	default:
		assert( !"This should not happend" );
		return;
		break;
	}
}

void FireballAnimation::move(AnimatedObject &object, float deltaTime)
{
	currentTime += deltaTime;

	float x = horizontalSpeed * currentTime;
	float y = verticalSpeed * currentTime + verticalSpeed * -1.0f * currentTime * currentTime;

	isFalling = (y < 0.0f);
	D3DXVECTOR3 newPos; 

	D3DXVec3TransformCoord( &newPos, &D3DXVECTOR3(x, y, 0), &(transformMatrix) );

	object.setPosition(newPos);
}

void FireballAnimation::resetPosition(AnimatedObject &object)
{
	object.setPosition(initialPosition);
	currentTime = 0.0f;
}


void FireballAnimation::setSpeeds(float pVerticalSpeed, float pHorizontalSpeed)
{
	verticalSpeed = pVerticalSpeed;
	horizontalSpeed = pHorizontalSpeed;

}
