#ifndef POLAR_ANGLES_PARTITIONER_H
#define POLAR_ANGLES_PARTITIONER_H

#include "ISpatialPartitioner.h"
#include <map>
#include <list>

/**
* A polar angles spatial partitioner. It divides the
* sphere space into grids using 2 angles. Grids on the
* poles are considered as an unique grid.
*
* It is a singleton.
*/

typedef std::vector<int> PAGridIndex;
typedef std::map<Object *, std::vector<PAGridIndex>> StaticObjectsInSpaceMap;
typedef std::map<IntelligentObject *, PAGridIndex> MovableObjectsInSpaceMap;

class PolarAnglesPartitioner : public ISpatialPartitioner< PAGridIndex >
{
protected:
	// Struct of a grid of space
	typedef struct GridStruct {
		std::set<Object *> staticObjectSet;
		std::set<IntelligentObject *> movableObjectSet;
	} Grid;

	// Space
	int gridsNumberY;
	int gridsNumberX;
	Grid **space;

	// List of objects in space
	StaticObjectsInSpaceMap staticObjectsInSpace;
	MovableObjectsInSpaceMap movableObjectsInSpace;

public:
	~PolarAnglesPartitioner();

	static PolarAnglesPartitioner * getPolarAnglesPartitioner();

	int getGridsNumberY() const { return gridsNumberY; };
	int getGridsNumberX() const { return gridsNumberX; };

	void cleanSpace();

	bool addStaticObject(Object *staticObject);
	bool addMovableObject(IntelligentObject *movableObject);

	void removeStaticObject(Object *staticObject);
	void removeMovableObject(IntelligentObject *movableObject);

	// Normaly not used -- use update movable objects one time for move all objects in space
	void moveMovableObject(IntelligentObject *movableObject);

	void updateSpace();

	void whereIs(Object *object, TypeSeek typeSeek, std::vector<PAGridIndex> &output) const;
	void whereIs(IntelligentObject *movableObject, PAGridIndex &output) const;
	void whichGridsIndexWouldBe(Object *object, std::vector<PAGridIndex> &output) const;
	void whichGridsIndexWouldBe(AnimatedObject *object, PAGridIndex &output) const;

	void getObjectsInGridIndex(const PAGridIndex &gridIndex, std::set<Object *> &output) const;
	void getObjectsInSameGrid(IntelligentObject *object, std::set<Object *> &output) const;
	void getAdjacentObjects(const PAGridIndex &gridIndex, std::set<Object *> &output) const;
	void getAdjacentObjectsTo(IntelligentObject *object, std::set<Object *> &output) const;
	
	void getMovableObjectsInGridIndex(const PAGridIndex &gridIndex, std::set<IntelligentObject *> &output) const;
	void getMovableObjectsInSameGrid(IntelligentObject *object, std::set<IntelligentObject *> &output) const;
	void getAdjacentMovableObjects(const PAGridIndex &gridIndex, std::set<IntelligentObject *> &output) const;
	void getAdjacentMovalbeObjectsTo(IntelligentObject *object, std::set<IntelligentObject *> &output) const;

	void getStaticObjectsInGridIndex(const PAGridIndex &gridIndex, std::set<Object *> &output) const;
	void getStaticObjectsInSameGrid(IntelligentObject *object, std::set<Object *> &output) const;
	void getAdjacentStaticObjects(const PAGridIndex &gridIndex, std::set<Object *> &output) const;
	void getAdjacentStaticObjectsTo(IntelligentObject *object, std::set<Object *> &output) const;

	void getAdjacentGridsIndex(const PAGridIndex &gridIndex, std::vector<PAGridIndex> &output) const;
	void getVisibleGridsIndex(const PAGridIndex &gridIndex, std::vector<PAGridIndex> &output) const;

private:
	static PolarAnglesPartitioner * polarAnglesPartitioner;
	PolarAnglesPartitioner();

	void moveMovableObject(IntelligentObject *movableObject, const PAGridIndex &gridsIndex);

	bool isRegistered(Object *staticObject) const;
	bool isRegistered(IntelligentObject *movableObject) const;
};

#endif // POLAR_ANGLES_PARTITIONER_H