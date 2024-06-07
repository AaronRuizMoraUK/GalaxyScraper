#ifndef ANIMATION_RELATION_H
#define ANIMATION_RELATION_H

#include <Windows.h>
#include <string>
#include <vector>
#include "Global/Definitions.h"

class AnimationState;

/**
* An animation relation between two animation states
*/

class AnimationRelation
{
protected:
	
	float clearTime;
	float blendTime;

	AnimationState * sourceAnimationState;
	AnimationState * targetAnimationState;

	bool mustFinish;
	bool cancelPreviousAction;
	bool cancelPreviousCycle;

public:
    AnimationRelation(float pClearTime, float pBlendTime,
		AnimationState *pSourceAnimationState, AnimationState *pTargetAnimationState,
		bool pMustFinish, bool pCancelPreviousAction, bool pCancelPreviousCycle);
	
    
    ~AnimationRelation() {
		OutputDebugString("Destroying AnimationRelation...\n");
	};
	
	float getClearTime() const { return clearTime; };
	float getBlendTime() const { return blendTime; };

	const std::string &getSourceAction() const;
	const std::string &getSourceCycle() const;

	const std::string &getTargetAction() const;
	const std::string &getTargetCycle() const;
	
	AnimationState *getTargetState() const {return targetAnimationState;};
	AnimationState *getSourceState() const {return sourceAnimationState;};

	bool getMustFinish() const{return mustFinish;};

	bool getCancelPreviousCycle() const{return cancelPreviousCycle;};
	bool getCancelPreviousAction() const{return cancelPreviousAction;};

	bool isSameState(const std::string &cycleName, const std::string &actionName) const;
};

class AnimationRelations : public std::vector<AnimationRelation *>
{
public:
	// Default constructor
	AnimationRelations(){};

	// Destructor
	~AnimationRelations() {
		OutputDebugString("Destroying AnimationRelations...\n");
		clear();
	};

	// Override clear function
	void clear() {
		while( !empty() ) {
			AnimationRelation * animationRelation = (*this)[size()-1];
			if(animationRelation)
				delete animationRelation, animationRelation=NULL;
			pop_back();
		}
	};
};

#endif // ANIMATION_RELATION_H