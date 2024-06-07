#include "BoilerFireballManager.h"
#include "Object/CollisionableObject/AnimatedObject/AnimatedObject.h"
#include "Global/GlobalFunctions.h"
#include "Global/GlobalVariables.h"
#include "Factories/MeshFactory.h"
#include "Factories/CalCoreModelFactory.h"
#include "Factories/AnimationStateGraphFactory.h"
#include "AnimationControllers/FireballAnimation/FireballAnimation.h"
#include "AIControllers/FireballBehaviour/FireballBehaviour.h"
#include "AIControllers/CanonBallBehaviour/CanonBallBehaviour.h"
#include "Managers/PlanetManager.h"
#include "Managers/DataLocatorManager.h"
#include "Managers/ParticlesManager.h"
#include <fstream>
#include <cassert>

BoilerFireballManager * BoilerFireballManager::boilerFireballManager = NULL;

BoilerFireballManager * BoilerFireballManager::getBoilerFireballManager() {
	if(boilerFireballManager == NULL)
		boilerFireballManager = new BoilerFireballManager();

	return boilerFireballManager;
}
 
BoilerFireballManager::BoilerFireballManager()
: currentParsed(NULL)
, currentParsedElementName("")
{
	init();
}

void BoilerFireballManager::init()
{
	DataLocatorManager *dataLocatorManager = DataLocatorManager::getDataLocatorManager();
	const std::string &pathToXml = dataLocatorManager->getPath("xml");

	std::string xmlFile = pathToXml + "/boiler/boiler_fireballs.xml";
	bool ok = xmlParseFile(xmlFile);
	assert(ok);
}

void BoilerFireballManager::onEndElement (const std::string &elem)
{
	if( elem == "animated_object" )
	{
		(*this)[currentParsedElementName] = currentParsed;
		// push_back(currentParsed);
		currentParsed = NULL;
	}
}

void BoilerFireballManager::onStartElement (const std::string &elem, MKeyValue &atts) 
{
	if( currentParsed ) {
		createElement( elem, atts );
		return;
	}

	if( elem == "animated_object" )
	{
		currentParsed = new IntelligentObject();
		currentParsedElementName = atts["name"];
	}
}

void BoilerFireballManager::createElement(const std::string &elem, MKeyValue &atts) 
{
	if( elem == "localization" )
	{
		std::string position=atts["transform"];
		D3DXMATRIX matrix;
		D3DXMatrixIdentity(&matrix);
		sscanf_s(position.c_str(),"%f %f %f %f %f %f %f %f %f %f %f %f",
			&matrix(0,0), &matrix(0,1), &matrix(0,2),
			&matrix(1,0), &matrix(1,1), &matrix(1,2),
			&matrix(2,0), &matrix(2,1), &matrix(2,2),
			&matrix(3,0), &matrix(3,1), &matrix(3,2));

		// Boiler planet center and radius
		const Planet *planet = PlanetManager::getPlanetManager()->getPlanet("boiler");
		const D3DXVECTOR3 &center = planet->getCenter();
		float radius = planet->getRadius();

		currentParsed->setTransformMatrix(matrix);
		currentParsed->setPosition(currentParsed->getPosition() + center);

		D3DXVECTOR3 pos = currentParsed->getPosition();
		const D3DXVECTOR3 &localFrontVector = currentParsed->getLocalFrontVector();
	
		// Intersection point between line and sphere
		float t = 0;
		float denominator = pow(pos.x-center.x, 2) + pow(pos.y-center.y, 2) + pow(pos.z-center.z, 2);
		if (denominator!=0)
			t = radius / sqrt(denominator);
		// New position (point in the sphere)
		// pos.x = center.x + t*(pos.x-center.x);
		// pos.y = center.y + t*(pos.y-center.y);
		// pos.z = center.z + t*(pos.z-center.z);

		float d = Global::distanceSqrt(pos, center);

		// Obtain Object's new up vector
		D3DXVECTOR3 newLocalUpVector(pos.x-center.x, pos.y-center.y, pos.z-center.z);
		Global::normalizeVector3(newLocalUpVector, newLocalUpVector);

		// pos -= newLocalUpVector * 4.0f;
		currentParsed->setPosition(pos);

		d = Global::distanceSqrt(pos, center);

		// Obtain Player's new left vector
		D3DXVECTOR3 newLocalLeftVector;
		D3DXVec3Cross(&newLocalLeftVector, &newLocalUpVector, &localFrontVector);
		Global::normalizeVector3(newLocalLeftVector, newLocalLeftVector);

		const D3DXVECTOR3 &oldLocalUpVector = currentParsed->getLocalUpVector();
		D3DXVECTOR3 upDifference = newLocalUpVector - oldLocalUpVector;
		float difference = D3DXVec3Length( &upDifference );
		if( abs(difference) < 0.00001 )
			return;

		// Obtain Player's new front vector
		D3DXVECTOR3 newLocalFrontVector;
		D3DXVec3Cross(&newLocalFrontVector, &newLocalLeftVector, &newLocalUpVector);

		// Set the new transform matrix
		const D3DXMATRIX &transform = currentParsed->getTransformMatrix();
		D3DXMATRIX newTransform = transform;
		newTransform(0,0) = newLocalLeftVector.x;
		newTransform(0,1) = newLocalLeftVector.y;
		newTransform(0,2) = newLocalLeftVector.z;
		newTransform(1,0) = newLocalUpVector.x;
		newTransform(1,1) = newLocalUpVector.y;
		newTransform(1,2) = newLocalUpVector.z;
		newTransform(2,0) = newLocalFrontVector.x;
		newTransform(2,1) = newLocalFrontVector.y;
		newTransform(2,2) = newLocalFrontVector.z;

		// Set new transform matrix and new position
		currentParsed->setTransformMatrix(newTransform);
		currentParsed->setPosition(pos);

	}
	else if( elem == "mesh" )
	{
		std::string name = atts["name"];
		IMesh *mesh = MeshFactory::getMeshFactory( )->createMesh( name );
		assert( mesh );
		currentParsed->setMesh( mesh );

		// Now mesh exists, create collision object
		ShapeHandle *collisionShapeHandle = 
			CollisionShapeFactory::getCollisionShapeFactory()->createCollisionShape("fireball"
			, currentParsed->getMesh(), FIREBALL_SHAPE, false);
		assert(collisionShapeHandle);
		DT_ObjectHandle collisionObjectHandle = DT_CreateObject(currentParsed, collisionShapeHandle->shapeHandle);
		currentParsed->setCollisionShape(collisionShapeHandle);
		currentParsed->setCollisionObject(collisionObjectHandle);
		// Do know the setTransformMatrix for update also the collision position in world
		currentParsed->setTransformMatrix( currentParsed->getTransformMatrix() );
		
	}
	else if( elem == "material" )
	{
		std::string texture = atts["name"];
		std::string shader = atts["shader"];
		std::string technique = atts["technique"];
		currentParsed->SETMATERIAL(texture, shader, technique);
 	}
	else if( elem == "animation_controller" )
	{
		
		IAnimationController *animationController = NULL;

		std::string animControllerName = atts["name"];
		bool collisionable = atts.getBool("collisionable", true);

		// If this ball is collisionable insert to the collisionable fireballs vector
		if (collisionable)
			collisionableFireballs.push_back(currentParsed);

		if( animControllerName=="fireball_animation" )
		{
			float timeSequence = atts.getFloat("time_sequence", 0.0f);
			float timeMovement = atts.getFloat("time_movement", 0.0f);
			float speed = atts.getFloat("speed", 0.0f);
			float angle = atts.getFloat("angle", 0.0f);
			std::string behaviour = atts["aicontroller"];

			FireballAnimation *fa = new FireballAnimation();
			fa->setInitialPosition(currentParsed->getPosition());
			fa->setSpeeds(speed * cos(angle), speed * sin(angle));
			fa->setTransformMatrix(currentParsed->getTransformMatrix());
			animationController = fa;

			IAIController *aiController;

			if (behaviour == "fireball_behaviour")
			{
				aiController = new FireBallBehaviour(timeSequence, timeMovement);
			}
			else if (behaviour == "canonball_behaviour")
			{
				aiController = new CanonBallBehaviour(timeSequence, timeMovement);
			}

			currentParsed->setAIController(aiController);
		}
		else {
			assert( !"Not type of animation constroller allowed in BoilerAnimatedObjectManager" );
			return;
		}
		
		if( animationController )
			currentParsed->setAnimationController(animationController);
		
	}
	else if ( elem == "behaviour")
	{
		
	}
}

void BoilerFireballManager::updateAI(float deltaTime)
{
	iterator it = begin();
	while (it != end() ) 
	{
		it->second->updateAI(deltaTime);
		++it;
	}
}

void BoilerFireballManager::updateAnimation(float deltaTime)
{
	iterator it = begin();
	while (it != end() ) 
	{
		it->second->updateAnimation(deltaTime);
		++it;
	}
}

void BoilerFireballManager::render() const {
	const_iterator i = begin();

#ifdef DEBUG
	int staticElementsDrawn = 0;
#endif

	while(i!=end()) {
		const Object *obj = i->second;

		const IMesh * mesh = obj->getMesh();
		const D3DXVECTOR3 &center = mesh->getBoundingSphereCenter();
		float radius = mesh->getBoundingSphereRadius();

		D3DXVECTOR3 centerInstance = center + obj->getPosition();
		if( Global::frustum.isBSphereVisible(centerInstance, radius) &&
			Global::backSideFrustum.isBSphereVisible(centerInstance, radius) ) {
			obj->render();
		}
		++i;
	}

#ifdef DEBUG
	Global::print(0,40, 0xffff0000, "Animated objects drawn: %d", staticElementsDrawn);
#endif 
}

void BoilerFireballManager::cleanUp() {
	collisionableFireballs.clear();

	iterator it = begin();
	while(it != end())
	{	
		IntelligentObject *intelligentObject = (it->second);
		if(intelligentObject)
			delete intelligentObject, intelligentObject=NULL;
		++it;
	}
	clear();
}


IntelligentObject * BoilerFireballManager::getIntelligentObject( const std::string &name ) const {
	const_iterator it = find( name );
	if( it == end() ) {
		assert( !"Iuranus animated object that not exist in BoilerAnimatedObjectManager" );
		return NULL;
	}

	return it->second;
}

void BoilerFireballManager::reloadShader() {
	iterator it = begin();

	while( it != end() ) {
		it->second->reloadShader();
		++it;
	}
}

void  BoilerFireballManager::shootBall(const std::string &name)
{
	iterator it = find( name );
	if( it == end() ) {
		assert( !"Iuranus animated object that not exist in BoilerAnimatedObjectManager" );
	}

	CanonBallBehaviour *cbb = (CanonBallBehaviour *) it->second->getAIController();
	cbb->setObjectMoving(*(it->second));
}

void BoilerFireballManager::renewParticles()
{
}
