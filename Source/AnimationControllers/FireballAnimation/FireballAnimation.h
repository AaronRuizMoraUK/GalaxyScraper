#ifndef FIREBALL_ANIMATION_H
#define FIREBALL_ANIMATION_H

#include "AnimationControllers/IAnimationController.h"
#include "DirectX/DirectX.h"

enum FireballAnimationState
{
	A_MOVING, A_WAITING_TO_MOVE, A_WAITING_TO_RESTART
};

/**
* Enemy Animation Controller. It checks in what animation status
* the enemy is and updates the animations
*/

class FireballAnimation: public IAnimationController
{
protected:
	static std::string controllerType;

    D3DXVECTOR3 position;
	D3DXQUATERNION rotation;
	D3DXVECTOR3 initialPosition;
	float verticalSpeed;
	float horizontalSpeed;
	FireballAnimationState state;
	float currentTime;
	D3DXMATRIX transformMatrix;
	bool isFalling;
	
public:
	FireballAnimation();
    
    ~FireballAnimation() {
		OutputDebugString("Destroying FireballAnimation controller...\n");
	};

	const std::string & getControllerType () const { return controllerType; };

    void update(AnimatedObject &object, float deltaTime=(1.0f/60.0f) );    
	bool isFinished() const { return false; };
	const D3DXVECTOR3 &getPosition() const { return position; };
    const D3DXQUATERNION &getRotation() const { return rotation; }; 

	void setInitialPosition(const D3DXVECTOR3 &v_position) {initialPosition = v_position;};

	void resetPosition(AnimatedObject &object);

	int getCurrentAnimationState() const { return state; };
	void setCurrentAnimationState(int pState) {state=(FireballAnimationState)pState;} ;

	bool getIsFalling() const {return isFalling;};

	void setSpeeds(float pVerticalSpeed, float pHorizontalSpeed);

	void setTransformMatrix(const D3DXMATRIX &pTransformMatrix) {transformMatrix = pTransformMatrix;};

protected:
	void move(AnimatedObject &object, float deltaTime);
	

	
};

#endif // FIREBALL_ANIMATION_H
