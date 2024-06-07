#include "LogicBox.h"
#include "Object/CollisionableObject/AnimatedObject/Player/Player.h"

LogicBox::LogicBox(const std::string &pVolumeName,  bool pCheckForPlayer, bool pCheckIsInside, const D3DXVECTOR3 &pPMin, const D3DXVECTOR3 &pPMax) 
: LogicVolume(pVolumeName, pCheckForPlayer, pCheckIsInside)
, pMin(0,0,0)
, pMax(0,0,0)
, parentObject( NULL )
, pMinLocal(pPMin)
, pMaxLocal(pPMax)
{
}

bool LogicBox::checkInside() const
{
	Player *player = Player::getPlayer();
	const D3DXVECTOR3 &playerPosition = player->getPosition();

	if( pMin.x < playerPosition.x && playerPosition.x < pMax.x &&
		 pMin.y < playerPosition.y && playerPosition.y < pMax.y &&
		 pMin.z < playerPosition.z && playerPosition.z < pMax.z		)
		 return true;
	return false;
}

bool LogicBox::checkObjectInsideVolume(const Object &obj) const
{
	const D3DXVECTOR3 &objPos = obj.getPosition();
	if ( pMin.x < objPos.x && objPos.x < pMax.x &&
			 pMin.y < objPos.y && objPos.y < pMax.y &&
			 pMin.z < objPos.z && objPos.z < pMax.z	)
		return true;
	return false;
}