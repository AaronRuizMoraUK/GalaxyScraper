#include "TargetPoint.h"
#include "Global/GlobalFunctions.h"
#include "Global/GlobalVariables.h"

std::string TargetPoint::controllerType = "target_point";

TargetPoint::TargetPoint( const D3DXVECTOR3 &pEndPosition, const D3DXVECTOR3 &pEndTarget )
: ICameraController()
, endPosition(pEndPosition)
, endTarget(pEndTarget)
, currentPosition(0.0f, 0.0f, 0.0f)
, currentTarget(0.0f, 0.0f, 0.0f)
, speedPosition(3.0f)
, speedRotation(2.0f)
{
}

void TargetPoint::update( Camera &camera, float deltaTime ){
	speedPosition += deltaTime;
	speedRotation += deltaTime;

	D3DXVECTOR3 newPosition = 0.6f*camera.getPosition() + 0.4f*endPosition;
	D3DXVECTOR3 newTarget = 0.6f*camera.getTarget() + 0.4f*endTarget;

	newPosition = camera.getPosition() + ( newPosition - camera.getPosition() ) * speedPosition * deltaTime;
	newTarget = camera.getTarget() + ( newTarget - camera.getTarget() ) * speedRotation * deltaTime;

	camera.lookAt( newPosition, newTarget );
	currentPosition = newPosition;
	currentTarget = newTarget;
}

bool TargetPoint::isFinished() { 
	bool isFinished=false;

	if ( Global::distanceSqrt(currentPosition, endPosition)<0.01f && 
		Global::distanceSqrt(currentTarget, endTarget)<0.01f ) {
		Global::camera->lookAt(endPosition, endTarget);

		isFinished = true;

		speedPosition = 3.0f;
		speedRotation = 2.0f;
	}

	return isFinished;
}
