#include "AnimationRelation.h"
#include "AnimationState.h"

AnimationRelation::AnimationRelation(float pClearTime, float pBlendTime,
							AnimationState *pSourceAnimationState, AnimationState *pTargetAnimationState,
							bool pMustFinish, bool pCancelPreviousAction, bool pCancelPreviousCycle):
clearTime(pClearTime)
, blendTime(pBlendTime)
, sourceAnimationState(pSourceAnimationState)
, targetAnimationState(pTargetAnimationState)
, mustFinish(pMustFinish)
, cancelPreviousAction(pCancelPreviousAction)
, cancelPreviousCycle(pCancelPreviousCycle)
{
	assert( pSourceAnimationState!=NULL && pTargetAnimationState!=NULL );
}

bool AnimationRelation::isSameState(const std::string &cycleName, const std::string &actionName) const
{
	return targetAnimationState->isSameState(cycleName, actionName);
}

const std::string & AnimationRelation::getTargetAction() const
{
	return targetAnimationState->getAction();
}

const std::string & AnimationRelation::getTargetCycle() const
{
	return targetAnimationState->getCycle();
}

const std::string & AnimationRelation::getSourceAction() const
{
	return sourceAnimationState->getAction();
}
const std::string & AnimationRelation::getSourceCycle() const
{
	return sourceAnimationState->getCycle();
}