#ifndef ISPATIAL_PARTITIONER_H
#define ISPATIAL_PARTITIONER_H

#include <Windows.h>
#include <vector>
#include <set>

class Object;
class AnimatedObject;
class IntelligentObject;

/**
* Interface of a spatial partitioner. It stores both
* static objects (do not move in space) and movable
* object (move in space)
*/

// Type of seek enumeration for 'WhereIs' functions
typedef enum TypeSeekEnum {STATIC, MOVABLE, ANY} TypeSeek;

template<class GridIndex>
class ISpatialPartitioner
{
public:
	ISpatialPartitioner() {};
	virtual ~ISpatialPartitioner()
	{
		OutputDebugString("Destroying ISpatialPartitioner...\n");
	};

	virtual void cleanSpace() = 0;

	virtual bool addStaticObject(Object *staticObject) = 0;
	virtual bool addMovableObject(IntelligentObject *movableObject) = 0;

	virtual void removeStaticObject(Object *staticObject) = 0;
	virtual void removeMovableObject(IntelligentObject *movableObject) = 0;

	// Normaly not used -- use update movable objects one time for move all objects in space
	virtual void moveMovableObject(IntelligentObject *movableObject) = 0;

	virtual void updateSpace() = 0;

	virtual void whereIs(Object *object, TypeSeek typeSeek, std::vector<GridIndex> &output) const = 0;
	virtual void whereIs(IntelligentObject *movableObject, GridIndex &output) const = 0;
	virtual void whichGridsIndexWouldBe(Object *object, std::vector<GridIndex> &output) const = 0;
	virtual void whichGridsIndexWouldBe(AnimatedObject *object, GridIndex &output) const = 0;

	virtual void getObjectsInGridIndex(const GridIndex &gridIndex, std::set<Object *> &output) const = 0;
	virtual void getObjectsInSameGrid(IntelligentObject *object, std::set<Object *> &output) const = 0;
	virtual void getAdjacentObjects(const GridIndex &gridIndex, std::set<Object *> &output) const = 0;
	virtual void getAdjacentObjectsTo(IntelligentObject *object, std::set<Object *> &output) const = 0;
	
	virtual void getMovableObjectsInGridIndex(const GridIndex &gridIndex, std::set<IntelligentObject *> &output) const = 0;
	virtual void getMovableObjectsInSameGrid(IntelligentObject *object, std::set<IntelligentObject *> &output) const = 0;
	virtual void getAdjacentMovableObjects(const GridIndex &gridIndex, std::set<IntelligentObject *> &output) const = 0;
	virtual void getAdjacentMovalbeObjectsTo(IntelligentObject *object, std::set<IntelligentObject *> &output) const = 0;

	virtual void getStaticObjectsInGridIndex(const GridIndex &gridIndex, std::set<Object *> &output) const = 0;
	virtual void getStaticObjectsInSameGrid(IntelligentObject *object, std::set<Object *> &output) const = 0;
	virtual void getAdjacentStaticObjects(const GridIndex &gridIndex, std::set<Object *> &output) const = 0;
	virtual void getAdjacentStaticObjectsTo(IntelligentObject *object, std::set<Object *> &output) const = 0;

	virtual void getAdjacentGridsIndex(const GridIndex &gridIndex, std::vector<GridIndex> &output) const = 0;
	virtual void getVisibleGridsIndex(const GridIndex &gridIndex, std::vector<GridIndex> &output) const = 0;
};

#endif // ISPATIAL_PARTITIONER_H
