#ifndef KEYFRAMED_ROA_H
#define KEYFRAMED_ROA_H

#include "AnimationControllers/IAnimationController.h"
#include "RigidAnimationResource.h"

/**
* Animation Controller for making animated objects follow
* a secuence of keyframed prerecorded animation
*/

class KeyFramedROA: public IAnimationController
{
protected:
	static std::string controllerType;

    const RigidAnimationResource * rigidAnimationResource;
    float currentTime;
    float timeScale;
    D3DXVECTOR3 position, positionSync;
    D3DXQUATERNION rotation, rotationSync;

public:
    KeyFramedROA( const RigidAnimationResource * pRigidAnimationResource );
    KeyFramedROA( const RigidAnimationResource * pRigidAnimationResource
		, const D3DXVECTOR3 &initialPosition, const D3DXQUATERNION &initialRotation );

    ~KeyFramedROA() {
		OutputDebugString("Destroying Key Framed ROA controller...\n");

		// KeyFramedROA does not have to delete RigidAnimationResource 
	};
    
	const std::string & getControllerType () const { return controllerType; };

    void update(AnimatedObject &object, float deltaTime=(1.0f/60.0f));
    bool isFinished() const;
    const D3DXVECTOR3 &getPosition() const { return position; };
    const D3DXQUATERNION &getRotation() const { return rotation; };

    void restart( const D3DXVECTOR3 &initialPosition, const D3DXQUATERNION &initialRotation );
	void setTimeScale( float pTimeScale ) { timeScale = pTimeScale; };
};

#endif // KEYFRAMED_ROA_H
