#ifndef PLAYER_KEYFRAMED_ROA_H
#define PLAYER_KEYFRAMED_ROA_H

#include "PlayerControllers/IPlayerController.h"
#include "AnimationControllers/KeyFramed/RigidAnimationResource.h"

/**
* Player Controller for making player follows
* a secuence of keyframed prerecorded animation
*/

class PlayerKeyFramedROA: public IPlayerController
{
protected:
    const RigidAnimationResource * rigidAnimationResource;
    float currentTime;
    float timeScale;
    D3DXVECTOR3 position, positionSync;
    D3DXQUATERNION rotation, rotationSync;

public:
    PlayerKeyFramedROA( const RigidAnimationResource * pRigidAnimationResource );
    PlayerKeyFramedROA( const RigidAnimationResource * pRigidAnimationResource
		, const D3DXVECTOR3 &initialPosition, const D3DXQUATERNION &initialRotation );

    ~PlayerKeyFramedROA() {
		OutputDebugString("Destroying Player Key Framed ROA controller...\n");

		// PlayerKeyFramedROA does not have to delete RigidAnimationResource 
	};
    
    void update(AnimatedObject &object, float deltaTime=(1.0f/60.0f));
    bool isFinished() const;

    void restart( const D3DXVECTOR3 &initialPosition, const D3DXQUATERNION &initialRotation );
    void restart( );
	void setTimeScale( float pTimeScale ) { timeScale = pTimeScale; };
};

#endif // PLAYER_KEYFRAMED_ROA_H
