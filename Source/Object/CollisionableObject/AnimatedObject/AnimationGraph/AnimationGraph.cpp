#include "AnimationGraph.h"
#include <string>

void AnimationGraph::setRelationToState(ActionType actionType, const std::string &name, bool hasFinished, bool aloneAnimation)
{
	AnimationRelation *ar = currentAnimationState->searchRelationToState(actionType, name, aloneAnimation);

	if (ar)
	{
		if (hasFinished || !(ar->getMustFinish()))
		{
			previousToCurrentRelation = ar;
		}
	}
}

void AnimationGraph::setRelationToState(const std::string &cycleName, const std::string &actionName, bool hasFinished)
{
	AnimationRelation *ar = currentAnimationState->searchRelationToState(cycleName, actionName);

	if (ar)
	{
		if (hasFinished || !(ar->getMustFinish()))
		{
			previousToCurrentRelation = ar;
		}
	}
}

const std::string & AnimationGraph::getNextCycle() const
{
	return currentAnimationState->getNextCycle();
}

const std::string & AnimationGraph::getNextAction() const
{
	return currentAnimationState->getNextAction();
}

const std::string & AnimationGraph::getCycle() const
{
	return currentAnimationState->getCycle();
}

const std::string & AnimationGraph::getAction() const
{
	return currentAnimationState->getAction();
}
