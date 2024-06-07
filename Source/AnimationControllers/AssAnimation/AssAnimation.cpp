#include "AssAnimation.h"
#include "AssDefines.h"

std::string AssAnimation::controllerType = "ass_animation";

AssAnimation::AssAnimation()
: IAnimationController()
, position(0.0f, 0.0f, 0.0f)
, rotation(0.0f, 0.0f, 0.0f, 1.0f)
{
}

void AssAnimation::update(AnimatedObject &object, float deltaTime ) 
{
	updateGraph(object, deltaTime);

	const std::string &currentAction = object.getCurrentAction();

	if( ( currentAction == ASS_LITTLE_FART ) )
	{
		object.setSpeedMultiplier(5.0f);
	} 
}

void AssAnimation::updateGraph(AnimatedObject &object, float deltaTime )
{
	AnimationGraph * animationGraph = object.getAnimationGraph();

	CalModel *calModel = object.getCalModel();
	if(calModel) {

		const AnimationRelation *animationRelation = object.getAnimationRelation();

		if (animationRelation != NULL)
		{	
			bool actionCanceled = false;

			const std::string &sourceCycle = animationRelation->getSourceCycle();
			const std::string &sourceAction = animationRelation->getSourceAction();

			const std::string &targetCycle = animationRelation->getTargetCycle();
			const std::string &targetAction = animationRelation->getTargetAction();

			if (animationRelation->getCancelPreviousAction())
			{
				if (! sourceAction.empty())
				{
					int id = object.getCalIdAnim(sourceAction);
					calModel->getMixer()->removeAction(id);
					actionCanceled = true;
				}
			}

			if (animationRelation->getCancelPreviousCycle())
			{
				if (! sourceCycle.empty())
				{
					int id = object.getCalIdAnim(sourceCycle);
					calModel->getMixer()->clearCycle(id, animationRelation->getClearTime());
				}
			}

			if (!targetCycle.empty())
			{
				int id = object.getCalIdAnim(targetCycle);
				calModel->getMixer()->blendCycle(id, 1, animationRelation->getBlendTime());
			}

			if (!targetAction.empty())
			{
				if (object.getCurrentAction().empty() || actionCanceled)
				{
					int id = object.getCalIdAnim(targetAction);
					calModel->getMixer()->executeAction(id, animationRelation->getBlendTime(), 0.2f, 1, false);
				}
			}

			object.moveAnimationState();
		}

		if (animationGraph->isExecutingAction() && object.getCalModel()->getMixer()->getAnimationActionList().empty()) {
			const std::string &cycleName = animationGraph->getNextCycle();
			const std::string &actionName = animationGraph->getNextAction();

			assert( !cycleName.empty() || !actionName.empty() );

			animationGraph->setRelationToState(cycleName, actionName, true);
		}


		float animTimeFactor;

		if (object.getCurrentSpeed()!= 0)
			animTimeFactor = object.getSpeedMultiplier()*object.getCurrentSpeed()/(1.0f+object.getCurrentSpeed());
		else
			animTimeFactor = object.getSpeedMultiplier();

		calModel->getMixer()->setTimeFactor(animTimeFactor);
		calModel->getMixer()->setActionsTimeFactor(animTimeFactor);
		calModel->update(deltaTime, object.getDrawn());

		calModel->getMixer()->setActionsTimeFactor(1.0f);
		object.setSpeedMultiplier(1.0f);
	}
}
