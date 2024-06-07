#include "AnimationState.h"

AnimationState::AnimationState(const std::string &pCycle, const std::string &pAction, AnimationState * pNextState):
cycle(pCycle), action(pAction), nextState(pNextState), emptyString( "" )
{
}

const AnimationRelation * AnimationState::searchRelationToState(ActionType actionType, const std::string &nameState, bool aloneAnimation) const
{
	std::string actionName = "";
	std::string cycleName = "";

	if (actionType == ACTION)
	{
		if (aloneAnimation)
			cycleName = "";
		else if (!cycle.empty())
			cycleName = cycle;
		actionName = nameState;
	}
	else
	{
		cycleName = nameState;
		if (aloneAnimation)
			actionName = "";
		else if (!action.empty())
			actionName = action;
	}

	return searchRelationToState(cycleName, actionName);
}

AnimationRelation * AnimationState::searchRelationToState(ActionType actionType, const std::string &nameState, bool aloneAnimation)
{
	std::string actionName = "";
	std::string cycleName = "";

	if (actionType == ACTION)
	{
		if (aloneAnimation)
			cycleName = "";
		else if (!cycle.empty())
			cycleName = cycle;
		actionName = nameState;
	}
	else
	{
		cycleName = nameState;
		if (aloneAnimation)
			actionName = "";
		else if (!action.empty())
			actionName = action;
	}

	return searchRelationToState(cycleName, actionName);
}

const AnimationRelation * AnimationState::searchRelationToState(const std::string &cycleName, const std::string &actionName) const
{
	AnimationRelations::const_iterator it = animationRelations.begin();

	while (it != animationRelations.end())
	{
		if ((*it)->isSameState(cycleName, actionName))
		{
			return (*it);
		}
		++it;
	}

	return NULL;
}

AnimationRelation * AnimationState::searchRelationToState(const std::string &cycleName, const std::string &actionName)
{
	AnimationRelations::const_iterator it = animationRelations.begin();

	while (it != animationRelations.end())
	{
		if ((*it)->isSameState(cycleName, actionName))
		{
			return (*it);
		}
		++it;
	}

	return NULL;
}

const std::string &AnimationState::getNextAction() const
{
	if(nextState)
		return nextState->getAction();
	else
		return emptyString;
}

const std::string &AnimationState::getNextCycle() const
{
	if(nextState)
		return nextState->getCycle();
	else
		return emptyString;
}

void AnimationState::addRelation(AnimationRelation *animationRelation)
{
	animationRelations.push_back(animationRelation);
}

const std::string & AnimationState::getCycle() const
{
	return cycle;
}

const std::string & AnimationState::getAction() const
{
	return action;
}

bool AnimationState::isSameState(const std::string &pCycle, const std::string &pAction) const
{
	if (pCycle == cycle && pAction == action)
		return true;

	return false;
}

// -------------------------------------------------------

// Override clear function
void AnimationStates::clear() {
	while( !empty() ) {
		AnimationState * animationState = (*this)[size()-1];
		if(animationState)
			delete animationState, animationState=NULL;
		pop_back();
	}
}

const AnimationState * AnimationStates::findCycle(const std::string &cycleName) const {
	const_iterator it = begin();

	while (it != end()) {
		bool ok = (*it)->isSameState(cycleName, "");
		if(ok)
			return *it;
		++it;
	}

	return NULL;
}

const AnimationState * AnimationStates::findState(const std::string &cycleName, const std::string &actionName) const {
	const_iterator it = begin();

	while (it != end()) {
		bool ok = (*it)->isSameState(cycleName, actionName);
		if(ok)
			return *it;
		++it;
	}

	return NULL;
}

AnimationState * AnimationStates::findCycle(const std::string &cycleName) {
	const_iterator it = begin();

	while (it != end()) {
		bool ok = (*it)->isSameState(cycleName, "");
		if(ok)
			return *it;
		++it;
	}

	return NULL;
}

AnimationState * AnimationStates::findState(const std::string &cycleName, const std::string &actionName) {
	const_iterator it = begin();

	while (it != end()) {
		bool ok = (*it)->isSameState(cycleName, actionName);
		if(ok)
			return *it;
		++it;
	}

	return NULL;
}
