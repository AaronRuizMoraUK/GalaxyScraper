#ifndef TARGET_POINT_H
#define TARGET_POINT_H

#include "CameraControllers/ICameraController.h"
#include <vector>

/**
* Camera Animation Controller for making camera go to
* a target point
*/

class TargetPoint: public ICameraController
{
protected:
	static std::string controllerType;

	D3DXVECTOR3 endPosition;
	D3DXVECTOR3 endTarget;

	// Current speed and position
	D3DXVECTOR3 currentPosition;
	D3DXVECTOR3 currentTarget;

	float speedPosition;
	float speedRotation;

public:
    TargetPoint( const D3DXVECTOR3 &pEndPosition, const D3DXVECTOR3 &pEndTarget );
    ~TargetPoint() {
		OutputDebugString("Destroying TargetPoint controller...\n");
	};
    
    void update(Camera &camera, float deltaTime=(1.0f/60.0f));
    bool isFinished();

	const std::string & getControllerType () const { return controllerType; };
};

#endif // TARGET_POINT_H
