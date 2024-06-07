#include "BoilerAnimation.h"

std::string BoilerAnimation::controllerType = "boiler_animation";

BoilerAnimation::BoilerAnimation()
: IAnimationController()
, position(0.0f, 0.0f, 0.0f)
, rotation(0.0f, 0.0f, 0.0f, 1.0f)
{
}

void BoilerAnimation::updateGraph(AnimatedObject &object, float deltaTime )
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

		float objCurrentSpeed = object.getCurrentSpeed();
		if (objCurrentSpeed != 0)
			animTimeFactor = object.getSpeedMultiplier()*objCurrentSpeed/(1.0f+objCurrentSpeed);
		else
			animTimeFactor = object.getSpeedMultiplier();

		calModel->getMixer()->setTimeFactor(animTimeFactor);
		calModel->getMixer()->setActionsTimeFactor(animTimeFactor);

		calModel->update(deltaTime, object.getDrawn());

		// Reset actions time factor and speed multiplier
		calModel->getMixer()->setActionsTimeFactor(1.0f);
		object.setSpeedMultiplier(object.getDefaultSpeedMultiplier());
	}
}

void BoilerAnimation::update(AnimatedObject &object, float deltaTime ) 
{
	updateGraph(object, deltaTime);

	// TODO Según IA meter una acción un otra
}
