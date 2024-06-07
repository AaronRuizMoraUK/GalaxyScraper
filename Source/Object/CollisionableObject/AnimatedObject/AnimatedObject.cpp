#include "AnimatedObject.h"
#include "Object/CollisionableObject/AnimatedObject/AnimationGraph/AnimationState.h"
#include "Object/CollisionableObject/AnimatedObject/AnimationGraph/AnimationGraph.h"
#include "Object/CollisionableObject/AnimatedObject/AnimationGraph/AnimationRelation.h"
#include "Global/GlobalFunctions.h"
#include "Global/GlobalVariables.h"
#include "AnimationControllers/PlayerAnimation/PlayerAnimation.h"
#include "AnimationControllers/KeyFramed/KeyFramedROA.h"
#include "AnimationControllers/OscilatingPosition/OscilatingPositionROA.h"
#include "AnimationControllers/EnemyAnimation/EnemyAnimation.h"
#include "AnimationControllers/AssAnimation/AssAnimation.h"
#include "AnimationControllers/BushAnimation/BushAnimation.h"
#include "AnimationControllers/MouthAnimation/MouthAnimation.h"
#include "AnimationControllers/TongueAnimation/TongueAnimation.h"
#include "AnimationControllers/FireballAnimation/FireballAnimation.h"
#include "AnimationControllers/BoilerAnimation/BoilerAnimation.h"


// Default constructor
AnimatedObject::AnimatedObject()
: CollisionableObject()
, animationController(NULL)
, calAnimModel(NULL)
, alpha(0.0f)
, delta(0.0f)
, currentSpeed(0.0f)
, speedMultiplier(1.0f)
, defaultSpeedMultiplier(1.0f)
{
	D3DXMatrixIdentity(&previousTransformMatrix);
}

// Override copy constructor
AnimatedObject::AnimatedObject(const AnimatedObject &source)
: CollisionableObject(source)
, animationController(NULL)
, calAnimModel(NULL)
{
	copy(source);
}

void AnimatedObject::clear() {
	// Destroy Animation controller
	if(animationController)
		delete animationController, animationController=NULL;

	// Destroy calAnimModel
	if(calAnimModel)
		delete calAnimModel, calAnimModel=NULL;
}

void AnimatedObject::copy(const AnimatedObject &source) {
	animationGraph = source.animationGraph;

	if( source.animationController != NULL ) {
		const std::string & controller_type = source.animationController->getControllerType();

		// TODO Check all copy constructors
		if(controller_type == "player_animation")
			animationController = new PlayerAnimation( *((PlayerAnimation *) source.animationController) );

		else if(controller_type == "oscilating_position_roa")
			animationController = new OscilatingPositionROA( *((OscilatingPositionROA *) source.animationController) );

		else if(controller_type == "keyframed_roa")
			animationController = new KeyFramedROA( *((KeyFramedROA *) source.animationController) );

		else if(controller_type == "bush_animation")
			animationController = new BushAnimation( *((BushAnimation *) source.animationController) );

		else if(controller_type == "ass_animation")
			animationController = new AssAnimation( *((AssAnimation *) source.animationController) );

		else if(controller_type == "mouth_animation")
			animationController = new MouthAnimation( *((MouthAnimation *) source.animationController) );

		else if(controller_type == "tongue_animation")
			animationController = new TongueAnimation( *((TongueAnimation *) source.animationController) );

		else if(controller_type == "enemy_animation")
			animationController = new EnemyAnimation( *((EnemyAnimation *) source.animationController) );

		else if(controller_type == "fireball_animation")
			animationController = new FireballAnimation( *((FireballAnimation *) source.animationController) );

		else if(controller_type == "boiler_animation")
			animationController = new BoilerAnimation( *((BoilerAnimation *) source.animationController) );

		else {
			assert( !"Not type of animation controller allowed in AnimatedObject" );
			return;
		}
	}

	if( source.calAnimModel != NULL )
		calAnimModel = new CalAnimModel( *(source.calAnimModel) );

	delta = source.delta;
	alpha = source.alpha;
	currentSpeed=0.0f;
	speedMultiplier = source.getSpeedMultiplier();
	defaultSpeedMultiplier = source.getDefaultSpeedMultiplier();
	
	previousTransformMatrix = source.previousTransformMatrix;
}

const std::string & AnimatedObject::getCurrentCycle() const
{
	return animationGraph.getCycle();
}

const std::string & AnimatedObject::getCurrentAction() const 
{
	return animationGraph.getAction();
}

void AnimatedObject::updateAnimation(float deltaTime) {
	if (animationController)
		animationController->update(*this, deltaTime);
}

void AnimatedObject::renderCal() const
{
	Global::world.update(transformMatrix);

	// Calculate World View Projection matrix
	const D3DXMATRIX &matView = Global::camera->getMatView();
	const D3DXMATRIX &matProj = Global::camera->getMatProj();
	D3DXMATRIX matWorlViewProj;
	D3DXMatrixMultiply(&matWorlViewProj, &matView, &matProj);
	D3DXMatrixMultiply(&matWorlViewProj, &(Global::world.getMatWorld()), &matWorlViewProj);

	for(unsigned int i=0; i<materialVector.size(); ++i) {
		materialVector[i]->setMatWorldViewProj(matWorlViewProj);
		materialVector[i]->activateCal( getCalModel() );
		materialVector[i]->activate();
		
		UINT cPasses, iPass;
		const Effect &shader = materialVector[i]->getShader();
		shader->Begin( &cPasses, 0 ); // How many passes has the technique?
		for( iPass = 0; iPass < cPasses; ++iPass ) { // For each pass
			shader->BeginPass( iPass );	// Begin pass
		
			// Do the real rendering of geometry
			mesh->renderStrip(i);
 
			shader->EndPass( );	// End Pass
		}
		shader->End( );
	}

	/*
	// DRAW BOUNDING BOX, AXIS BOUNDING SPHERE, AXIS AND DIRECTION
	Global::world.activate();
	mesh->drawBoundingBox();
	mesh->drawAxisBoundingSphere();
	Global::drawAxis();
	Global::drawDirection();
	*/
}

void AnimatedObject::render() const {

	if(!calAnimModel) {
		CollisionableObject::render();
	}
	else {
		renderCal();
		
		return;

		//Global::world.updateScale(transformMatrix, D3DXVECTOR3(0.01f, 0.01f, 0.01f));
		//Global::world.updateScale(transformMatrix, D3DXVECTOR3(10.0f, 10.0f, 10.0f));
		Global::world.update(transformMatrix);
		Global::world.activate();

		renderCalSkeleton();
		
		// Get the matrix in local space of first bone
		/*
		CalBone *bone = calAnimModel->calModel->getSkeleton()->getBone(0);
		const CalVector &boneCalPosition = bone->getTranslationBoneSpace();
		const CalQuaternion &boneCalQRotation = bone->getRotationBoneSpace();
		const CalMatrix boneCalRotation = boneCalQRotation;
		D3DXMATRIX boneTransform;
		D3DXMatrixIdentity(&boneTransform);
		boneTransform(0,0) = boneCalRotation.dxdx;
		boneTransform(0,1) = boneCalRotation.dydx;
		boneTransform(0,2) = boneCalRotation.dzdx;
		boneTransform(1,0) = boneCalRotation.dxdy;
		boneTransform(1,1) = boneCalRotation.dydy;
		boneTransform(1,2) = boneCalRotation.dzdy;
		boneTransform(2,0) = boneCalRotation.dxdz;
		boneTransform(2,1) = boneCalRotation.dydz;
		boneTransform(2,2) = boneCalRotation.dzdz;
		boneTransform(3,0) = boneCalPosition.x;
		boneTransform(3,1) = boneCalPosition.y;
		boneTransform(3,2) = boneCalPosition.z;

		// Bone in world space. Multiply per transform matrix
		// Draw BOUNDING BOX and AXIS BOUNDING SPHERE
		D3DXMatrixMultiply(&boneTransform, &boneTransform, &transformMatrix);
		Global::world.update(boneTransform);
		Global::world.activate();
		mesh->drawBoundingBox();
		mesh->drawAxisBoundingSphere();
		*/
		
		// Draw AXIS and DIRECTION
		/*Global::world.update(transformMatrix);
		Global::world.activate();
		Global::drawAxis();
		Global::drawDirection();*/

		// Draw cyles
		int screenPosition=120;
		typedef std::list<CalAnimationCycle *> LCycles;
		LCycles & cycles = calAnimModel->calModel->getMixer()->getAnimationCycle();
		LCycles::iterator itC = cycles.begin();
		while (itC!=cycles.end()){
			CalAnimationCycle * cycle = *itC;
			float current_time = calAnimModel->calModel->getMixer()->getAnimationTime();
			float duration = cycle->getCoreAnimation()->getDuration();
			float percentage = current_time*100/duration;
			const std::string name = cycle->getCoreAnimation()->getFilename();
			Global::print(0,screenPosition, 0xffff0000, "Cycle %s: %f seg -- %f %%", name.c_str(), current_time, percentage);
			screenPosition+=10;

			++itC;
		}

		// Draw actions
		typedef std::list<CalAnimationAction *> LActions;
		LActions & actions = calAnimModel->calModel->getMixer()->getAnimationActionList();
		LActions::iterator itA = actions.begin();
		while (itA!=actions.end()){
			CalAnimationAction * action = *itA;
			float current_time = action->getTime();
			float duration = action->getCoreAnimation()->getDuration();
			float percentage = current_time*100/duration;
			const std::string name = action->getCoreAnimation()->getFilename();
			Global::print(0,screenPosition, 0xffff0000, "Action %s: %f seg -- %f %%", name.c_str(), current_time, percentage);
			screenPosition+=10;

			++itA;
		}

	}
}

void AnimatedObject::renderCalSkeleton () const {

	float positions[120][6];
	int numBones = calAnimModel->calModel->getSkeleton()->getBoneLines(&positions[0][0]);
	
	// Render Skeleton
	for(int i=0; i<numBones; ++i) {
		D3DXVECTOR3 line1 (&positions[i][0]);
		D3DXVECTOR3 line2 (&positions[i][3]);
		Global::drawLine(line1, line2, 0xFF0000FF);
	}
}

// -----------------------------------------------------
// LEVER OBJECT

LeverObject::LeverObject()
: AnimatedObject()
, leverActivated(false)
, openedDoor(false)
, baseMesh(NULL) {
	D3DXMatrixIdentity(&baseLeverTransformMatrix);
	D3DXMatrixIdentity(&initialTransformMatrix);
}

void LeverObject::render() const {
	// Render lever
	AnimatedObject::render();

	// Render lever base
	Global::world.update(baseLeverTransformMatrix);

	// Calculate World View Projection matrix
	const D3DXMATRIX &matView = Global::camera->getMatView();
	const D3DXMATRIX &matProj = Global::camera->getMatProj();
	D3DXMATRIX matWorlViewProj;
	D3DXMatrixMultiply(&matWorlViewProj, &matView, &matProj);
	D3DXMatrixMultiply(&matWorlViewProj, &(Global::world.getMatWorld()), &matWorlViewProj);

	for(unsigned int i=0; i<materialVector.size(); ++i) {
		materialVector[i]->setMatWorldViewProj(matWorlViewProj);
		materialVector[i]->activate();

		UINT cPasses, iPass;
		const Effect &shader = materialVector[i]->getShader();
		shader->Begin( &cPasses, 0 ); // How many passes has the technique?
		for( iPass = 0; iPass < cPasses; ++iPass ) { // For each pass
			shader->BeginPass( iPass );	// Begin pass
		
			// Do the real rendering of geometry
			baseMesh->renderStrip(i);
 
			shader->EndPass( );	// End Pass
		}
		shader->End( );
	}
}

// -----------------------------------------------------
// CANON OBJECT

CanonObject::CanonObject()
: AnimatedObject()
, canonActivated(false)
, canonMoved(false)
, canonShot(false)
, boilerHit(false)
, baseMesh(NULL) {
	D3DXMatrixIdentity(&baseCanonTransformMatrix);
	D3DXMatrixIdentity(&initialTransformMatrix);
}

void CanonObject::render() const {
	// Render Canon
	AnimatedObject::render();

	// Render Canon base
	Global::world.update(baseCanonTransformMatrix);

	// Calculate World View Projection matrix
	const D3DXMATRIX &matView = Global::camera->getMatView();
	const D3DXMATRIX &matProj = Global::camera->getMatProj();
	D3DXMATRIX matWorlViewProj;
	D3DXMatrixMultiply(&matWorlViewProj, &matView, &matProj);
	D3DXMatrixMultiply(&matWorlViewProj, &(Global::world.getMatWorld()), &matWorlViewProj);

	for(unsigned int i=0; i<materialVector.size(); ++i) {
		materialVector[i]->setMatWorldViewProj(matWorlViewProj);
		materialVector[i]->activate();

		UINT cPasses, iPass;
		const Effect &shader = materialVector[i]->getShader();
		shader->Begin( &cPasses, 0 ); // How many passes has the technique?
		for( iPass = 0; iPass < cPasses; ++iPass ) { // For each pass
			shader->BeginPass( iPass );	// Begin pass
		
			// Do the real rendering of geometry
			baseMesh->renderStrip(i);
 
			shader->EndPass( );	// End Pass
		}
		shader->End( );
	}
}
