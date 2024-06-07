#ifndef ENEMY_ANIMATION_H
#define ENEMY_ANIMATION_H

#include "AnimationControllers/IAnimationController.h"

/**
* Enemy Animation Controller. It checks in what animation status
* the enemy is and updates the animations
*/

class EnemyAnimation: public IAnimationController
{
protected:
	static std::string controllerType;

    D3DXVECTOR3 positionNotUsed;
	D3DXQUATERNION rotation;

//	float currentSpeed;
	float acceleration;
	float deceleration;
	float rotationSpeed;

	float jumpTime;
	float jumpDurationTime;
	float frequency;
	float amplitude;
	float variation;
	float jumpDistance;
	D3DXVECTOR3 planetCenter;
//	float jumpDistance;
//	D3DXVECTOR3 jumpPrevPosition;
	bool foolishedFromExpel;


	D3DXVECTOR3 whereToFollow; // TODO FIXED whereToFollow es un PUNTO, NO un VECTOR (es decir, no es una direccion)
	float maxSpeed;

public:
	EnemyAnimation(float pRotationSpeed, float pJumpHeight);
    
	~EnemyAnimation() {
		OutputDebugString("Destroying Enemy Animation controller...\n");
	};

	const std::string & getControllerType () const { return controllerType; };

    void update(AnimatedObject &object, float deltaTime=(1.0f/60.0f) );    
	bool isFinished() const { return false; };
	const D3DXVECTOR3 &getPosition() const { assert( !"This must not happend" ); return positionNotUsed; };
    const D3DXQUATERNION &getRotation() const { assert( !"This must not happend" ); return rotation; };

	// TODO FIXED whereToFollow es un PUNTO, NO un VECTOR (es decir, no es una direccion)
	void setWhereToFollow(const D3DXVECTOR3 &pWhereToFollow)
	{
		whereToFollow = pWhereToFollow;
	};

	void setAnimationMaxSpeed(float pMaxSpeed)
	{
		maxSpeed = pMaxSpeed;
	};

	//void setCurrentSpeed(float pCurrentSpeed)
	//{
	//	currentSpeed = pCurrentSpeed;
	//}

	void initJumpFields(float pJumpDurationTime)
	{
		jumpTime = 0.0f;
		jumpDurationTime = pJumpDurationTime;
	};

	void setFoolishedFromExpel(bool pFromSpeel) {foolishedFromExpel = pFromSpeel;};

	void fallDown( AnimatedObject &object, float deltaTime );

	void collisionDetected( AnimatedObject &object, const D3DXVECTOR3 &normal, D3DXVECTOR3 &additionalMovementInOut);
	void collisionDetected(AnimatedObject& object, const D3DXVECTOR3& normal);

private:
	void follow( AnimatedObject &object, const D3DXVECTOR3 &aim, float speed, float deltaTime=(1.0f/60.0f) );
	void directObject( AnimatedObject &object, const D3DXVECTOR3 &aim, float deltaTime=(1.0f/60.0f) );
	void alignObject( AnimatedObject &object, const D3DXVECTOR3 &aim ); 
	void moveObject( AnimatedObject &object, float speed, float deltaTime=(1.0f/60.0f) );
	void correctPosition( AnimatedObject &object, const D3DXVECTOR3 &newPosition );

	void jump(AnimatedObject &object, float deltaTime, bool attack = false);
	void jumpFoolished(AnimatedObject &object, float deltaTime, bool attack = false);
	void hit(AnimatedObject &object, const D3DXVECTOR3 &aim, float speed, float deltaTime=(1.0/60.0f));

	void emerge( AnimatedObject &object, float deltaTime=(1.0f/60.0f) );

	void updateGraph(AnimatedObject &object, float deltaTime=(1.0f/60.0f));
//	void jumpAttack(AnimatedObject &object, float deltaTime);

	bool isJumping( AnimatedObject &object );
};

#endif // ENEMY_ANIMATION_H
