#include "PlayerKeyFramedROA.h"
#include "Global/GlobalFunctions.h"

PlayerKeyFramedROA::PlayerKeyFramedROA( const RigidAnimationResource * pRigidAnimationResource )
: IPlayerController()
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

PlayerKeyFramedROA::PlayerKeyFramedROA( const RigidAnimationResource * pRigidAnimationResource
						   , const D3DXVECTOR3 &initialPosition, const D3DXQUATERNION &initialRotation)
: IPlayerController()
, rigidAnimationResource( pRigidAnimationResource )
, positionSync(0,0,0)
, rotationSync(0,0,0,1)
, currentTime( 0 )
, timeScale( 1 )
{
	restart(initialPosition, initialRotation);
}

void PlayerKeyFramedROA::update( AnimatedObject &object, float deltaTime )
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
	object.setTransformMatrix(newRotation);
	object.setPosition(position);

	//char msg[ 128 ];
	//sprintf_s(msg, sizeof(msg), "Position: (%1.2f,%1.2f,%1.2f) Target: (%1.2f,%1.2f,%1.2f)\n", position.x, position.y, position.z, target.x, target.y, target.z);
	//OutputDebugString(msg);
}

void PlayerKeyFramedROA::restart( const D3DXVECTOR3 &initialPosition, const D3DXQUATERNION &initialRotation )
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

void PlayerKeyFramedROA::restart( ) {
	//update(0);
	// Obtain sample of position and rotation in current time (0) said by animation
	currentTime = 0;
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

bool PlayerKeyFramedROA::isFinished() const { 
	if ( currentTime >= rigidAnimationResource->getDuration() )
		return true;

	return false;
}
