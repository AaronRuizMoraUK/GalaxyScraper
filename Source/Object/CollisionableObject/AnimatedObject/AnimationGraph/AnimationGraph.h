#ifndef ANIMATION_GRAPH_H
#define ANIMATION_GRAPH_H

#include "AnimationRelation.h"
#include "AnimationState.h"

/**
* Animation Graph. This has current animation state and the 
* previous to current state relation
*/

class AnimationGraph
{
protected:
	AnimationState * currentAnimationState;
	AnimationRelation * previousToCurrentRelation;

public:
	// Default constructor
    AnimationGraph() : currentAnimationState(NULL)
		, previousToCurrentRelation(NULL) {};

	// Destructor
	~AnimationGraph() {
		OutputDebugString("Destroying Animation Graph controller...\n");

		// AnimationGraph does not destroy its states and relations, AnimationStateGraphFactory does!!.
	};

	const AnimationRelation * getPreviousToCurrentRelation() const { return previousToCurrentRelation; };

	void moveAnimationState() {
		currentAnimationState = previousToCurrentRelation->getTargetState(); 
		previousToCurrentRelation = NULL;
	};

	void setRelationToState(ActionType actionType, const std::string &name, bool hasFinished, bool aloneAnimation);

	void setRelationToState(const std::string &cycleName, const std::string &actionName, bool hasFinished);

	const std::string & getNextCycle() const;
	const std::string & getNextAction() const;

	const std::string & getCycle() const;
	const std::string & getAction() const;

	void setCurrentState(AnimationState * pCurrentState) {currentAnimationState = pCurrentState; };

	bool isExecutingAction() const {return !currentAnimationState->getAction().empty(); };
};

#endif // ANIMATION_GRAPH_H
