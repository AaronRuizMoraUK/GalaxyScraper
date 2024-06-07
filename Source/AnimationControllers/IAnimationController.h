#ifndef IANIMATION_CONTROLLER_H
#define IANIMATION_CONTROLLER_H

#include "Directx/Directx.h"
#include "AnimationControllers/IAnimationController.h"
#include "cal3d/cal3d.h"
#include "Object/CollisionableObject/AnimatedObject/AnimationGraph/AnimationGraph.h"
#include <string>

class AnimatedObject;

/**
* Interface of Animation Controller for animated objects
*/

class IAnimationController
{
public:
	IAnimationController() {};
    virtual ~IAnimationController() {
		OutputDebugString("Destroying Animation controller...\n");
	};

    virtual void update( AnimatedObject &object, float deltaTime=(1.0f/60.0f) ) = 0;
    virtual bool isFinished() const = 0;
	virtual void restart( const D3DXVECTOR3 &initialPosition, const D3DXQUATERNION &initialRotation ) {};
    
	virtual const D3DXVECTOR3 &getPosition() const = 0;
	virtual const D3DXQUATERNION &getRotation() const = 0;
	
	virtual const std::string & getControllerType () const = 0;

	virtual void collisionDetected( AnimatedObject &object, const D3DXVECTOR3 &normal, D3DXVECTOR3 &additionalMovementInOut )
	{
	};
	virtual void collisionDetected(AnimatedObject& object, const D3DXVECTOR3& normal)
	{
	};
	virtual void solidCollisionDetected( AnimatedObject &object, const D3DXVECTOR3 &normal ) {};
	virtual void fireballCollisionDetected( AnimatedObject &object ) {};
};

#include "Object/CollisionableObject/AnimatedObject/AnimatedObject.h"

#endif // IANIMATION_CONTROLLER_H

