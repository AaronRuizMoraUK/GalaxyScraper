#include "BoilerFixedParticlesManager.h"
#include "Particles/BirthPointsParticles/BirthPointsParticles.h"
#include "Particles/LavaBubblesParticles/LavaBubblesParticles.h"
#include "Factories/ParticleFactory.h"
#include "Global/GlobalFunctions.h"
#include "Global/GlobalVariables.h"
#include "Object/Object.h"
#include "Object/CollisionableObject/AnimatedObject/AnimatedObject.h"
#include "Managers/DataLocatorManager.h"
#include "Managers/PlanetManager.h"
#include <cassert>

BoilerFixedParticlesManager * BoilerFixedParticlesManager::boilerFixedParticlesManager = NULL;

BoilerFixedParticlesManager * BoilerFixedParticlesManager::getBoilerFixedParticlesManager() {
	if(boilerFixedParticlesManager == NULL)
		boilerFixedParticlesManager = new BoilerFixedParticlesManager();

	return boilerFixedParticlesManager;
}


BoilerFixedParticlesManager::BoilerFixedParticlesManager()
: currentType("")
, currentParent(NULL)
, currentNumQuads(0)
{
	init();
}

void BoilerFixedParticlesManager::init()
{
	DataLocatorManager *dataLocatorManager = DataLocatorManager::getDataLocatorManager();
	const std::string &pathToXml = dataLocatorManager->getPath("xml");

	std::string xmlFile = pathToXml + "boiler/boiler_fixed_particles.xml";
	bool ok = xmlParseFile(xmlFile);
	assert(ok);
}

void BoilerFixedParticlesManager::onEndElement (const std::string &elem)
{
	if( elem == "particle" )
	{
		addParticle(currentType, currentParent, currentNumQuads);

		currentType = "";
		currentParent = NULL;
		currentNumQuads = 0;
	}
}

void BoilerFixedParticlesManager::onStartElement (const std::string &elem, MKeyValue &atts) 
{
	if( elem == "particle" )
	{
		currentType = atts["type"];
		currentNumQuads = atts.getInt( "num_quads", 0 );

		std::string animation_type = atts["animation_type"];

		currentParent = new AnimatedObject();

		if (animation_type == "roa")
		{
			// TODO - MIRAR//HABLAR COMO GENERAR EL ANIMATION CONTROLLER
		}

	}
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

		currentParent->setTransformMatrix(matrix);
		currentParent->setPosition(currentParent->getPosition() + center);

		D3DXVECTOR3 pos = currentParent->getPosition();
		const D3DXVECTOR3 &localFrontVector = currentParent->getLocalFrontVector();
	
		// Intersection point between line and sphere
		float t = 0;
		float denominator = pow(pos.x-center.x, 2) + pow(pos.y-center.y, 2) + pow(pos.z-center.z, 2);
		if (denominator!=0)
			t = radius / sqrt(denominator);
		// New position (point in the sphere)
		pos.x = center.x + t*(pos.x-center.x);
		pos.y = center.y + t*(pos.y-center.y);
		pos.z = center.z + t*(pos.z-center.z);

		// Obtain Object's new up vector
		D3DXVECTOR3 newLocalUpVector(pos.x-center.x, pos.y-center.y, pos.z-center.z);
		Global::normalizeVector3(newLocalUpVector, newLocalUpVector);

		pos -= newLocalUpVector * 4;
		currentParent->setPosition(pos);

		// Obtain Player's new left vector
		D3DXVECTOR3 newLocalLeftVector;
		D3DXVec3Cross(&newLocalLeftVector, &newLocalUpVector, &localFrontVector);
		Global::normalizeVector3(newLocalLeftVector, newLocalLeftVector);

		const D3DXVECTOR3 &oldLocalUpVector = currentParent->getLocalUpVector();
		D3DXVECTOR3 upDifference = newLocalUpVector - oldLocalUpVector;
		float difference = D3DXVec3Length( &upDifference );
		if( abs(difference) < 0.00001 )
			return;

		// Obtain Player's new front vector
		D3DXVECTOR3 newLocalFrontVector;
		D3DXVec3Cross(&newLocalFrontVector, &newLocalLeftVector, &newLocalUpVector);

		// Set the new transform matrix
		const D3DXMATRIX &transform = currentParent->getTransformMatrix();
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
		currentParent->setTransformMatrix(newTransform);
		currentParent->setPosition(pos);
	}
}

void BoilerFixedParticlesManager::addParticle(const std::string &type, AnimatedObject *parent, int pNumQuads)
{
	assert( type != "" && parent && pNumQuads>0 );

	ParticleEmitter *p = NULL;

	if(type == BirthPointsParticles::type)
		p = new BirthPointsParticles();
	else if (type == LavaBubblesParticles::type)
		p = new LavaBubblesParticles();
	else {
		assert(!"Undefined particle type");
		return;
	}
	assert( p );

	bool isAnimated = ( parent->getAnimationController() != NULL ) ? true : false;
	p->setParent(parent, isAnimated);
	p->setNumQuads(pNumQuads);

	IMesh *mesh = ParticleFactory::getParticleFactory()->createParticle(pNumQuads);	
	assert(mesh);
	p->setMesh(mesh);

	p->SETMATERIAL(p->getTextureName(), "shader.fx", "particle_tech");

	p->generate();

	push_back(p);
}


void BoilerFixedParticlesManager::render() const {
	const_iterator it = begin();

	while(it!=end()) {
		// TODO Hacer algo para que las particulas que queden fuera del frustum no se vean
		(*it)->render();
		++it;
	}
}
	
BoilerFixedParticlesManager::iterator BoilerFixedParticlesManager::killParticle(BoilerFixedParticlesManager::iterator &particleIterator)
{
	ParticleEmitter *particle =(*particleIterator);

	if(particle) {
		Object *parent = particle->getParent();
		if( parent )
			delete parent, parent=NULL;

		delete particle, particle=NULL;
	}
	return erase(particleIterator);
}

void BoilerFixedParticlesManager::update(float deltaTime) {
	iterator it = begin();

	while(it!=end()) {
		(*it)->update(deltaTime); // Here inside is update its parent too
		++it;
	}
}


void BoilerFixedParticlesManager::cleanUp()
{
	while(!empty()) {
		ParticleEmitter * particle = back();
		
		if(particle) {
			Object *parent = particle->getParent();
			if( parent )
				delete parent, parent=NULL;

			delete particle, particle=NULL;
		}
		pop_back();
	}
}

void BoilerFixedParticlesManager::reloadShader() {
	iterator it = begin();
	while( it != end() ) {
		(*it)->reloadShader();
		++it;
	}
}
