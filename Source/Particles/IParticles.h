#ifndef IPARTICLES_H
#define IPARTICLES_H

#include <Windows.h>
#include "Material/Material.h"
#include <string>

#include "Papi/pAPI.h"

class Object;
class IMesh;

class IParticles
{
public:
	const static int MAX_PARTICLES = 50;

	static PAPI::ParticleContext_t particleContext;

protected:
	int groupId;

	Object *parent;
	bool isParentAnimated;

	int particleCurrentCount;
	int numQuads;

	IMesh *mesh;
	Material *material;

	D3DXMATRIX identity;

public:
	IParticles()
		: groupId(0)
		, parent(NULL)
		, isParentAnimated(false)
		, particleCurrentCount(0)
		, numQuads(0)
		, mesh(NULL)
		, material(NULL)
	{
		D3DXMatrixIdentity(&identity);
	};

	virtual ~IParticles() {
		OutputDebugString("Destroying IParticles...\n");
		if (material)
			delete material, material = NULL;
		// IParticle does not destroy its Mesh, ParticleFactory does!!
		// Parent musn't be deleted here
	};

	virtual void generateSource() = 0;

	virtual void update( float deltaTime=(1.0f/60.0f) ) = 0; // compute
	virtual const std::string & getType() const = 0;
	virtual const std::string & getTextureName() const = 0;

	void setMesh(IMesh *pMesh) {mesh = pMesh; };
	void setParent(Object *pParent, bool isAnimated=false);
	void setNumQuads(int pNumQuads) {
		assert( pNumQuads > 0 && pNumQuads <= MAX_PARTICLES );
		numQuads = pNumQuads;
	};

	Object * getParent() const {return parent;};
	int getNumQuads() const {return numQuads; };

	const Material * getMaterial() const {
		return material;
	};

	void SETMATERIAL (const std::string &texture, const std::string &shader, const std::string &technique){
		if (material)
			delete material, material = NULL;

		material = new Material(texture, shader, technique);
	};

	bool isEmpty() const { return particleCurrentCount < 1; };

	void render();

	void reloadShader();

protected:
	void activateGroup();

};

/*
class AssociatedParticles : public std::vector<IParticles *>
{
public:

	AssociatedParticles() 
	{
		// something
	};

	~AssociatedParticles(void)
	{
		OutputDebugString("Destroying AssociatedParticles...\n");
	};

	void update(float deltaTime=(1.0f/60.0f) )
	{
		const_iterator it = begin();
		while( it != end() )
		{
			(*it)->update( deltaTime );
			++it;
		}
	}

	void render() 
	{
		const_iterator it = begin();
		while( it != end() )
		{
			(*it)->render();
			++it;
		}
	}

	void clear(void)
	{
		while( !empty() )	
		{
			IParticles *iParticles = (*this)[size()-1];
			if( iParticles )
				delete iParticles, iParticles = NULL;
			pop_back();
		}
	};
};

#include "Object/Object.h"
*/

#endif //IPARTICLES_H
