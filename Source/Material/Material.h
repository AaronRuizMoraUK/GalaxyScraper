#ifndef MATERIAL_H
#define MATERIAL_H


#include "Directx/Directx.h"
#include "cal3d/cal3d.h"
#include <vector>
#include <string>
#include "Factories/TextureFactory.h"

typedef struct ShadowInformationStruct ShadowInformation;

typedef struct AmbientColorStruct {
	bool ambientColorEnable;
	DWORD color;
	float ambientColorAmount;
} AmbientColor;

/**
* Texture map struct
*/

typedef struct TextureMapStruct {
	bool mapEnable;
	Texture texture;
	Texture normalMap;
	std::string mapFilename;
	float mapAmount;
	bool hasAplha;
	bool ownCoordinates;

	TextureMapStruct()
		: mapEnable(false)
		, texture(NULL)
		, normalMap(NULL)
		, mapFilename("")
		, mapAmount(0.0f)
		, hasAplha(false)
		, ownCoordinates(false)
	{
	};

} TextureMap;

/**
* Texture map vector
*/

class ParticleEmitter;

class TextureMapVector : public std::vector<TextureMap *>
{
public:
	// Default constructor
	TextureMapVector() {};

	// Override copy constructor
	TextureMapVector( const TextureMapVector &source ) {
		copy(source);
	};

	// Destructor
	~TextureMapVector() {
		OutputDebugString("Destroying TextureMapVector...\n");
		clear();
	};

	// Override Operator '='
	void operator=(const TextureMapVector &source) {
		clear();
		copy(source);
	};

	// Override clear function
	void clear() {
		while( !empty() ) {
			TextureMap * textureMap = (*this)[size()-1];
			// TextureFactory is who destroies the texture!!
			if(textureMap)
				delete textureMap, textureMap=NULL;
			pop_back();
		}
	};

private:
	// Copy function
	void copy(const TextureMapVector &source) {
		for(unsigned int i=0; i<source.size(); ++i) {
			TextureMap *textureMap = new TextureMap( *(source[i]) );
			push_back(textureMap);
		}
	};
};

/**
* Material for object. This has the texture and shader.
*/

class Material
{
protected:
	AmbientColor ambientColor;
	TextureMapVector textureMapVector;
	Effect shader;
	std::string shaderName;
	std::string techniqueName;

	// Additional color for checking behaviours
	AmbientColor additionalColor;

	D3DXMATRIX matWorlViewProj;
	
public:

	~Material(){
		OutputDebugString("Destroying Material...\n");

		// Automaticaly texture map vector destroies itself

		// EffectFactory is who destrois the effect (shader)!!
	};

	// VERY VERY VERY OPTIONAL!!!
	Material(const std::string &filename, const std::string &pShaderName, const std::string &pTechniqueName);
	Material(Texture texture, const std::string &pShaderName, const std::string &pTechniqueName);

	void activate() const;
	void activateCal(const CalModel *calModel) const;
	void activateParticle( const ParticleEmitter &particle ) const;
	void activateShadow( const ShadowInformation *shadowInfs, int numShadowInformations ) const;

	const Effect & getShader() const { return shader; };

	// TODO
	void setTechnique(const std::string &technique) {
		assert( technique == "basic_tech" || technique == "normal_tech" );
		techniqueName = technique;

		if( techniqueName == "normal_tech" ) {
			if( textureMapVector[0]->normalMap != NULL )
				return;

			std::string filenameNormal = textureMapVector[0]->mapFilename;
			assert( filenameNormal != "" );
			std::string::size_type pos = filenameNormal.find(".dds");
			filenameNormal.insert(pos, "_normalmap");

			textureMapVector[0]->normalMap = TextureFactory::getTextureFactory()->createTexture(filenameNormal);
			assert( textureMapVector[0]->normalMap != NULL );
		}
	}

	void reloadShader();

	void setMatWorldViewProj(const D3DXMATRIX &pMatWorlViewProj) { 
		matWorlViewProj = pMatWorlViewProj;
	};

	void enableAdditionalColor(DWORD color) {
		additionalColor.ambientColorEnable=true;
		additionalColor.color=color;
	};

	void disableAdditionalColor() {
		additionalColor.ambientColorEnable=false;
	};

};

class MaterialVector : public std::vector<Material *>
{
public:
	// Default constructor
	MaterialVector(){};

	// Override copy constructor
	MaterialVector(const MaterialVector &source) {
		copy(source);
	};

	// Destructor
	~MaterialVector() {
		OutputDebugString("Destroying MaterialVector...\n");
		clear();
	};

	// Override Operator '='
	void operator=(const MaterialVector &source) {
		clear();
		copy(source);
	};

	// Override clear function
	void clear() {
		while( !empty() ) {
			Material * material = (*this)[size()-1];
			if(material)
				delete material, material=NULL;
			pop_back();
		}
	}

private:
	// Copy function
	void copy(const MaterialVector &source) {
		for(unsigned int i=0; i<source.size(); ++i) {
			Material *material = new Material( *(source[i]) );
			push_back(material);
		}
	};

};

#endif //MATERIAL_H
