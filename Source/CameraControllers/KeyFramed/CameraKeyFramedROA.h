#ifndef CAMERA_KEYFRAMED_ROA_H
#define CAMERA_KEYFRAMED_ROA_H

#include "CameraControllers/ICameraController.h"
#include "AnimationControllers/KeyFramed/RigidAnimationResource.h"

/**
* Camera Animation Controller for making camera follows
* a secuence of keyframed prerecorded animation
*/

class CameraKeyFramedROA: public ICameraController
{
protected:
	static std::string controllerType;

    const RigidAnimationResource * rigidAnimationResource;
    float currentTime;
    float timeScale;
    D3DXVECTOR3 position, positionSync;
    D3DXQUATERNION rotation, rotationSync;

public:
    CameraKeyFramedROA( const RigidAnimationResource * pRigidAnimationResource );
    CameraKeyFramedROA( const RigidAnimationResource * pRigidAnimationResource
		, const D3DXVECTOR3 &initialPosition, const D3DXQUATERNION &initialRotation );

    ~CameraKeyFramedROA() {
		OutputDebugString("Destroying Camera Key Framed ROA controller...\n");

		// CameraKeyFramedROA does not have to delete RigidAnimationResource 
	};
    
    void update(Camera &camera, float deltaTime=(1.0f/60.0f));
    bool isFinished();

    void restart( const D3DXVECTOR3 &initialPosition, const D3DXQUATERNION &initialRotation );
	void setTimeScale( float pTimeScale ) { timeScale = pTimeScale; };

	const std::string & getControllerType () const { return controllerType; };
};

#endif // CAMERA_KEYFRAMED_ROA_H
