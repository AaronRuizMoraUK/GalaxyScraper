#ifndef IPLAYER_CONTROLLER_H
#define IPLAYER_CONTROLLER_H

#include "Object/CollisionableObject/AnimatedObject/AnimatedObject.h"

/**
* Player Controller
*/

class IPlayerController
{

public:
	IPlayerController() {};
	virtual ~IPlayerController() {
		OutputDebugString("Destroying player controller...\n");
	};

	/**
	* Update the player with a delta time.
	* 
	* @param Player player
	* @param deltaTime Delta time for update
	*/
	virtual void update(AnimatedObject &object, float deltaTime=(1.0f/60.0f)) = 0;

	virtual bool isFinished() const {return false;};
	virtual void restart( const D3DXVECTOR3 &initialPosition, const D3DXQUATERNION &initialRotation ) {};
	virtual void restart( ) {};
};

#endif //IPLAYER_CONTROLLER_H