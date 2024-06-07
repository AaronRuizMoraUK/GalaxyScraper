#ifndef COLLISION_MANAGER_H
#define COLLISION_MANAGER_H

#include <Windows.h>

/**
* Interface of managers of collision events.
*/

class CollisionManager
{
public:
	CollisionManager() {};
	virtual ~CollisionManager() {
		OutputDebugString("Destroying CollisionManager...\n");
	};

	virtual void detectCollisions() = 0;
	virtual void treatCollisions() = 0;
	
	virtual void clean() = 0;
};

#endif //COLLISION_MANAGER_H
