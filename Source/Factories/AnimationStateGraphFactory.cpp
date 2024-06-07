#include "AnimationStateGraphFactory.h"
#include "Managers/DataLocatorManager.h"

AnimationStateGraphFactory * AnimationStateGraphFactory::animationStateGraphFactory = NULL;

AnimationStateGraphFactory * AnimationStateGraphFactory::getAnimationStateGraphFactory() {
	if(animationStateGraphFactory == NULL)
		animationStateGraphFactory = new AnimationStateGraphFactory();

	return animationStateGraphFactory;
}

AnimationStateGraphFactory::AnimationStateGraphFactory() 
{
	currentAnimationStates = NULL;

	DataLocatorManager *dataLocatorManager = DataLocatorManager::getDataLocatorManager();
	const std::string & pathToXml = dataLocatorManager->getPath("xml");
	bool ok = xmlParseFile(pathToXml + "animation_state_graphs.xml");
	assert( ok );
};

AnimationStates * AnimationStateGraphFactory::createAnimationStateGraph(const std::string &filename) {
	AnimationStateGraphMap::const_iterator it = animationStateGraphMap.find(filename);

	if(it != animationStateGraphMap.end())
		return it->second;

	return NULL;
}

void AnimationStateGraphFactory::onStartElement (const std::string &elem, MKeyValue &atts) {
	if (currentAnimationStates)
	{
		if (elem == "animation_state")
		{
			std::string cycle = atts["cycle"];
			std::string action = atts["action"];

			AnimationState *animationState = new AnimationState(cycle, action);

			currentAnimationStates->push_back(animationState);
		}
		else if (elem == "animation_relation")
		{
			std::string sourceCycle = atts["source_cycle"];
			std::string sourceAction = atts["source_action"];
			std::string targetCycle = atts["target_cycle"];
			std::string targetAction = atts["target_action"];

			AnimationState * sourceState = currentAnimationStates->findState(sourceCycle, sourceAction);
			AnimationState * targetState = currentAnimationStates->findState(targetCycle, targetAction);

			assert(sourceState!=NULL && targetState!=NULL);

			float blendTime = atts.getFloat( "blend_time", 0.0f );
			float clearTime = atts.getFloat( "clear_time", 0.0f );
			int mustFinish = atts.getInt( "must_finish", 0 );
			int cancelCycle = atts.getInt( "cancel_cycle", 0 );
			int cancelAction = atts.getInt( "cancel_action", 0 );

			AnimationRelation *animRelation = new AnimationRelation(clearTime, blendTime, sourceState, targetState, mustFinish == 1, cancelAction == 1, cancelCycle == 1);

			sourceState->addRelation(animRelation);
		}
		else if (elem == "next_animation")
		{
			std::string sourceCycle = atts["cycle"];
			std::string sourceAction = atts["action"];
			std::string targetCycle = atts["next_cycle"];
			std::string targetAction = atts["next_action"];

			
			AnimationState * sourceState = currentAnimationStates->findState(sourceCycle, sourceAction);
			AnimationState * targetState = currentAnimationStates->findState(targetCycle, targetAction);

			sourceState->setNextState(targetState);
		}
	}
	else
	{
		if (elem == "animation_states")
		{
			std::string name = atts["name"];
			currentAnimationStates = new AnimationStates();
			animationStateGraphMap[name] = currentAnimationStates;
		}
	}
}

void AnimationStateGraphFactory::onEndElement (const std::string &elem) {
	if( currentAnimationStates && elem=="animation_states" )
		currentAnimationStates = NULL;
}

AnimationStateGraphFactory::~AnimationStateGraphFactory() {
	OutputDebugString("Destroying AnimationStateGraphFactory...\n");

	// Destroy animationStateGraph map
	AnimationStateGraphMap::const_iterator it = animationStateGraphMap.begin();
	while(it !=  animationStateGraphMap.end()) {	
		AnimationStates *animationStates = (it->second);

		if(animationStates) {
			// Automaticaly animationState vector within animationStates destroies itself
			delete animationStates, animationStates=NULL;
		}
		++it;
	}
	animationStateGraphMap.clear();
}
