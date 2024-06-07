#include "ArachnidBehaviour.h"
#include "AIControllers/MouthBehaviour/MouthBehaviour.h"
#include "AIControllers/AssBehaviour/AssBehaviour.h"
#include "Managers/IuranusManagers/IuranusEnemyManager.h"
#include "Managers/IuranusManagers/IuranusPlanetElementsManager.h"
#include "Managers/SoundManager.h"
#include "Object/CollisionableObject/AnimatedObject/Player/Player.h"
#include "Global/GlobalFunctions.h"
#include "Managers/PlanetManager.h"
#include "Object/Planet/Planet.h"
#include "AnimationControllers/EnemyAnimation/StatesDefines.h"
#include "AnimationControllers/EnemyAnimation/EnemyAnimation.h"
#include "Managers/ParticlesManager.h"

std::string ArachnidBehaviour::controllerType = "arachnid_behaviour";
bool ArachnidBehaviour::playedSoundFoolishedFromExpel(false);

ArachnidBehaviour::ArachnidBehaviour(const D3DXVECTOR3 &localSlotPosition, 
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
									 float pStopFollowingPlayerDistance) 
 : IAIController()
 , slot(localSlotPosition)
 , firstTime(true)
 , mayChangeDirection(false)
 , aiState(EMERGE)
 , mouthIdleDistance(pMouthIdleDistance)
 , mouthSlotableDistance(pMouthSlotableDistance)
 , assIdleDistance(pAssIdleDistance)
 , assSlotableDistance(pAssSlotableDistance)
 , attackDistance(pAttackDistance)
 , attackDirection(pAttackDirection)
 , groupmentDistance(pGroupmentDistance)
 , foolishedTime(pFoolishedTime)
 , slotOccupiedBy(NONE)
 , walkSpeed(pWalkSpeed)
 , hitSpeed(pHitSpeed)
 , prepareAttackTime(pPrepareAttackTime)
 , attackSpeed(pAttackSpeed)
 , runawaySpeed(pRunawaySpeed)
 , visionDistance(pVisionDistance)
 , visionAngle(pVisionAngle)
 , visionRadius(pVisionRadius)
 , maxPlayerAttacants(pMaxPlayerAttacants)
 , stopFollowingPlayerDistance(pStopFollowingPlayerDistance)
 , runawayWayPoint(0.0f, 1.0f, 0.0f)
 , frontHit(0.0f, 1.0f, 0.0f)
 , targetAttackPosition(0.0f, 0.0f, 0.0f)
 , foolishedFromExpel(false)
 , attackSampleChannel(NULL)
{
}


void ArachnidBehaviour::update(AnimatedObject &object, float deltaTime)
{
	if( mayICheckForNearity() )
	{
		bool playerSeen = checkPlayerSeen(object);
		if(!playerSeen)
		{
			bool nearFoolished = checkNearFoolished(object);
		}
	}

	switch (aiState) 
	{
	case EMERGE:	// Init emergence animation or continue if planet already was created
		if( ( object.getCurrentCycle() == ANIM_IDLE ) 
			&& object.getCurrentAction().empty() ) 
		{
			aiState = GONE_OUT; // RANDOM_WALK;
		
			//if( Global::samplesEnabled )
			//{
			//SoundManager *soundManager = SoundManager::getSoundManager();
			//soundManager->playSample("enemy_just_born");
			//}
		}
		else if( object.getCurrentCycle() == ANIM_VICTORY )
		{
			// TODO FIXED Cuidado, los que acaban de nacer no tienen whereToFollow,
			// por lo tanto en EnemyAnimation hara un follow a (0,0,0) y eso peta

			// ir a random
			//object.setRelationToState( CYCLE, ANIM_WALK, false, true );
			
			aiState = RANDOM_WALK;
			// Hacer otra vez el update para que haga el RANDOM_WALK y asi
			// le de al object un whereToFollow
			update( object, deltaTime );
		}
		break;
	case GONE_OUT:	// calculate random next action and goto next state
		{
			object.setRelationToState( CYCLE, ANIM_IDLE, false);

			float nextAction = Global::obtainRandom(0.0f, 5.0f);
			if( (nextAction >= 0.0f) && (nextAction <= 2.5f) )		{ aiState = GOING_MOUTH;}
			else if( (nextAction > 2.5f) && (nextAction <= 4.5f) )	{ aiState = IDLE_OUT;	}
			else													{ aiState = GOING_ASS;	}
		}
		break;
	case IDLE_OUT:	
		// timeout to make decisions be more "naturals"
		if(firstTime)
		{
			object.setRelationToState( CYCLE, ANIM_IDLE, false);
			float randTimeout = Global::obtainRandom(0.0f, 0.2f);
			timeout.init(randTimeout);
			firstTime = false;
		}
		else
		{
			if(timeout.isFinished())
			{
				float action = 0.0f;
				if(slot.isFree())	action = Global::obtainRandom(0.0f, 3.0f);
				else				action = Global::obtainRandom(0.0f, 2.0f);

				if(action >= 0.0f && action <= 1.0f)		aiState = RANDOM_WALK;
				else if(action > 1.0f && action <= 2.0f)	aiState = IDLE_OUT;
				else										aiState = FIND_PARTNER;

				firstTime = true;
			}
			else
				timeout.update(deltaTime);
		}
		break;
	case RANDOM_WALK:
		releaseAnyLockedArachnidSlot();

		if(firstTime) 
		{
			//object.setRelationToState( CYCLE, ANIM_WALK, false, false );
			initRandomWalkOptions(object, 5.0f);
			firstTime = false;
		}
		else
		{
			if(finishedRandomWalk(deltaTime))
			{
				aiState = IDLE_OUT;
				firstTime = true;
			}
			else
			{
				if( mayIChangeDirection(object) )
				{
					firstTime = true;
					aiState = GO_AWAY_FROM_MOUTH;
				}
				else
					updateRandomWalkTime(deltaTime, object);
			}
		}
		break;
	case FIND_PARTNER:
		{
			// if I have a free slot, I can go to group with others
			// otherwise, go back to IDLE_OUT
			bool myFreeSlot = slot.isFree();
			if(myFreeSlot)
			{
				IntelligentObject *partner = IuranusEnemyManager::getIuranusEnemyManager()->getGroupablePartner((IntelligentObject &)object);
				if( partner != NULL )
				{
					// lock my partner slot & change aiState
					ArachnidBehaviour *partnerAI = (ArachnidBehaviour *) partner->getAIController();
					partnerAI->slot.setToWho(&object);
					partnerAI->slot.setTargetSlotNumber(1);
					partnerAI->setCurrentAIState(GO_TO_PARTNER);
					partnerAI->setSlotOccupiedBy(ARACHNID);

					// lock my slot & change state
					slot.setToWho(partner);
					slot.setTargetSlotNumber(1);
					slotOccupiedBy = ARACHNID;
					aiState = GO_TO_PARTNER;

				}
				else
					aiState = IDLE_OUT;
			}
			else
				aiState = IDLE_OUT;
		}
		break;
	case GO_TO_PARTNER:
		{
			// update follow's until they reach each other
			// recalculate distance to partner slot position
			IntelligentObject *partner = (IntelligentObject *) slot.getToWho();
			if(partner != NULL)
			{
				ArachnidBehaviour *partnerAI = (ArachnidBehaviour *) partner->getAIController();
				assert( partnerAI );

				// TODO No se pueden sumar posiciones locales a posiciones globales
				const D3DXVECTOR3 &partnerSlotPos = partnerAI->slot.getMyLocalSlotPosition();
				D3DXVECTOR3 slotPos;
				D3DXVec3TransformCoord( &slotPos, &partnerSlotPos, &(partner->getTransformMatrix( )) );

				float distanceToPartner = Global::distanceSqrt(object.getPosition(), slotPos);

				if(distanceToPartner <= groupmentDistance) 
				{
					aiState = STAY_TOGETHER;
					partnerAI->setCurrentAIState(STAY_TOGETHER);
				}
				else 
				{
					// TODO FIXED Las posiciones no se normalizan
					//Global::normalizeVector3(slotPos, slotPos);
					assert( slotPos != object.getPosition() );
					fillAnimationFollowFieldsAndWalk(object, slotPos, walkSpeed);
				}
			}
			else	
			{
				// if the partner has been deleted before this 
				// point, we don't have who to stay together with
				releaseArachnidSlotAndPartner();
				aiState = GONE_OUT;
			}
		}
		break;
	case STAY_TOGETHER:
		{
			if(firstTime)
			{
				// define release agroupation timeout
				float randTimeout = Global::obtainRandom(2.5f, 4.0f);
				timeout.init(randTimeout); // in seconds
				firstTime = false;

				object.setRelationToState( CYCLE, ANIM_TOGETHER, false );
				
				/*
				if( Global::samplesEnabled )
				{
					SoundManager *soundManager = SoundManager::getSoundManager();
					soundManager->playSample("enemy_grouped");
				}
				*/
			}
			else
			{
				if(timeout.isFinished())
				{
					// release both arachnid slots
					releaseArachnidSlotAndPartner();
					aiState = GONE_OUT; 
					firstTime = true;
				}
				else timeout.update(deltaTime);
			}
		}
		break;
	case GOING_MOUTH:
		{
			// makes the arachnids go to the mouth, but standing at
			// MOUTH_IDLE_DISTANCE from it
			IntelligentObject *mouth = IuranusPlanetElementsManager::getIuranusPlanetElementsManager()->getElement("mouth");
			const D3DXVECTOR3 &mouthPosition = mouth->getPosition();
			float mouthRadius = IuranusPlanetElementsManager::getIuranusPlanetElementsManager()->getElementRadius("mouth");

			float distanceToMouth = Global::distanceSqrt(object.getPosition(), mouthPosition);
			if( distanceToMouth < mouthRadius + mouthIdleDistance )
			{
				aiState = IDLE_AROUND_MOUTH;
			}
			else
			{
				// TODO FIXED Las posiciones no se normalizan
				//Global::normalizeVector3(mouthPosition, mouthPosition);
				fillAnimationFollowFieldsAndWalk(object, mouthPosition, walkSpeed);
			}
		}
		break;
	case IDLE_AROUND_MOUTH:
		{
			// if there is any mouth slot free, lock it and
			// change to GO_TO_MOUTH_SLOT aiState
			// otherwise stay on IDLE_AROUND_MOUTH

			if(firstTime)
			{
				object.setRelationToState( CYCLE, ANIM_IDLE, false );

				float randTimeout = Global::obtainRandom(0.0f, 2.0f);
				timeout.init(randTimeout);
				firstTime = false;
			}
			else
			{
				if(timeout.isFinished())
				{
					//  check free mouth slot
					// if mouth has any free slot, we occupy it and go there
					// otherwise, we go to IDLE_AROUND_MOUTH aiState
					bool myFreeSlot = slot.isFree();
					if(myFreeSlot)
					{
						Slot *s = IuranusPlanetElementsManager::getIuranusPlanetElementsManager()->getFreeElementSlot("mouth");
						if(s != NULL)
						{
							IntelligentObject *mouth = IuranusPlanetElementsManager::getIuranusPlanetElementsManager()->getElement("mouth");
							unsigned int mouthSlot = IuranusPlanetElementsManager::getIuranusPlanetElementsManager()->getElementSlotIndex("mouth", s);
							if(mouthSlot != -1)
							{
								// lock my slot
								slot.setToWho(mouth);
								slot.setTargetSlotNumber(mouthSlot);
								slotOccupiedBy = MOUTH;
								// lock mouth slot
								s->setToWho(&object);
								s->setTargetSlotNumber(1);

								aiState = GO_TO_MOUTH;
							}
							else aiState = RANDOM_WALK;
						}
						else aiState = RANDOM_WALK;
					}

					firstTime = true;
				}
				else
				{
					timeout.update(deltaTime);
				}
			}
		}
		break;
	case GO_TO_MOUTH:
		{
			// go to locked slot position
			// when arrive there, change aiState to STAY_AT_MOUTH
			unsigned int mouthSlotIdx = slot.getTargetSlotNumber();
			Slot *mouthSlot = IuranusPlanetElementsManager::getIuranusPlanetElementsManager()->getElementSlotByIndex("mouth", mouthSlotIdx);
			if(mouthSlot == NULL)
			{
				releaseMouthSlotAndPartner();
				aiState = RANDOM_WALK;
			}
			else
			{
				float mouthRadius = IuranusPlanetElementsManager::getIuranusPlanetElementsManager()->getElementRadius("mouth");
				IntelligentObject *mouth = IuranusPlanetElementsManager::getIuranusPlanetElementsManager()->getElement("mouth");
				const D3DXVECTOR3 &mouthPos = mouth->getPosition();

				// TODO No se pueden sumar posiciones locales a posiciones globales
				const D3DXVECTOR3 &mouthSlotLocalPos = mouthSlot->getMyLocalSlotPosition();
				D3DXVECTOR3 worldSlotPos;
				D3DXVec3TransformCoord( &worldSlotPos, &mouthSlotLocalPos, &(mouth->getTransformMatrix( )) );

				const D3DXVECTOR3 &objectPos = object.getPosition();
				float distanceToSlot = Global::distanceSqrt(objectPos, worldSlotPos);
				if( distanceToSlot > mouthSlotableDistance + mouthRadius ) 
				{
					// TODO FIXED Las posiciones no se normalizan
					//Global::normalizeVector3(worldSlotPos, worldSlotPos);
					fillAnimationFollowFieldsAndWalk(object, worldSlotPos, walkSpeed);
				}
				else
				{
					aiState = STAY_AT_MOUTH;
				}				
			}
		}
		break;
	case STAY_AT_MOUTH:
		{
			// init timeout to stay occupying the mouth slot
			if(firstTime)
			{
				object.setRelationToState( CYCLE, ANIM_IDLE, false );

				float randTimeout = Global::obtainRandom(0.0f, 4.0f);
				timeout.init(randTimeout);
				firstTime = false;
			}
			else
			{
				if(timeout.isFinished())
				{
					float action = Global::obtainRandom(0.0f,2.0f);
					if(action >= 0 && action < 1) aiState = RELEASE_MOUTH_SLOT;
					else aiState = STAY_AT_MOUTH;
					firstTime = true;
				}
				else timeout.update(deltaTime);
			}
		}
		break;
	case RELEASE_MOUTH_SLOT:
		{
			// release both arachnid and mouth slots
			releaseMouthSlotAndPartner();
			aiState = GO_AWAY_FROM_MOUTH;
		}
		break;
	case GO_AWAY_FROM_MOUTH:
		{
			if(firstTime)
			{
				timeout.init(4.0f);
				firstTime = false;
				const D3DXVECTOR3 &assPos = IuranusPlanetElementsManager::getIuranusPlanetElementsManager()->getElement("ass")->getPosition();
				// TODO FIXED Las posiciones no se normalizan
				//Global::normalizeVector3(assPos, assPos);
				fillAnimationFollowFieldsAndWalk(object, assPos, 0.8f);
			}
			else
			{
				if(timeout.isFinished())
				{
					aiState = RANDOM_WALK;
					firstTime = true;
				}
				else
					timeout.update(deltaTime);
			}
		}
		break;
	case GOING_ASS:
		{
			// makes the arachnids go to the mouth, but standing at
			// ASS_IDLE_DISTANCE from it
			IntelligentObject *ass = IuranusPlanetElementsManager::getIuranusPlanetElementsManager()->getElement("ass");
			const D3DXVECTOR3 &assPosition = ass->getPosition();
			float assRadius = IuranusPlanetElementsManager::getIuranusPlanetElementsManager()->getElementRadius("ass");

			float distanceToAss = Global::distanceSqrt(object.getPosition(), assPosition);
			if( distanceToAss < assRadius + assIdleDistance )
			{
				aiState = IDLE_AROUND_ASS;
			}
			else
			{
				// TODO FIXED Las posiciones no se normalizan y no son vectores
				//D3DXVECTOR3 whereToGo = assPosition - object.getPosition();
				//Global::normalizeVector3(whereToGo, whereToGo);
				fillAnimationFollowFieldsAndWalk(object, assPosition, walkSpeed);
			}
		}
		break;
	case IDLE_AROUND_ASS:
		{
			// if there is any mouth slot free, lock it and
			// change to GO_TO_ASS_SLOT aiState
			// otherwise stay on IDLE_AROUND_ASS

			if(firstTime)
			{
				object.setRelationToState( CYCLE, ANIM_IDLE, false );

				float randTimeout = Global::obtainRandom(0.0f, 2.2f);
				timeout.init(randTimeout);
				firstTime = false;
			}
			else
			{
				if(timeout.isFinished())
				{
					//  check free mouth slot
					// if mouth has any free slot, we occupy it and go there
					// otherwise, we go to IDLE_AROUND_ASS aiState
					bool myFreeSlot = slot.isFree();
					if(myFreeSlot)
					{
						Slot *s = IuranusPlanetElementsManager::getIuranusPlanetElementsManager()->getFreeElementSlot("ass");
						if(s != NULL)
						{
							IntelligentObject *ass = IuranusPlanetElementsManager::getIuranusPlanetElementsManager()->getElement("ass");
							unsigned int assSlot = IuranusPlanetElementsManager::getIuranusPlanetElementsManager()->getElementSlotIndex("ass", s);
							if(assSlot != -1)
							{
								// lock my slot
								slot.setToWho(ass);
								slot.setTargetSlotNumber(assSlot);

								s->setToWho(&object);
								s->setTargetSlotNumber(1);
								slotOccupiedBy = ASS;

								aiState = GO_TO_ASS;
							} 
							else aiState = RANDOM_WALK;
						}
						else aiState = RANDOM_WALK;
					}

					firstTime = true;
				}
				else
					timeout.update(deltaTime);
			}
		}
		break;
	case GO_TO_ASS:
		{
			// go to locked slot position
			// when arrive there, change aiState to STAY_AT_ASS
			unsigned int assSlotIdx = slot.getTargetSlotNumber();
			Slot *assSlot = IuranusPlanetElementsManager::getIuranusPlanetElementsManager()->getElementSlotByIndex("ass", assSlotIdx);
			if(assSlot == NULL)
			{
				releaseAssSlotAndPartner();
				aiState = RANDOM_WALK;
			}
			else
			{
				float assRadius = IuranusPlanetElementsManager::getIuranusPlanetElementsManager()->getElementRadius("ass");
				IntelligentObject *ass = IuranusPlanetElementsManager::getIuranusPlanetElementsManager()->getElement("ass");
				const D3DXVECTOR3 &assPos = ass->getPosition();

				// TODO No se pueden sumar posiciones locales a posiciones globales
				const D3DXVECTOR3 &assSlotLocalPos = assSlot->getMyLocalSlotPosition();
				D3DXVECTOR3 worldAssSlotPos;
				D3DXVec3TransformCoord( &worldAssSlotPos, &assSlotLocalPos, &(ass->getTransformMatrix( )) );

				const D3DXVECTOR3 &objectPos = object.getPosition();
				float distanceToSlot = Global::distanceSqrt(objectPos, worldAssSlotPos);
				if( distanceToSlot > assSlotableDistance + assRadius )
				{
					fillAnimationFollowFieldsAndWalk(object, worldAssSlotPos, walkSpeed);
				}
				else
					aiState = STAY_AT_ASS;
			}
		}
		break;
	case STAY_AT_ASS:
		{
			// init timeout to stay occupying the ass slot
			if(firstTime)
			{
				object.setRelationToState( CYCLE, ANIM_IDLE, false );

				float randTimeout = Global::obtainRandom(0.0f, 4.0f);
				timeout.init(randTimeout);
				firstTime = false;
			}
			else
			{
				if(timeout.isFinished())
				{
					aiState = RELEASE_ASS_SLOT;
					firstTime = true;
				}
				else timeout.update(deltaTime);
			}
		}
		break;
	case RELEASE_ASS_SLOT:
		{
			// release both arachnid and ass slots
			releaseAssSlotAndPartner();
			aiState = RANDOM_WALK;
		}
		break;
	case CHECK_PLAYER_SLOT:
		{
			// release any previous locked slot because
			// this aiState is asynchronously assigned
			releaseAnyLockedArachnidSlot();

			Player *player = Player::getPlayer();
			Slot *s = player->getFreeSlot(maxPlayerAttacants); 
			// TODO Se hacen 2 recorridos de los slots del player, una en getFreeSlot
			// y otra en getSlotIndex, en getFreeSlot se podría devolver ya el index del slot

			if(s != NULL)
			{
				unsigned int playerSlot = player->getSlotIndex(s);
				if(playerSlot != -1)
				{
					slot.setToWho(player);
					slot.setTargetSlotNumber(playerSlot);
					slotOccupiedBy = PLAYER;

					s->setToWho(&object);
					s->setTargetSlotNumber(1);

					aiState = GO_TO_PLAYER;
				} else
					aiState = IDLE_AROUND_PLAYER;
			}
			else 
				aiState = IDLE_AROUND_PLAYER;
		}
		break;
	case IDLE_AROUND_PLAYER:
		{
			if(firstTime) 
			{
				disableArachnidColor(object);
				enableArachnidColor(object, 0xFFFF0000);

				initRandomWalkOptions(object, 2.5f);
				firstTime = false;
			}
			else
			{
				if(finishedRandomWalk(deltaTime))
				{
					aiState = RANDOM_WALK;
					firstTime = true;
				}
				else
				{
					if( mayIChangeDirection(object) )
					{
						firstTime = true;
						aiState = GO_AWAY_FROM_MOUTH;
					}
					else
						updateRandomWalkTime(deltaTime, object);
				}
			}
		}
		break;
	case GO_TO_PLAYER:
		{
			if( slot.getToWho() != NULL)
			{
				bool playerNearerToMouthThanEnemy = checkIfPlayerIsNearerToMouthThanEnemy(object);

				if( !playerNearerToMouthThanEnemy ) // enemy has to take round walk to get player's slot
					aiState = ROUND_PLAYER_BEFORE_ATTACK;
				else								// enemy can directly go to player's slot
				{
					unsigned int playerSlotIdx = slot.getTargetSlotNumber();
					D3DXVECTOR3 playerSlotPos(0,0,0);
					calculatePlayerSlotPosition(playerSlotPos, playerSlotIdx);
					const D3DXVECTOR3 &enemyPos = object.getPosition();

					float distToSlot = Global::distanceSqrt( enemyPos, playerSlotPos );
					if( distToSlot > stopFollowingPlayerDistance )
						aiState = RANDOM_WALK;
					else if( distToSlot < attackDistance )
						aiState = ATTACK_IDLE;
					else
						fillAnimationFollowFieldsAndWalk(object, playerSlotPos, attackSpeed);
				}
			}
			else
			{
				aiState = RANDOM_WALK;
			}
		}
		break;
	case ROUND_PLAYER_BEFORE_ATTACK:
		{
			D3DXVECTOR3 wayPoint(0,0,0);
			calculateRoundWayPoint(wayPoint, object);
			fillAnimationFollowFieldsAndWalk(object, wayPoint, attackSpeed);
			aiState = GO_TO_PLAYER;
		} 
		break;
	case ATTACK_IDLE:
		{
			if(firstTime)
			{
				if( Global::samplesEnabled )
				{
					SoundManager *soundManager = SoundManager::getSoundManager();

					int randomAttackNum = floor(Global::obtainRandom( 1.0f, 4.5f ));
					randomAttackNum = Global::clamp( randomAttackNum, 1, 4 );

					char attackSound[ 128 ];
					sprintf_s(attackSound, sizeof(attackSound), "enemy_attacking_%d", randomAttackNum);
		
					attackSampleChannel = soundManager->playStopableSample(attackSound);
				}

				object.setRelationToState( CYCLE, ANIM_PREPARE_ATTACK, false, true);
				firstTime = false;

				const D3DXVECTOR3 &attacantPos = object.getPosition();	
				const D3DXVECTOR3 &playerPos = Player::getPlayer()->getPosition();
				float distanceToPlayer = Global::distanceSqrt( attacantPos, playerPos );
				distanceToPlayer *= 1.5f;

				float jumpDuration = distanceToPlayer / hitSpeed;

				EnemyAnimation *enemyAnimationController = (EnemyAnimation *) object.getAnimationController();
				enemyAnimationController->initJumpFields( jumpDuration );
				fillAnimationFollowFields( object, playerPos, hitSpeed*0.8f );

				timeout.init( prepareAttackTime );
			}
			else
			{
				if( timeout.isFinished() )
				{
					object.setRelationToState( CYCLE, ANIM_FLY_DOWNWARDS, false, true );
					firstTime = true;
					aiState = JUMP_ATTACK;
				}
				else
					timeout.update( deltaTime );
			}
		}
		break;
	case JUMP_ATTACK:
		{
			if( object.getCurrentCycle() == ANIM_IDLE )
			{
				stopAttackSample();
				firstTime = true;
				releaseAnyLockedArachnidSlot();
				aiState = RANDOM_WALK;
			}
		}
		break;
	case HIT:
		{
			releaseAnyLockedArachnidSlot();
			IuranusEnemyManager::getIuranusEnemyManager()->removeFoolished(&object);

			Player * player = Player::getPlayer();
			frontHit = player->getLocalFrontVector();

			// TODO: calculate jumpDurationTime and where to move
			//		 depending on player velocity
			float playerKickSpeedResult = calculatePlayerKickForce();				
			
			// TODO FIXED fillAnimationFollowFields necesita un punto, no una dirección
			//D3DXVECTOR3 hitWayPoint = object.getPosition() + frontHit * 3.0f;
			assert( playerKickSpeedResult != 0.0f );
			D3DXVECTOR3 hitWayPoint = object.getPosition() + frontHit * playerKickSpeedResult;
			//float jumpDistance = Global::distanceSqrt( hitWayPoint, object.getPosition() );
			//float jumpDuration = jumpDistance / hitSpeed; //(playerKickSpeedResult*2.5);
			float jumpDuration = 0.6f;

			// fill the jump parameters to animationController
			EnemyAnimation *enemyAnimationController = (EnemyAnimation *) object.getAnimationController();
			enemyAnimationController->initJumpFields( jumpDuration );

			object.setRelationToState( CYCLE, ANIM_FLY_UPWARDS, false, true );
			fillAnimationFollowFields( object, hitWayPoint, playerKickSpeedResult );

			aiState = FOOLISH;
			firstTime = true;
			//IuranusEnemyManager::getIuranusEnemyManager()->addFoolished(&object);					

			ParticlesManager *pm = ParticlesManager::getParticlesManager();
		}
		break;
	case FOOLISH:
		{
			if(firstTime)
			{
				firstTime = false;					
				releaseAnyLockedArachnidSlot();
			//	IuranusEnemyManager::getIuranusEnemyManager()->addFoolished(&object);					

				if( foolishedFromExpel && !playedSoundFoolishedFromExpel )
				{
					SoundManager *soundManager = SoundManager::getSoundManager();
					soundManager->playSample("enemy_running_away_from_mouth");
					playedSoundFoolishedFromExpel = true;
				}
				else if( !foolishedFromExpel )
				{
					if( Global::samplesEnabled )
					{
						SoundManager *soundManager = SoundManager::getSoundManager();
	
						int randNum = floor(Global::obtainRandom( 1.0f, 4.5f ));
						randNum = Global::clamp( randNum, 1, 4 );
	
						char sound[ 128 ];
						sprintf_s(sound, sizeof(sound), "enemy_foolished_%d", randNum);
			
						soundManager->playSample(sound);
					}
				}
			}
			else
			{
				// check if foolished action has finished
				if( ( object.getCurrentCycle() == ANIM_SCARED ) 
					&& object.getCurrentAction().empty() )
				{
					IuranusEnemyManager::getIuranusEnemyManager()->addFoolished(&object);					
					firstTime = true;
					aiState = MOVING_AWAY;
					//IuranusEnemyManager::getIuranusEnemyManager()->removeFoolished(&object);
				}
			}
		}
		break;
	case MOVING_AWAY:
		{
			if(firstTime)
			{
				playedSoundFoolishedFromExpel = false;

				if( Global::samplesEnabled )
				{
					SoundManager *soundManager = SoundManager::getSoundManager();

					int randNum = floor(Global::obtainRandom( 1.0f, 3.5f ));
					randNum = Global::clamp( randNum, 1, 3 );

					char sound[ 128 ];
					sprintf_s(sound, sizeof(sound), "enemy_running_away_%d", randNum);
		
					soundManager->playSample(sound);
				}
				
				IuranusEnemyManager::getIuranusEnemyManager()->removeFoolished(&object);
				getMovingAwayWayPoint(runawayWayPoint, object);
				fillAnimationFollowFieldsAndWalk(object, runawayWayPoint, runawaySpeed);
				timeout.init(1.0f); // in seconds
				firstTime = false;
			}
			else
			{
				if(timeout.isFinished())
				{
					firstTime = true;
					if( foolishedFromExpel )
						aiState = GOING_MOUTH;
					else
						aiState = RANDOM_WALK;
					foolishedFromExpel = false;
					fillAnimationFoolishedFromExpel(object, false);
				}
				else
				{
					if( mayIChangeDirection(object) )
					{
						firstTime = true;
						aiState = GO_AWAY_FROM_MOUTH;
					}
					else
						timeout.update(deltaTime);
				}
			}
		}
		break;
	case MOUTH_EXPEL:
		{
			releaseAnyLockedArachnidSlot();
			firstTime = true;

			const D3DXVECTOR3 &mouthPos = IuranusPlanetElementsManager::getIuranusPlanetElementsManager()->getElement("mouth")->getPosition();
			const D3DXVECTOR3 &enemyPos = object.getPosition();

			assert( enemyPos != mouthPos );
			D3DXVECTOR3 mouthToEnemyVec = enemyPos - mouthPos;
			Global::normalizeVector3(mouthToEnemyVec, mouthToEnemyVec);

			float jumpDuration = 0.5f;

			EnemyAnimation *enemyAnimationController = (EnemyAnimation *) object.getAnimationController();
			enemyAnimationController->initJumpFields( jumpDuration );

			object.setRelationToState( CYCLE, ANIM_FLY_UPWARDS, false, true );
			// TODO FIXED fillAnimationFollowFields necesita un punto, no una dirección
			D3DXVECTOR3 mouthExpelWayPoint = object.getPosition() + mouthToEnemyVec * 3.0f;
			fillAnimationFollowFields( object, mouthExpelWayPoint, hitSpeed );

			foolishedFromExpel = true;
			fillAnimationFoolishedFromExpel(object, true);
			aiState = FOOLISH;
		}
		break;
	case ASS_EXPEL:
		{
			releaseAnyLockedArachnidSlot();
			firstTime = true;

			const D3DXVECTOR3 &assPos = IuranusPlanetElementsManager::getIuranusPlanetElementsManager()->getElement("ass")->getPosition();
			const D3DXVECTOR3 &enemyPos = object.getPosition();

			assert( enemyPos != assPos );
			D3DXVECTOR3 assToEnemyVec = enemyPos - assPos;
			Global::normalizeVector3(assToEnemyVec, assToEnemyVec);

			float jumpDuration = 0.6f;

			EnemyAnimation *enemyAnimationController = (EnemyAnimation *) object.getAnimationController();
			enemyAnimationController->initJumpFields( jumpDuration );

			object.setRelationToState( CYCLE, ANIM_FLY_UPWARDS, false, true );
			// TODO FIXED fillAnimationFollowFields necesita un punto, no una dirección
			D3DXVECTOR3 assExpelWayPoint = object.getPosition() + assToEnemyVec * 3.0f;
			fillAnimationFollowFields( object, assExpelWayPoint, hitSpeed );

			//	timeout.init( jumpDuration );
			aiState = FOOLISH;
		}
		break;
	case PREPARE_PLAYER_EATEN:
		{
			releaseAnyLockedArachnidSlot();
			IuranusEnemyManager::getIuranusEnemyManager()->removeFoolished(&object);
			EnemyAnimation *enemyAnimationController = (EnemyAnimation *) object.getAnimationController();
			enemyAnimationController->initJumpFields( 0.6f );
			object.setRelationToState( CYCLE, ANIM_VICTORY, false, true );				
			aiState = PLAYER_EATEN;
			firstTime = true;

			/*
			if( Global::samplesEnabled )
			{
				SoundManager *soundManager = SoundManager::getSoundManager();
				soundManager->playSample("enemy_happines_jump");
			}
			*/
		}
		break;
	case PLAYER_EATEN:
		{
			// let enemies jump ad eternum
		}
		break;
	case SENTENCED_TO_DEATH:
		{
			// just for checking if enemy is waiting to be eaten
		}
		break;
	default:
		aiState = RANDOM_WALK;
		firstTime = true;
		break;
	}
}

void ArachnidBehaviour::initRandomWalkOptions(AnimatedObject &object, float totalWalkTime)
{
	//	EnemyAnimation *enemyAnimationController = (EnemyAnimation *) object.getAnimationController();
	//	enemyAnimationController->setCurrentSpeed(randSpeed);
	object.setCurrentSpeed(walkSpeed);

	randWalkOpts.elapsedWalkTime = 0.0f;
	randWalkOpts.totalWalkTime = totalWalkTime;
	getAroundWayPoint(randWalkOpts.walkTarget, object);

	fillAnimationFollowFieldsAndWalk(object, randWalkOpts.walkTarget, walkSpeed);
}

void ArachnidBehaviour::updateRandomWalkTime(float deltaTime, AnimatedObject &object)
{
	const D3DXVECTOR3 &objPosition = object.getPosition();

	randWalkOpts.elapsedWalkTime += deltaTime;

	// if we are going directly into mouth,
	// get direction that avoids that
	if( mayIChangeDirection(object) )
	{
		getMovingAwayWayPoint(randWalkOpts.walkTarget, object);
		fillAnimationFollowFieldsAndWalk(object, randWalkOpts.walkTarget, walkSpeed);
	}
	else
	{
		float distanceToTarget = Global::distanceSqrt(objPosition, randWalkOpts.walkTarget);
		if( ( distanceToTarget <= 0.5f ) && !finishedRandomWalk(deltaTime) )
		{
			initRandomWalkOptions( object, randWalkOpts.totalWalkTime - randWalkOpts.elapsedWalkTime );
		}
	}
}

bool ArachnidBehaviour::finishedRandomWalk(float deltaTime) const
{
	return (randWalkOpts.elapsedWalkTime >= randWalkOpts.totalWalkTime);
}

void ArachnidBehaviour::getMovingAwayWayPoint(D3DXVECTOR3 &out, const AnimatedObject &object)
{	
	const D3DXVECTOR3 &objectDirection = object.getLocalLeftVector();
	float random = Global::obtainRandom(0.0f,1.0f);

	D3DXVECTOR3 newDirection(0,0,0);
	if( random < 0.5f )
		newDirection = objectDirection;
	else
		newDirection = -objectDirection;

	const D3DXVECTOR3 &mouthPos = IuranusPlanetElementsManager::getIuranusPlanetElementsManager()->getElement("mouth")->getPosition();
	D3DXVECTOR3 mouthDirection = mouthPos - object.getPosition();
	Global::normalizeVector3(mouthDirection, mouthDirection);
	// check that the new direction does not brings the arachnid to the mouth
	// if so, get the other posible direction
	float alpha = acos( D3DXVec3Dot(&mouthDirection, &out) );
	if( alpha < D3DX_PI/2 ) // otherSide = true;
		newDirection = -newDirection;

	out = object.getPosition() + newDirection * 3.0f;
}

void ArachnidBehaviour::getAroundWayPoint(D3DXVECTOR3 &out, const AnimatedObject &object)
{
	// Current planet center and radius
	const Planet * planet = PlanetManager::getPlanetManager()->getCurrentPlanet();
	const D3DXVECTOR3 &center = planet->getCenter();
	float radius = planet->getRadius();

	float a = center.x;
	float b = center.y;
	float c = center.z;
	float x, y, z;

	float randomAngle = Global::obtainRandom(-D3DX_PI, D3DX_PI);
	x = Global::obtainRandom(0.0f,radius) * cos(randomAngle);
	z = Global::obtainRandom(0.0f,radius) * sin(randomAngle);

	float ecuationPart = sqrt( b*b - 4.0f*(b*b - radius*radius + pow(x-a,2.0f) + pow(z-c,2.0f)) );

	float random = Global::obtainRandom(0.0f,1.0f);
	if(random>=0.5f) 
		y = ( b + ecuationPart ) / 2.0f;
	else 
		y = ( b - ecuationPart ) / 2.0f;

	out = D3DXVECTOR3( x, y, z ); // Random point in sphere
	assert( out != object.getPosition() );
}

void ArachnidBehaviour::calculatePlayerSlotPosition(D3DXVECTOR3 &out, unsigned int playerSlotIdx)
{
	IuranusPlanetElementsManager *iuranusPlanetElementsManager = IuranusPlanetElementsManager::getIuranusPlanetElementsManager();
	const D3DXVECTOR3 &mouthPosition = iuranusPlanetElementsManager->getElement("mouth")->getPosition();
	Player *player = Player::getPlayer();
	const D3DXVECTOR3 &playerPosition = player->getPosition();
	D3DXVECTOR3 mouthToPlayerVec = playerPosition - mouthPosition;
	Global::normalizeVector3( mouthToPlayerVec, mouthToPlayerVec );

	assert( attackDistance != 0.0f );
	if( playerSlotIdx == 1 )
	{
		out = playerPosition + (mouthToPlayerVec * attackDistance);
	}
	else
	{
		const D3DXVECTOR3 &playerUp = player->getLocalUpVector();
		D3DXVECTOR3 crossResult;

		if( playerSlotIdx == 0 )
		{
			D3DXVec3Cross( &crossResult, &mouthToPlayerVec, &playerUp );
			Global::normalizeVector3( crossResult, crossResult );
		}
		else if( playerSlotIdx == 2 )
		{
			D3DXVec3Cross( &crossResult, &playerUp, &mouthToPlayerVec );
			Global::normalizeVector3( crossResult, crossResult );
		}
		else {
			assert( !"Cross Result do not have values in ArachnidBehaviour" );
			return;
		}

		D3DXVECTOR3 tmpPos = mouthToPlayerVec + crossResult; // this way the arachnids stop in 45º angles
		Global::normalizeVector3( tmpPos, tmpPos );
		out = playerPosition + (tmpPos * attackDistance);
	}
}

///// ARACHNID COLORS MANAGEMENT /////
void ArachnidBehaviour::enableArachnidColor(const AnimatedObject &object, DWORD color)
{
	const MaterialVector &objectMaterialVector = object.getMaterialVector();
	MaterialVector::const_iterator it = objectMaterialVector.begin();

	while ( it != objectMaterialVector.end() ) {
		(*it)->enableAdditionalColor(color);
		++it;
	}
}

void ArachnidBehaviour::disableArachnidColor(const AnimatedObject &object)
{
	const MaterialVector &objectMaterialVector = object.getMaterialVector();
	MaterialVector::const_iterator it = objectMaterialVector.begin();

	while ( it != objectMaterialVector.end() ) {
		(*it)->disableAdditionalColor();
		++it;
	}
}
///// END ARACHNID COLORS MANAGEMENT /////

///// SLOTS MANAGEMENT /////
void ArachnidBehaviour::releaseAnyLockedArachnidSlot()
{
	if(!slot.isFree())
	{
		switch(slotOccupiedBy)
		{
		case MOUTH:
			releaseMouthSlotAndPartner();
			slotOccupiedBy = NONE;
			break;
		case ASS:
			releaseAssSlotAndPartner();
			slotOccupiedBy = NONE;
			break;
		case ARACHNID:
			releaseArachnidSlotAndPartner();
			slotOccupiedBy = NONE;
			break;
		case PLAYER:
			releasePlayerSlotAndPartner();
			slotOccupiedBy = NONE;
			break;
		default:
			assert( !"An slot is not free, but has a not known SlotOccupiedBy" );
			return;
			break;
		}
	}

	// Automaticaly slot destroies itself
}

void ArachnidBehaviour::releasePlayerSlotAndPartner()
{
	unsigned int playerLockedSlotIdx = slot.getTargetSlotNumber();
	Slot *playerSlot = Player::getPlayer()->getSlotByIndex(playerLockedSlotIdx);
	if(playerSlot != NULL)
	{
		playerSlot->setToWho(NULL);
		playerSlot->setTargetSlotNumber(-1);
	}
	slot.setToWho(NULL);
	slot.setTargetSlotNumber(-1);
}

void ArachnidBehaviour::releaseMouthSlotAndPartner()
{
	IntelligentObject *mouth = (IntelligentObject *) slot.getToWho();
	if(mouth != NULL)
	{
		unsigned int mouthLockedSlotIdx = slot.getTargetSlotNumber();
		MouthBehaviour *mouthAI = (MouthBehaviour *) mouth->getAIController();
		Slot *mouthSlot = mouthAI->getSlotByIndex(mouthLockedSlotIdx);
		if(mouthSlot != NULL)
		{
			mouthSlot->setToWho(NULL);
			mouthSlot->setTargetSlotNumber(-1);
		}
	}

	slot.setToWho(NULL);
	slot.setTargetSlotNumber(-1);
}

void ArachnidBehaviour::releaseAssSlotAndPartner()
{
	IntelligentObject *ass = (IntelligentObject *) slot.getToWho();
	if(ass != NULL)
	{
		unsigned int assLockedSlotIdx = slot.getTargetSlotNumber();
		AssBehaviour *assAI = (AssBehaviour *) ass->getAIController();
		Slot *assSlot = assAI->getSlotByIndex(assLockedSlotIdx);
		if(assSlot != NULL)
		{
			assSlot->setToWho(NULL);
			assSlot->setTargetSlotNumber(-1);
		}
	}

	slot.setToWho(NULL);
	slot.setTargetSlotNumber(-1);
}

void ArachnidBehaviour::releaseArachnidSlotAndPartner()
{
	IntelligentObject *partner = (IntelligentObject *) slot.getToWho();
	if(partner != NULL)
	{
		ArachnidBehaviour *partnerAI = (ArachnidBehaviour *) partner->getAIController();
		partnerAI->slot.setToWho(NULL);
		partnerAI->slot.setTargetSlotNumber(-1);
		partnerAI->setCurrentAIState(RANDOM_WALK);
	}

	slot.setToWho(NULL);
	slot.setTargetSlotNumber(-1);
}
///// END SLOTS MANAGEMENT /////

float ArachnidBehaviour::calculatePlayerKickForce()
{
	float resultantHitForce = 0.0f;
	Player * player = Player::getPlayer();
	//DWORD playerState = player->getState();

	if (player->getCurrentCycle() == "jog")				// RUNNING
		resultantHitForce = hitSpeed * 0.8f;
	else if (player->getCurrentCycle() == "walk")		// WALKING
		resultantHitForce = hitSpeed * 0.6f;
	else if (player->getCurrentCycle() == "idle")		// STOPPED
		resultantHitForce = hitSpeed * 0.4f;

	return resultantHitForce;
}

bool ArachnidBehaviour::mayICheckForNearity() const
{
	if( (aiState != CHECK_PLAYER_SLOT) &&
		(aiState != GO_TO_PLAYER) &&
		(aiState != ROUND_PLAYER_BEFORE_ATTACK) &&
		(aiState != IDLE_AROUND_PLAYER) &&
		(aiState != ATTACK_IDLE) &&
		(aiState != JUMP_ATTACK) &&
		(aiState != HIT) &&
		(aiState != FOOLISH) &&
		(aiState != MOVING_AWAY) &&
		(aiState != MOUTH_EXPEL) &&
		(aiState != ASS_EXPEL) &&
		(aiState != RECOVER_STATE) &&
		(aiState != PREPARE_PLAYER_EATEN) &&
		(aiState != PLAYER_EATEN) &&
		(aiState != EMERGE) )
		return true;
	else
		return false;
}

bool ArachnidBehaviour::checkNearFoolished(AnimatedObject &object)
{
	// distance from new foolished < visionDistance
	bool nearFoolished = IuranusEnemyManager::getIuranusEnemyManager()->someNearFoolished(object, visionDistance);
	if( nearFoolished ) 
	{
		firstTime = true;
		aiState = MOVING_AWAY;
		object.setRelationToState( CYCLE, ANIM_SCARED, false, true );
		return true;
	}

	return false;
}

bool ArachnidBehaviour::checkPlayerSeen(const AnimatedObject &object)
{
	Player *player = Player::getPlayer();
	const D3DXVECTOR3 &playerPosition = player->getPosition();
	const D3DXVECTOR3 &arachnidPosition = object.getPosition();

	float playerDistance = Global::distanceSqrt(playerPosition, arachnidPosition);
	if( playerDistance < visionRadius )
	{
		firstTime = true;
		aiState = CHECK_PLAYER_SLOT;
		return true;
	}
	else if( playerDistance < visionDistance )
	{
		D3DXVECTOR3 arachnidPlayerVector = playerPosition - arachnidPosition;
		Global::normalizeVector3( arachnidPlayerVector, arachnidPlayerVector );

		const D3DXVECTOR3 &arachnidFront = object.getLocalFrontVector();
		float cosinus = D3DXVec3Dot( &arachnidFront, &arachnidPlayerVector );
		float alpha = acos( cosinus );

		// Player is in angle vision
		if( alpha < visionAngle )
		{
			firstTime = true;
			aiState = CHECK_PLAYER_SLOT;
			return true;
		}
	}

	return false;
}

bool ArachnidBehaviour::mayIChangeDirection(const AnimatedObject &object) const
{
	const D3DXVECTOR3 &arachnidPos = object.getPosition();
	IuranusPlanetElementsManager *iuranusPlanetElementsManager = IuranusPlanetElementsManager::getIuranusPlanetElementsManager();
	//const D3DXVECTOR3 &mouthPos = iuranusPlanetElementsManager->getElement("mouth")->getPosition();
	const D3DXVECTOR3 &mouthPos = iuranusPlanetElementsManager->getElement("mouth")->getPosition();
	float mouthRadius = iuranusPlanetElementsManager->getElementRadius("mouth");

	// TODO: mouthPos should be in the up part of the mouth oject
	//	mouthPos.y += 2.7473f;

	float distanceToMouth = Global::distanceSqrt( mouthPos, arachnidPos );

	if( distanceToMouth < mouthSlotableDistance + mouthRadius ) 
		return true;

	return false;
}

bool ArachnidBehaviour::checkIfPlayerIsNearerToMouthThanEnemy(const AnimatedObject &object) const
{
	Player *player = Player::getPlayer();
	IuranusPlanetElementsManager *iuranusPlanetElementsManager = IuranusPlanetElementsManager::getIuranusPlanetElementsManager();

	const D3DXVECTOR3 &enemyPos = object.getPosition();
	const D3DXVECTOR3 &playerPos = player->getPosition();
	//const D3DXVECTOR3 &mouthPos = iuranusPlanetElementsManager->getElement("mouth")->getPosition();
	const D3DXVECTOR3 &mouthPos = iuranusPlanetElementsManager->getElement("mouth")->getPosition();

	// TODO:con el pivote correcto no necesario
	// mouthPos.y += 2.7473f;

	float enemyDistanceToMouth = Global::distance( enemyPos, mouthPos );
	float playerDistanceToMouth = Global::distance( playerPos, mouthPos );

	if( enemyDistanceToMouth <= playerDistanceToMouth )
		return false;

	return true;
}

void ArachnidBehaviour::calculateRoundWayPoint(D3DXVECTOR3 &out, const AnimatedObject &object)
{
	Player *player = Player::getPlayer();
	IuranusPlanetElementsManager *iuranusPlanetElementsManager = IuranusPlanetElementsManager::getIuranusPlanetElementsManager();
	const D3DXVECTOR3 &playerPos = player->getPosition();
	const D3DXVECTOR3 &enemyPos = object.getPosition();
	const D3DXVECTOR3 &mouthPos = iuranusPlanetElementsManager->getElement("mouth")->getPosition(); 

	D3DXVECTOR3 playerMouthVec = mouthPos - playerPos;
	Global::normalizeVector3( playerMouthVec, playerMouthVec );

	D3DXVECTOR3 playerEnemyVec = enemyPos - playerPos;
	Global::normalizeVector3( playerEnemyVec, playerEnemyVec );

	float alpha = acos( D3DXVec3Dot( &playerMouthVec, &playerEnemyVec ) );

	D3DXVECTOR3 newDirection(0,0,0);

	if( alpha < D3DX_PI )
	{
		// get the enemy perpendicular vector to enemyMouthVec as wayPoint
		/*
		//const Planet * planet = PlanetManager::getPlanetManager()->getCurrentPlanet();
		//const D3DXVECTOR3 &planetCenter = planet->getCenter();
		//D3DXVECTOR3 falseUpEnemyVec = enemyPos - planetCenter;
		//Global::normalizeVector3( falseUpEnemyVec, falseUpEnemyVec );
		
		const D3DXVECTOR3 &falseUpEnemyVec = object.getLocalUpVector();

		D3DXVECTOR3 enemyMouthVec = mouthPos - enemyPos;
		Global::normalizeVector3( enemyMouthVec, enemyMouthVec );

		if( attackDirection <= 1 )
			D3DXVec3Cross(&out, &falseUpEnemyVec, &enemyMouthVec);
		else
			D3DXVec3Cross(&out, &enemyMouthVec, &falseUpEnemyVec);
		Global::normalizeVector3( out, out );
		*/

		D3DXVECTOR3 mouthToEnemyVec = enemyPos - mouthPos;
		Global::normalizeVector3( mouthToEnemyVec, mouthToEnemyVec );

		newDirection = mouthToEnemyVec * 5.0f - playerEnemyVec;
		Global::normalizeVector3( newDirection, newDirection );

		/*
		newDirection = playerEnemyVec - playerMouthVec * 5.0f;
		Global::normalizeVector3( out, out );
		*/
	}
	else
	{
		//newDirection = playerPos - mouthPos;
		newDirection = -playerEnemyVec;
	}

	//out *= 1.4f;
	out = object.getPosition() + newDirection * 3.0f;
}

void ArachnidBehaviour::collisionDetected( IntelligentObject &object )
{
	if( aiState == JUMP_ATTACK )
	{
		aiState = FOOLISH;
		firstTime = true;
		object.setRelationToState( CYCLE, ANIM_ATTACK_OK, false, true );
		Player *player = Player::getPlayer();

		player->hitRecieved(object.getLocalFrontVector());
		
		// stop the enemy attack sample when reaching the player
		stopAttackSample();
	}
}

// TODO FIXED whereToFollow es un PUNTO, NO un VECTOR (es decir, no es una direccion)	
void ArachnidBehaviour::fillAnimationFollowFieldsAndWalk(AnimatedObject &object, const D3DXVECTOR3 &whereToFollow, float maxSpeed)
{
	object.setRelationToState( CYCLE, ANIM_WALK, false );
	fillAnimationFollowFields( object, whereToFollow, maxSpeed );
}

// TODO FIXED whereToFollow es un PUNTO, NO un VECTOR (es decir, no es una direccion)	
void ArachnidBehaviour::fillAnimationFollowFields(AnimatedObject &object, const D3DXVECTOR3 &whereToFollow, float maxSpeed)
{
	EnemyAnimation *enemyAnimationController = (EnemyAnimation *) object.getAnimationController();
	assert( whereToFollow != D3DXVECTOR3(0,0,0) );
	assert( whereToFollow != object.getPosition() );
	enemyAnimationController->setWhereToFollow(whereToFollow);
	enemyAnimationController->setAnimationMaxSpeed(maxSpeed);
}

void ArachnidBehaviour::fillAnimationFoolishedFromExpel(AnimatedObject &object, bool fromExpel)
{
	EnemyAnimation *enemyAnimationController = (EnemyAnimation *) object.getAnimationController();
	enemyAnimationController->setFoolishedFromExpel(fromExpel);
}

void ArachnidBehaviour::stopAttackSample() 
{
	SoundManager::getSoundManager()->stopSample(attackSampleChannel);
	attackSampleChannel = NULL;
}