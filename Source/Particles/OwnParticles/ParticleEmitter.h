#ifndef PARTICLE_EMITTER_H
#define PARTICLE_EMITTER_H

#include <Windows.h>
#include "Material/Material.h"
#include <string>

#include "Papi/pAPI.h"
#include "Particle.h"

class Object;
class IMesh;


class ParticleEmitter
{
public:
	const static int MAX_PARTICLES = 50;

	bool isParentAnimated;
	bool isParentPivot;

	int particleCurrentCount;
	int numQuads;

	IMesh *mesh;
	Material *material;

	D3DXMATRIX identity;

	bool isLocal;
	
public:

	ParticleEmitter(int pNumQuads);

	~ParticleEmitter();

	virtual void generate() = 0;

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

	bool isEmpty() const { return numQuads < 1; };

	bool getIsParentPivot() const {return isParentPivot;};
	void setIsParentPivot(bool isPivot) {isParentPivot = isPivot;};

	void render();

	void reloadShader();

	const D3DXVECTOR3 &getParticlePosition(int index) const {return particles.getPosition(index);};
	float getSize(int index) const {return particles.getSize(index);};
	const D3DXVECTOR4 &getColor(int index) const {return particles.getColor(index);};

protected:
	Particles particles;
	Object *parent;
};

#endif //PARTICLE_EMITTER_H
