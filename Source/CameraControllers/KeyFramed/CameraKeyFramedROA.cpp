#include "CameraKeyFramedROA.h"
#include "Global/GlobalFunctions.h"

std::string CameraKeyFramedROA::controllerType = "camera_keyframed_roa";

CameraKeyFramedROA::CameraKeyFramedROA( const RigidAnimationResource * pRigidAnimationResource )
: ICameraController()
, rigidAnimationResource( pRigidAnimationResource )
, positionSync(0,0,0)
, rotationSync(0,0,0,1)
, currentTime( 0 )
, timeScale( 1 )
{
	//update(0);
	// Obtain sample of position and rotation in current time (0) said by animation
	D3DXVECTOR3 positionFrameCurrentTime;
	D3DXQUATERNION rotationFrameCurrentTime;
	rigidAnimationResource->getPosition( currentTime, positionFrameCurrentTime );
	rigidAnimationResource->getRotation( currentTime, rotationFrameCurrentTime );

	// Update position and rotation
	rotation = rotationFrameCurrentTime * rotationSync;
	D3DXVECTOR3 positionFrameCurrentTimeRotateSync;
	Global::rotateVector3Quaternion(positionFrameCurrentTimeRotateSync, positionFrameCurrentTime, rotationSync);
	position = positionSync + positionFrameCurrentTimeRotateSync;
}

CameraKeyFramedROA::CameraKeyFramedROA( const RigidAnimationResource * pRigidAnimationResource
						   , const D3DXVECTOR3 &initialPosition, const D3DXQUATERNION &initialRotation)
: ICameraController()
, rigidAnimationResource( pRigidAnimationResource )
, positionSync(0,0,0)
, rotationSync(0,0,0,1)
, currentTime( 0 )
, timeScale( 1 )
{
	restart(initialPosition, initialRotation);
}

void CameraKeyFramedROA::update( Camera &camera, float deltaTime )
{
	// Obtain sample of position and rotation in current time said by animation
	currentTime += deltaTime*timeScale;
	D3DXVECTOR3 positionFrameCurrentTime;
	D3DXQUATERNION rotationFrameCurrentTime;
	rigidAnimationResource->getPosition( currentTime, positionFrameCurrentTime );
	rigidAnimationResource->getRotation( currentTime, rotationFrameCurrentTime );

	// Update position and rotation
	rotation = rotationFrameCurrentTime * rotationSync;
	D3DXVECTOR3 positionFrameCurrentTimeRotateSync;
	Global::rotateVector3Quaternion(positionFrameCurrentTimeRotateSync, positionFrameCurrentTime, rotationSync);
	position = positionSync + positionFrameCurrentTimeRotateSync;

	// Update object
	//if( isFinished() )
	//	restart( position, rotation );
	D3DXMATRIX newRotation;
	D3DXMatrixRotationQuaternion(&newRotation, &rotation);
	D3DXVECTOR3 localFront( newRotation(2,0), newRotation(2,1), newRotation(2,2) );
	D3DXVECTOR3 target = position+localFront;
	camera.lookAt(position, target);

	//char msg[ 128 ];
	//sprintf_s(msg, sizeof(msg), "Position: (%1.2f,%1.2f,%1.2f) Target: (%1.2f,%1.2f,%1.2f)\n", position.x, position.y, position.z, target.x, target.y, target.z);
	//OutputDebugString(msg);
}

void CameraKeyFramedROA::restart( const D3DXVECTOR3 &initialPosition, const D3DXQUATERNION &initialRotation )
{
	// Sample start time
	currentTime = 0;
	D3DXVECTOR3 positionFrame0;
	D3DXQUATERNION rotationFrame0;

	rigidAnimationResource->getPosition( 0, positionFrame0 );
	rigidAnimationResource->getRotation( 0, rotationFrame0 );

	// Compute corrections so that:
	// r(t) = rs*r(t)      => r(0) = r0 = rs*R(0)   => rs = r0*R(0)^-1
	// p(t) = ps + rs*P(t) => p(0) = p0 = ps + rs*P(0) => ps = p0-rs*P(0)
	D3DXQUATERNION inverseRotationFrame0;
	D3DXQuaternionInverse( &inverseRotationFrame0, &rotationFrame0);
	rotationSync = inverseRotationFrame0 * initialRotation;
	D3DXVECTOR3 positionFrame0RotateSync;
	Global::rotateVector3Quaternion(positionFrame0RotateSync, positionFrame0, rotationSync);
	positionSync = initialPosition - positionFrame0RotateSync;

	// Init cached state
	position = initialPosition;
	rotation = initialRotation;
}

bool CameraKeyFramedROA::isFinished() { 
	if ( currentTime >= rigidAnimationResource->getDuration() )
		return true;

	return false;
}
