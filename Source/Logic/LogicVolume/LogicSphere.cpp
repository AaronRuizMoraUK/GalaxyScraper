#include "LogicSphere.h"
#include "Global/GlobalFunctions.h"
#include "Object/CollisionableObject/AnimatedObject/Player/Player.h"

LogicSphere::LogicSphere(const std::string &pVolumeName, bool pCheckForPlayer, bool pCheckIsInside, const D3DXVECTOR3 &pCenter, float pRadius) 
: LogicVolume(pVolumeName, pCheckForPlayer, pCheckIsInside)
, center(pCenter)
, radius(pRadius)
, parentObject( NULL )
, centerLocal(pCenter)
{
}

bool LogicSphere::checkInside() const
{
	Player *player = Player::getPlayer();
	const D3DXVECTOR3 &playerPosition = player->getPosition();
	float playerDistance = Global::distanceSqrt(playerPosition, center);
	return ( playerDistance < radius );
}

bool LogicSphere::checkObjectInsideVolume(const Object &obj) const
{
	const D3DXVECTOR3 &objPos = obj.getPosition();

	float distance = Global::distanceSqrt( center, objPos );
	return (distance<radius);
}
