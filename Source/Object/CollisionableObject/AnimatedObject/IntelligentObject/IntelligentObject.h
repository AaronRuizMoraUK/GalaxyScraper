#ifndef INTELLIGENT_OBJECT_H
#define INTELLIGENT_OBJECT_H

#include "Object/CollisionableObject/AnimatedObject/AnimatedObject.h"
#include "AIControllers/IAIController.h"
#include "Util/SpatialPartitioner/PolarAnglesPartitioner.h"

/**
* This is an intelligent object. It is an AnimatedObject with an artificial
* intelligent controller
*/

class IntelligentObject : public AnimatedObject
{
protected:
	IAIController *aiController;

	// Grid in Space
	PAGridIndex spaceGrid;

public:
	// Default constructor
	IntelligentObject();
	// Override copy constructor
	IntelligentObject(const IntelligentObject &source);
	// Destructor
	~IntelligentObject() {
		OutputDebugString("Destroying IntelligentObject...\n");
		clear();
	};
	// Override Operator '='
	void operator=(const IntelligentObject &source) {
		AnimatedObject::operator=(source);
		clear();
		copy(source);
	};

	void updateAI(float deltaTime=(1.0f/60.0f));

	IAIController * getAIController() const {return aiController;};
	void setAIController(IAIController *pAiController) {
		if (aiController)
			delete aiController, aiController=NULL;
		aiController=pAiController;
	};

	const PAGridIndex & getSpatialGrid() const { return spaceGrid; };
	void setSpatialGrid(const PAGridIndex &pSpaceGrid) {
		assert( spaceGrid.size() == 2 && pSpaceGrid.size() == 2 );
		spaceGrid[0]=pSpaceGrid[0];
		spaceGrid[1]=pSpaceGrid[1];
	};

private:
	void clear();
	void copy(const IntelligentObject &source);
};

#endif //INTELLIGENT_OBJECT_H
