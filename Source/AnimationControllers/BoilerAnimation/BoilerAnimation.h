#ifndef BOILER_ANIMATION_H
#define BOILER_ANIMATION_H

#include "AnimationControllers/IAnimationController.h"

/**
* Boiler Animation Controller. It checks in what animation status
* the boiler is and updates the animations
*/

class BoilerAnimation: public IAnimationController
{
protected:
	static std::string controllerType;

	D3DXVECTOR3 position;
	D3DXQUATERNION rotation;

public:
	BoilerAnimation();
    
	~BoilerAnimation() {
		OutputDebugString("Destroying BoilerAnimation controller...\n");
	};

	const std::string & getControllerType () const { return controllerType; };

    void update(AnimatedObject &object, float deltaTime=(1.0f/60.0f) );

	bool isFinished() const { return false; };
	const D3DXVECTOR3 &getPosition() const { return position; };
    const D3DXQUATERNION &getRotation() const { return rotation; };
	
private:
	void updateGraph(AnimatedObject &object, float deltaTime=(1.0f/60.0f));
};

#endif // BOILER_ANIMATION_H
