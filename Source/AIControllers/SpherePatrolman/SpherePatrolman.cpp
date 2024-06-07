#include "SpherePatrolman.h"
#include "Global/GlobalFunctions.h"
#include "Object/CollisionableObject/AnimatedObject/Player/Player.h"
#include "Managers/PlanetManager.h"
#include "Object/Planet/Planet.h"

std::string SpherePatrolman::controllerType = "sphere_patrolman";

SpherePatrolman::SpherePatrolman(int pNumberWP)
: IAIController()
, numberWP(pNumberWP)
, wayPointId(0)
, state(SEEKWP)
, distanceNextWP(3) //10
, distancePlayer(15)//60
, distancePlayerFight(1.5) //5
, currentSpeed(0)
{
	// Current planet center and radius
	const Planet * planet = PlanetManager::getPlanetManager()->getCurrentPlanet();
	const D3DXVECTOR3 &center = planet->getCenter();
	float radius = planet->getRadius();

	// Obtain random way points in the sphere
	float a=center.x;
	float b=center.y;
	float c=center.z;
	float x, y, z;
	for(int i=0; i<numberWP; ++i) {
		float randomAngle = Global::obtainRandom(-D3DX_PI, D3DX_PI);
		x = Global::obtainRandom(0.0f,radius) * cos(randomAngle);
		z = Global::obtainRandom(0.0f,radius) * sin(randomAngle);

		float ecuationPart = sqrt( b*b - 4.0f*(b*b - radius*radius + pow(x-a,2.0f) + pow(z-c,2.0f)) );

		float random = Global::obtainRandom(0.0f,1.0f);
		if(random>=0.5f)
			y = ( b + ecuationPart ) / 2.0f;
		else
			y = ( b - ecuationPart ) / 2.0f;

		D3DXVECTOR3 *wayPoint = new D3DXVECTOR3(x, y, z);
		vectorWP.push_back(wayPoint);
	}

}

void SpherePatrolman::update(AnimatedObject &object, float deltaTime){
	Player * player = Player::getPlayer();
	const D3DXVECTOR3 &playerPosition= player->getPosition();

	switch (state) {
		case SEEKWP:
			if(Global::distance(object.getPosition(), playerPosition)<distancePlayer) {
				state=FOLLOWPLAYER;
			}
			else {
				if(Global::distance(object.getPosition(), *(vectorWP[wayPointId]))< distanceNextWP)
					state=NEXTWP;
				else
					follow(object, *(vectorWP[wayPointId]), 4, deltaTime);
			}
			break;

		case NEXTWP:
			wayPointId = (wayPointId+1)%numberWP;
			state=SEEKWP;
			break;

		case FOLLOWPLAYER:			
			if(Global::distance(object.getPosition(), playerPosition)>distancePlayer) {
				state=RESETWP;
			}
			else {
				if(Global::distance(object.getPosition(), playerPosition)<distancePlayerFight)
					state=FIGHT;
				else
					follow(object, playerPosition, 8,deltaTime);
			}
			break;

		case FIGHT:
			if( false/*player está muerto*/ ) {
				state=RESETWP;
			}
			else {
				if(Global::distance(object.getPosition(), playerPosition)>distancePlayerFight*3)
					state=FOLLOWPLAYER;
				else
					fight(object, playerPosition, deltaTime);
			}
			break;

		case RESETWP:
			state=NEXTWP;
			break;

		default:
			assert( !"This should not happend" );
			return;
			break;
	}
}

void SpherePatrolman::fight (Object &object, const D3DXVECTOR3 &aim, float deltaTime) {
	follow(object, aim, 0.0f, deltaTime);
}

void SpherePatrolman::follow (Object &object, const D3DXVECTOR3 &aim, float maxSpeed, float deltaTime) {

	directObject(object, aim, deltaTime);

	// Accelerate inertia
	if(maxSpeed >= currentSpeed ) {
		currentSpeed = currentSpeed + 20*deltaTime;
		if(currentSpeed>maxSpeed)
			currentSpeed=maxSpeed;
	}
	// Deceleration inertia
	else {
		currentSpeed = currentSpeed - 35*deltaTime;
		if(currentSpeed<0)
			currentSpeed=0;
	}

	moveObject(object, currentSpeed, deltaTime);
}

void SpherePatrolman::directObject (Object &object, const D3DXVECTOR3 &aim, float deltaTime) {
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
	D3DXVECTOR3 localFrontVector = pointPlane - position;
	Global::normalizeVector3(localFrontVector, localFrontVector);

	D3DXVECTOR3 delta = localFrontVector - object.getLocalFrontVector();
	
	localFrontVector = object.getLocalFrontVector() + delta*5.0f*deltaTime;
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

void SpherePatrolman::moveObject (Object &object, float speed, float deltaTime) {

	// New position (not a point in the sphere) with inertia
	const D3DXVECTOR3 &position = object.getPosition();
	const D3DXVECTOR3 &localFrontVector = object.getLocalFrontVector();
	D3DXVECTOR3 newPosition = position + localFrontVector*speed*deltaTime;

	// Current planet center and radius
	const Planet * planet = PlanetManager::getPlanetManager()->getCurrentPlanet();
	const D3DXVECTOR3 &center = planet->getCenter();
	float radius = planet->getRadius();

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
	D3DXVECTOR3 newLocalUpVector(newPosition.x-center.x, newPosition.y-center.y, newPosition.z-center.z);
	Global::normalizeVector3(newLocalUpVector, newLocalUpVector);

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
	object.setPosition(newPosition);
}

void SpherePatrolman::drawWP() const {
	for(int i=0; i<numberWP; ++i) {
		D3DXVECTOR3 temp1(vectorWP[i]->x, vectorWP[i]->y-0.3f, vectorWP[i]->z);
		D3DXVECTOR3 temp2(vectorWP[i]->x, vectorWP[i]->y+0.3f, vectorWP[i]->z);
		Global::drawLine(temp1, temp2, 0xffaabbcc);
		D3DXVECTOR3 temp3(vectorWP[i]->x-0.3f, vectorWP[i]->y, vectorWP[i]->z);
		D3DXVECTOR3 temp4(vectorWP[i]->x+0.3f, vectorWP[i]->y, vectorWP[i]->z);
		Global::drawLine(temp3, temp4, 0xffaabbcc);
	}
}
