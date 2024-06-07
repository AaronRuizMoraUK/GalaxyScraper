#ifndef MOUTH_ANIMATION_H
#define MOUTH_ANIMATION_H

#include "AnimationControllers/IAnimationController.h"

/**
* Enemy Animation Controller. It checks in what animation status
* the enemy is and updates the animations
*/

class MouthAnimation: public IAnimationController
{
protected:
	static std::string controllerType;

    D3DXVECTOR3 position;
	D3DXQUATERNION rotation;

	float animTime;
	bool mouthClosed;

public:
	MouthAnimation();
    
    ~MouthAnimation() {
		OutputDebugString("Destroying Mouth Animation controller...\n");
	};

	const std::string & getControllerType () const { return controllerType; };

    void update(AnimatedObject &object, float deltaTime=(1.0f/60.0f) );    
	bool isFinished() const { return false; };
	const D3DXVECTOR3 &getPosition() const { return position; };
    const D3DXQUATERNION &getRotation() const { return rotation; };

private:
	void updateGraph(AnimatedObject &object, float deltaTime=(1.0f/60.0f));
   
};

#endif // MOUTH_ANIMATION_H
