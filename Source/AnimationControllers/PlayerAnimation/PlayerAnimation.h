#ifndef PLAYER_ANIMATION_H
#define PLAYER_ANIMATION_H

#include "AnimationControllers/IAnimationController.h"

/**
* Player Animation Controller. It checks in what animation status
* the player is and updates the animations
*/

class PlayerAnimation: public IAnimationController
{
protected:
	static std::string controllerType;

    float jumpTime;
	float variation;
	float jumpDurationTime;
    float kickTime;
	float kickDurationTime;
	float runTime;
	float hitTime;
    D3DXVECTOR3 position;
	D3DXQUATERNION rotation;

	D3DXVECTOR3 initialPosition;
    D3DXVECTOR3 direction;

	D3DXVECTOR3	hitDirection;
    float amplitude;
    float frequency;

	// Speed for the player controller
	float maxSpeed;

	// Acceleration and deceleration for the player controller
	float acceleration;
	float deceleration;
	
	bool isKicking;
	bool alreadyPlayedKickSound;

	D3DXVECTOR3 jumpPrevPosition;

	bool firstMomentInJump;

	void updateGraph(AnimatedObject &object, float deltaTime=(1.0f/60.0f));


public:
    PlayerAnimation( const D3DXVECTOR3 &pInitialPosition,
		const D3DXQUATERNION &pInitialRotation, const D3DXVECTOR3 &pDirection, 
		float pAmplitude, float pFrequency );
    
    ~PlayerAnimation() {
		OutputDebugString("Destroying Player Animation controller...\n");
	};

	const std::string & getControllerType () const { return controllerType; };

	void resetJumpVariables( ) {
		jumpTime = 0.0f;
		variation = 0.0f;
	}

    void update(AnimatedObject &object, float deltaTime=(1.0f/60.0f) );    
    bool isFinished() const { return false; };
    const D3DXVECTOR3 &getPosition() const { return position; };
    const D3DXQUATERNION &getRotation() const { return rotation; };
	void hitRecieved(AnimatedObject &object, const D3DXVECTOR3 &newFront);
	bool isPlayerKicking() const;

	void collisionDetected( AnimatedObject &object, const D3DXVECTOR3 &normal, D3DXVECTOR3 &additionalMovementInOut);
	void collisionDetected(AnimatedObject& object, const D3DXVECTOR3& normal);
	void solidCollisionDetected( AnimatedObject &object, const D3DXVECTOR3 &normal );
	void fireballCollisionDetected( AnimatedObject &object );

private:
	void movePlayer (AnimatedObject &object, float deltaTime=(1.0f/60.0f));
	void movePlayerInJump (AnimatedObject &object, const D3DXVECTOR3 &frontJump, float deltaTime=(1.0f/60.0f));
	void jumpPlayer (AnimatedObject &object, float deltaTime=(1.0f/60.0f));
	void jumpAssExpulsionPlayer(AnimatedObject &object, float deltaTime=(1.0f/60.0f));
	void moveExpulsionPlayer(AnimatedObject &object, float deltaTime=(1.0f/60.0f));
	void deepInAss( AnimatedObject &object, float deltaTime=(1.0f/60.0f) );
	void kickPlayer (AnimatedObject &object, float deltaTime=(1.0f/60.0f));
	void hit(AnimatedObject &object, float speed, float deltaTime=(1.0/60.0f));
	void correctPosition( AnimatedObject &object, const D3DXVECTOR3 &newPosition );

	bool isJumping(AnimatedObject &object) const;
};

#endif // PLAYER_ANIMATION_H
