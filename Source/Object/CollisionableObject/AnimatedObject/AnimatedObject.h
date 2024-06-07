#ifndef ANIMATED_OBJECT_H
#define ANIMATED_OBJECT_H

#include "Object/CollisionableObject/CollisionableObject.h"
#include "AnimationControllers/IAnimationController.h"
#include "cal3d/cal3d.h"
#include "Object/CollisionableObject/AnimatedObject/AnimationGraph/AnimationGraph.h"
#include <map>

typedef std::map<std::string, int> IdAnimMap;

/**
* This struct has cal3d information and all the ids
* of the animations and mesh
*/

typedef struct CalAnimModelStruct {
	// Cal3D model
	CalModel *calModel;

	// Cal3D mesh Id
	int meshId;

	// Cal3D animations Id
	IdAnimMap *idAnimMap;

	// Default constructor
	CalAnimModelStruct() : calModel(NULL), meshId(0), idAnimMap(NULL) {};

	// Override copy constructor
	CalAnimModelStruct(const CalAnimModelStruct &source) {
		meshId = source.meshId;
		idAnimMap = source.idAnimMap;

		calModel = new CalModel( source.calModel->getCoreModel() );
		// Source's calModel status is not copied
		calModel->attachMesh(meshId);
	};

	// Destructor
	~CalAnimModelStruct() {
		OutputDebugString("Destroying CalAnimModel...\n");
		if(calModel)
			delete calModel, calModel=NULL;

		// The CalCoreModelFactory is who has to delete the idAnimMap
		idAnimMap = NULL;
	};

	// Override Operator '='
	void operator=(const CalAnimModelStruct &source) {
		if(calModel)
			delete calModel, calModel=NULL;

		// The CalCoreModelFactory is who has to delete the idAnimMap
		idAnimMap = NULL;

		meshId = source.meshId;
		idAnimMap = source.idAnimMap;

		calModel = new CalModel( source.calModel->getCoreModel() );
		// Source's calModel status is not copied
		calModel->attachMesh(meshId);
	};
} CalAnimModel;

/**
* A animated object in scene. It is an Object that is animated
* with cal3d, so it has an animation graph, an animation controller
* and a cal3d animation model
*/

class AnimatedObject : public CollisionableObject
{
protected:
	AnimationGraph animationGraph;

	IAnimationController *animationController;

	CalAnimModel *calAnimModel;

	// Angles delta and alpha in planet
	float delta;
	float alpha;

	// Previous state for collision
	D3DXMATRIX previousTransformMatrix;

	// TODO This must to be in animation controllers
	float currentSpeed;
	float speedMultiplier;
	float defaultSpeedMultiplier;

public:
	// Default constructor
	AnimatedObject();
	// Override copy constructor
	AnimatedObject(const AnimatedObject &source);
	// Destructor
	virtual ~AnimatedObject() {
		OutputDebugString("Destroying AnimatedObject...\n");
		clear();
	};
	// Override Operator '='
	virtual void operator=(const AnimatedObject &source) {
		CollisionableObject::operator=(source);
		clear();
		copy(source);
	};

	float getCurrentSpeed() const
	{
		return currentSpeed;
	};

	float getSpeedMultiplier() const
	{
		return speedMultiplier;
	};

	float getDefaultSpeedMultiplier() const
	{
		return defaultSpeedMultiplier;
	};

	void setCurrentSpeed(float pSpeed)
	{
		currentSpeed = pSpeed;
	};

	void setSpeedMultiplier(float pSpeed)
	{
		speedMultiplier = pSpeed;
	};

	void setDefaultSpeedMultiplier(float pSpeed)
	{
		defaultSpeedMultiplier = pSpeed;
	}

	void updateAnimation(float deltaTime=(1.0f/60.0f));

	void setAnimationController(IAnimationController *pAnimationController)
	{
		if(animationController)
			delete animationController, animationController=NULL;
		animationController = pAnimationController;
	};

	IAnimationController * getAnimationController() const { return animationController; }

	const std::string & getCurrentCycle() const;
	const std::string & getCurrentAction() const;

	const AnimationRelation * getAnimationRelation() const { 
		return animationGraph.getPreviousToCurrentRelation();
	};

	const D3DXMATRIX & getPreviousTransformMatrix() const {
		return previousTransformMatrix;
	};

	void setCurrentAnimationState(AnimationState *pAnimationState)
	{
		animationGraph.setCurrentState(pAnimationState);	
	};

	void setRelationToState(ActionType actionType, const std::string &nameState, bool hasFinished, bool aloneAnimation = false){
		animationGraph.setRelationToState(actionType, nameState, hasFinished, aloneAnimation);
	};

	void moveAnimationState() {
		animationGraph.moveAnimationState();
	};

	const CalModel * getCalModel() const {
		if(calAnimModel)
			return calAnimModel->calModel;
		return NULL;
	};
	CalModel * getCalModel() {
		if(calAnimModel)
			return calAnimModel->calModel;
		return NULL;
	};

	int getCalIdAnim(const std::string &name) const {
		if(calAnimModel) {
			IdAnimMap::const_iterator it = calAnimModel->idAnimMap->find(name);
			if( it != calAnimModel->idAnimMap->end() )
				return it->second;
			return -1;
		}
		return -1;
	};

	void setCalAnimModel(CalAnimModel *pCalAnimModel)
	{
		if(calAnimModel)
			delete calAnimModel, calAnimModel=NULL;
		calAnimModel = pCalAnimModel;
	};

	float getAlpha() const {return alpha;};
	float getDelta() const {return delta;};
	void setAlpha(float pAlpha) {alpha = pAlpha;};
	void setDelta(float pDelta) {delta = pDelta;};

	const AnimationGraph * getAnimationGraph() const {return &animationGraph;};
	AnimationGraph * getAnimationGraph() {return &animationGraph;};

	void refreshPreviousTransformMatrix() {
		previousTransformMatrix = transformMatrix;
	}

	virtual void render() const;

private:
	void renderCalSkeleton () const;
	void renderCal() const;
	void clear();
	void copy(const AnimatedObject &source);
	
};

// -----------------------------------------------------
// LEVER OBJECT

class LeverObject : public AnimatedObject
{
protected:
	bool leverActivated;
	bool openedDoor;

	// Mesh of lever base
	const IMesh *baseMesh;
	D3DXMATRIX baseLeverTransformMatrix;

	D3DXMATRIX initialTransformMatrix;

public:
	LeverObject();    
    ~LeverObject() {
		OutputDebugString("Destroying LeverObject...\n");

		// LeverObject does not destroy its base_mesh, MeshFactory does!!.
	};
		
	const IMesh * getBaseMesh() const {
		return baseMesh;
	};

	void setBaseMesh(IMesh *pBaseMesh) {
		baseMesh=pBaseMesh;
	};

	const D3DXMATRIX & getBaseLeverTransformMatrix() const {
		return baseLeverTransformMatrix;
	};

	void setBaseLeverTransformMatrix(const D3DXMATRIX &pTransformMatrix) {
		baseLeverTransformMatrix = pTransformMatrix;
	};

	void saveInitialTransformMatrix() {
		initialTransformMatrix = getTransformMatrix();
	}

	void restart() {
		leverActivated = false;
		openedDoor = false;
		setTransformMatrix( initialTransformMatrix );
		if( animationController )
		{
			D3DXQUATERNION rotationQuaternion;
			D3DXQuaternionRotationMatrix(&rotationQuaternion, &getTransformMatrix());
			animationController->restart( getPosition(), rotationQuaternion );
		}
	}

	void render() const;

	bool getLeverActivated() const {return leverActivated;};
	void setLeverActivated(bool pLeverActivated) { leverActivated = pLeverActivated; };
	bool getOpenedDoor() const { return openedDoor;};
	void setOpenedDoor(bool pOpenedDoor) { openedDoor = pOpenedDoor; };
	
};

// -----------------------------------------------------
// CANON OBJECT

class CanonObject : public AnimatedObject
{
protected:
	bool canonActivated;
	bool canonMoved;
	bool canonShot;
	bool boilerHit;

	// Mesh of Canon base
	const IMesh *baseMesh;
	D3DXMATRIX baseCanonTransformMatrix;

	D3DXMATRIX initialTransformMatrix;
public:
	CanonObject();    
    ~CanonObject() {
		OutputDebugString("Destroying CanonObject...\n");

		// CanonObject does not destroy its base_mesh, MeshFactory does!!.
	};
		
	const IMesh * getBaseMesh() const {
		return baseMesh;
	};

	void setBaseMesh(IMesh *pBaseMesh) {
		baseMesh=pBaseMesh;
	};

	const D3DXMATRIX & getBaseCanonTransformMatrix() const {
		return baseCanonTransformMatrix;
	};

	void setBaseCanonTransformMatrix(const D3DXMATRIX &pTransformMatrix) {
		baseCanonTransformMatrix = pTransformMatrix;
	};

	void saveInitialTransformMatrix() {
		initialTransformMatrix = getTransformMatrix();
	}

	void restart() {
		canonActivated = false;
		canonMoved = false;
		canonShot = false;
		boilerHit = false;
		setTransformMatrix( initialTransformMatrix );
		if( animationController )
		{
			D3DXQUATERNION rotationQuaternion;
			D3DXQuaternionRotationMatrix(&rotationQuaternion, &getTransformMatrix());
			animationController->restart( getPosition(), rotationQuaternion );
		}
	}

	void render() const;

	bool getCanonActivated() const {return canonActivated;};
	void setCanonActivated(bool pCanonActivated) { canonActivated = pCanonActivated; };
	bool getCanonShot() const { return canonShot;};
	void setCanonShot(bool pCanonShot) { canonShot = pCanonShot; };
	bool getBoilerHit() const {return boilerHit;};
	void setBoilerHit(bool pBoilerHit) {boilerHit = pBoilerHit;};
	bool getCanonMoved() const {return canonMoved;};
	void setCanonMoved(bool pCanonMoved) {canonMoved = pCanonMoved;};
	
};

#endif //ANIMATED_OBJECT_H
