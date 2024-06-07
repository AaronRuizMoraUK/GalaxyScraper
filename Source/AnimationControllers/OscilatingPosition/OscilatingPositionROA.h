#ifndef OSCILATING_POSITION_ROA_H
#define OSCILATING_POSITION_ROA_H

#include "AnimationControllers/IAnimationController.h"

/**
* Animation Controller for making oscilate animated objects
*/

class OscilatingPositionROA: public IAnimationController
{
protected:
	static std::string controllerType;

    float currentTime;
    D3DXVECTOR3 position;
	D3DXQUATERNION rotation;

	D3DXVECTOR3 initialPosition;
    D3DXVECTOR3 direction;
    float amplitude;
    float frequency;

public:
	OscilatingPositionROA( const D3DXVECTOR3 &pInitialPosition,
		const D3DXQUATERNION &pInitialRotation, const D3DXVECTOR3 &pDirection, 
		float pAmplitude, float pFrequency );
    
    ~OscilatingPositionROA() {
		OutputDebugString("Destroying Oscilating Position ROA controller...\n");
	};

	const std::string & getControllerType () const { return controllerType; };

    void update(AnimatedObject &object, float deltaTime=(1.0f/60.0f) );    
    bool isFinished() const { return false; };
    const D3DXVECTOR3 &getPosition() const { return position; };
    const D3DXQUATERNION &getRotation() const { return rotation; };

};

#endif // OSCILATING_POSITION_ROA_H
