#ifndef ANIMATION_STATE_H
#define ANIMATION_STATE_H

#include "AnimationRelation.h"

/**
* An animation state in animation graph.
*/

class AnimationState
{
protected:
	std::string cycle;
	std::string action;

	AnimationState *nextState; // Usefull only with actions

	AnimationRelations animationRelations;

private:
	std::string emptyString;

public:
	AnimationState(const std::string &pCycle, const std::string &pAction="", AnimationState * pNextState=NULL);

	~AnimationState() {
		OutputDebugString("Destroying AnimationState...\n");

		// Automaticaly AnimationRelations vector destroies itself
	};


	const std::string &getCycle() const;
	const std::string &getAction() const;

	const AnimationRelation * searchRelationToState(ActionType actionType, const std::string &nameState, bool aloneAnimation) const;
	AnimationRelation * searchRelationToState(ActionType actionType, const std::string &nameState, bool aloneAnimation);

	const AnimationRelation * searchRelationToState(const std::string &cycleName, const std::string &actionName) const;
	AnimationRelation * searchRelationToState(const std::string &cycleName, const std::string &actionName);

	void addRelation(AnimationRelation *animationRelation);

	const std::string &getNextCycle() const;
	const std::string &getNextAction() const;

	bool isSameState(const std::string &pCycle, const std::string &pAction) const;

	void setNextState(AnimationState * pNextState) {nextState = pNextState;};
};

class AnimationStates : public std::vector<AnimationState *>
{
public:
	// Default constructor
	AnimationStates(){};

	// Destructor
	~AnimationStates() {
		OutputDebugString("Destroying AnimationStates...\n");
		clear();
	};

	// Override clear function
	void clear();

	const AnimationState * findCycle(const std::string &cycleName) const;
	AnimationState * findCycle(const std::string &cycleName);

	const AnimationState * findState(const std::string &cycleName, const std::string &actionName) const;
	AnimationState * findState(const std::string &cycleName, const std::string &actionName);

	// TODO
	//const AnimationState * findAction(const std::string &actionName) const;
	//AnimationState * findAction(const std::string &actionName);
};

#endif // ANIMATION_STATE_H
