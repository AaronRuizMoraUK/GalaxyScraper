#include "IuranusFixedParticlesManager.h"
#include "Particles/DummyParticles/DummyParticles.h"
#include "Particles/BirthPointsParticles/BirthPointsParticles.h"
#include "Particles/MouthParticles/MouthParticles.h"
#include "Particles/AssParticles/AssParticles.h"
#include "Factories/ParticleFactory.h"
#include "Global/GlobalFunctions.h"
#include "Global/GlobalVariables.h"
#include "Object/Object.h"
#include "Object/CollisionableObject/AnimatedObject/AnimatedObject.h"
#include "Managers/DataLocatorManager.h"
#include "Managers/PlanetManager.h"
#include <cassert>

IuranusFixedParticlesManager * IuranusFixedParticlesManager::iuranusFixedParticlesManager = NULL;

IuranusFixedParticlesManager * IuranusFixedParticlesManager::getIuranusFixedParticlesManager() {
	if(iuranusFixedParticlesManager == NULL)
		iuranusFixedParticlesManager = new IuranusFixedParticlesManager();

	return iuranusFixedParticlesManager;
}


IuranusFixedParticlesManager::IuranusFixedParticlesManager()
: currentType("")
, currentParent(NULL)
, currentNumQuads(0)
{
	init();
}

void IuranusFixedParticlesManager::init()
{
	DataLocatorManager *dataLocatorManager = DataLocatorManager::getDataLocatorManager();
	const std::string &pathToXml = dataLocatorManager->getPath("xml");

	std::string xmlFile = pathToXml + "iuranus/iuranus_fixed_particles.xml";
	bool ok = xmlParseFile(xmlFile);
	assert(ok);
}

void IuranusFixedParticlesManager::onEndElement (const std::string &elem)
{
	if( elem == "particle" )
	{
		addParticle(currentType, currentParent, currentNumQuads);

		currentType = "";
		currentParent = NULL;
		currentNumQuads = 0;
	}
}

void IuranusFixedParticlesManager::onStartElement (const std::string &elem, MKeyValue &atts) 
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

		currentParent->setTransformMatrix(matrix);

		D3DXVECTOR3 pos = currentParent->getPosition();
		const D3DXVECTOR3 &localFrontVector = currentParent->getLocalFrontVector();
	
		// Current planet center and radius
		const Planet * planet = PlanetManager::getPlanetManager()->getCurrentPlanet();
		const D3DXVECTOR3 &center = planet->getCenter();
		float radius = planet->getRadius();

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

		// Calculate the final position with height
		// Necesary  to be here because fix the height
		float alpha = 0.0f;
		float delta = 0.0f;
		planet->getPolarAngles(pos, alpha, delta);
		float height = planet->getHeight(alpha, delta);
		float heightOld = planet->getHeight(currentParent->getAlpha(), currentParent->getDelta());
		currentParent->setAlpha(alpha);
		currentParent->setDelta(delta);
		if(height==0.0f)
			height=planet->getRadius();
		D3DXVECTOR3 finalPosition = center + (0.8f*heightOld+0.2f*height)*newLocalUpVector;
		if(finalPosition != pos) { // Set final position if exist differences
			//finalPosition = 0.1f*newPosition+0.9f*finalPosition;
			currentParent->setPosition( finalPosition );
		}

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
		currentParent->setPosition(finalPosition);
	}
}

void IuranusFixedParticlesManager::addParticle(const std::string &type, AnimatedObject *parent, int pNumQuads)
{
	assert( type != "" && parent && pNumQuads>0 );

	ParticleEmitter *p = NULL;

	if(type == BirthPointsParticles::type)
		p = new BirthPointsParticles();
	else if(type == MouthParticles::type)
		p = new MouthParticles();
	else if(type == AssParticles::type)
		p = new AssParticles();
	else {
		assert(!"Undefined particle type");
		return;
	}
	assert( p );

	bool isAnimated = ( parent->getAnimationController() != NULL ) ? true : false;
	p->setParent(parent, isAnimated);
	// p->setNumQuads(pNumQuads);

	IMesh *mesh = ParticleFactory::getParticleFactory()->createParticle(pNumQuads);	
	assert(mesh);
	p->setMesh(mesh);

	p->SETMATERIAL(p->getTextureName(), "shader.fx", "particle_tech");

	p->generate();

	push_back(p);
}


void IuranusFixedParticlesManager::render() const {
	const_iterator it = begin();

	while(it!=end()) {
		// TODO Hacer algo para que las particulas que queden fuera del frustum no se vean
		(*it)->render();
		++it;
	}
}
	
IuranusFixedParticlesManager::iterator IuranusFixedParticlesManager::killParticle(IuranusFixedParticlesManager::iterator &particleIterator)
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

void IuranusFixedParticlesManager::update(float deltaTime) {
	iterator it = begin();

	while(it!=end()) {
		(*it)->update(deltaTime); // Here inside is update its parent too
		++it;
	}
}


void IuranusFixedParticlesManager::cleanUp()
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

void IuranusFixedParticlesManager::reloadShader() {
	iterator it = begin();
	while( it != end() ) {
		(*it)->reloadShader();
		++it;
	}
}
