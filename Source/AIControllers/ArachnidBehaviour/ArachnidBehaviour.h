#ifndef ARACHNID_BEHAVIOUR_H
#define ARACHNID_BEHAVIOUR_H

#include "Object/CollisionableObject/AnimatedObject/IntelligentObject/IntelligentObject.h"
#include "AIControllers/IAIController.h"
#include "AIControllers/AISlots/Slot.h"
#include "Util/Timeout/Timeout.h"

// Arachnid AI states
const enum ArachnidBehaviourState 
{ 
	EMERGE, GONE_OUT,								// INIT 
	IDLE_OUT, RANDOM_WALK, FIND_PARTNER, GO_TO_PARTNER, STAY_TOGETHER,		// RANDOM WALK
	GOING_MOUTH, IDLE_AROUND_MOUTH, GO_TO_MOUTH, STAY_AT_MOUTH, 
	GO_AWAY_FROM_MOUTH, RELEASE_MOUTH_SLOT,									// MOUTH
	GOING_ASS, IDLE_AROUND_ASS, GO_TO_ASS, STAY_AT_ASS, RELEASE_ASS_SLOT,	// ASS
	SEPARATE_FROM_FOOLISH_PARTNER,											// RUN AWAY FROM PARTNER FOOLISHMEN
	CHECK_PLAYER_SLOT,														// DETECTED PLAYER
	GO_TO_PLAYER, ROUND_PLAYER_BEFORE_ATTACK, IDLE_AROUND_PLAYER,			// PREPEARING ATTACK
	ATTACK_IDLE, JUMP_ATTACK,												// ATTACK
	HIT, /*HIT2, */FOOLISH, MOVING_AWAY,										// HIT
	MOUTH_EXPEL, ASS_EXPEL, PREPARE_PLAYER_EATEN, PLAYER_EATEN, RECOVER_STATE,
	SENTENCED_TO_DEATH
};	

typedef struct RandomWalkOptionsStruct {
	float elapsedWalkTime;
	float totalWalkTime;
	D3DXVECTOR3 walkTarget;

	RandomWalkOptionsStruct() 
	: elapsedWalkTime(0.0f)
	, totalWalkTime(0.0f)
	, walkTarget(0.0f, 1.0f, 0.0f) {};
} RandomWalkOptions;

const enum SlotOccupiedBy { MOUTH, ASS, ARACHNID, PLAYER, NONE };

/**
* Artificial intelligent controller that implement
* the behaviour of the arachnid enemies of iuranus planet
*/

class ArachnidBehaviour : public IAIController
{
protected:
	static std::string controllerType;
	static bool playedSoundFoolishedFromExpel;

	Slot slot;
	ArachnidBehaviourState aiState;
	RandomWalkOptions randWalkOpts;
	Timeout timeout;

	// parameters read from xml //
	float mouthIdleDistance;
	float mouthSlotableDistance;
	float assIdleDistance;
	float assSlotableDistance;
	float attackDistance;
	float attackDirection;
	float groupmentDistance;
	float foolishedTime;
	bool foolishedFromExpel;

	float walkSpeed;
	float prepareAttackTime;
	float attackSpeed;
	float hitSpeed;
	float runawaySpeed;
	float visionDistance;
	float visionAngle;
	float visionRadius;
	unsigned int maxPlayerAttacants;
	float stopFollowingPlayerDistance;
	// end parameters read from xml //

	bool firstTime;
	bool mayChangeDirection;

	SlotOccupiedBy slotOccupiedBy;

	// Current speed
	float currentSpeed;
	
	D3DXVECTOR3 runawayWayPoint;
	D3DXVECTOR3 frontHit;

	// hit parameters //

	D3DXVECTOR3 targetAttackPosition;

	DWORD attackSampleChannel;

public:
	ArachnidBehaviour(const D3DXVECTOR3 &localSlotPosition, 
					 float pMouthIdleDistance,
					 float pMouthSlotableDistance,
					 float pAssIdleDistance,
					 float pAssSlotableDistance,
					 float pAttackDistance,
					 float pAttackDirection,
					 float pGroupmentDistance,
					 float pFoolishedTime,
					 float pWalkSpeed,
					 float pHitSpeed,
					 float pPrepareAttackTime,
					 float pAttackSpeed,
					 float pRunawaySpeed,
					 float pVisionDistance,
					 float pVisionAngle,
					 float pVisionRadius,
					 unsigned int pMaxPlayerAttacants,
					 float pStopFollowingPlayerDistance);

	~ArachnidBehaviour() {
		OutputDebugString("Destroying ArachnidBehaviour controller...\n");

		releaseAnyLockedArachnidSlot();
	};

	const std::string & getControllerType () const { return controllerType; };

	void update(AnimatedObject &object, float deltaTime=(1.0f/60.0f));
	int getCurrentAIState() const { return aiState; };
	void setCurrentAIState(int pAiState) { aiState = (ArachnidBehaviourState)pAiState; };
	
	bool freeSlot() const { return slot.isFree(); };

	void setSlotOccupiedBy(SlotOccupiedBy by) { slotOccupiedBy = by; };
	void releaseAnyLockedArachnidSlot();
	
	float getVisionDistance() const { return visionDistance; };
	float getVisionRadius() const { return visionRadius; };
	float getVisionAngle() const { return visionAngle; };
	bool mayICheckForNearity() const;

	void collisionDetected( IntelligentObject &object );

private:
	
	void initRandomWalkOptions(AnimatedObject &object, float totalWalkTime);
	void updateRandomWalkTime(float deltaTime, AnimatedObject &object);
	bool finishedRandomWalk(float deltaTime) const;

	void getAroundWayPoint(D3DXVECTOR3 &out, const AnimatedObject &object);
	void getMovingAwayWayPoint(D3DXVECTOR3 &out, const AnimatedObject &object);
	
	void releaseMouthSlotAndPartner();
	void releaseAssSlotAndPartner();
	void releaseArachnidSlotAndPartner();
	void releasePlayerSlotAndPartner();

	float calculatePlayerKickForce();
	bool checkNearFoolished(AnimatedObject &object);
	bool checkPlayerSeen(const AnimatedObject &object);
	bool mayIChangeDirection(const AnimatedObject &object) const;
	bool checkIfPlayerIsNearerToMouthThanEnemy(const AnimatedObject &object) const;
	void calculateRoundWayPoint(D3DXVECTOR3 &out, const AnimatedObject &object);
	void calculatePlayerSlotPosition(D3DXVECTOR3 &out, const unsigned int playerSlotIdx);

	// TODO FIXED whereToFollow es un PUNTO, NO un VECTOR (es decir, no es una direccion)	
	void fillAnimationFollowFields(AnimatedObject &object, const D3DXVECTOR3 &whereToFollow, float maxSpeed);
	void fillAnimationFollowFieldsAndWalk(AnimatedObject &object, const D3DXVECTOR3 &whereToFollow, float maxSpeed);
	void fillAnimationFoolishedFromExpel(AnimatedObject &object, bool fromExpel);

	void enableArachnidColor(const AnimatedObject &object, DWORD color);
	void disableArachnidColor(const AnimatedObject &object);

	void stopAttackSample();
};
#endif //ARACHNID_BEHAVIOUR_H
