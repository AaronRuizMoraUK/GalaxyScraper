#ifndef TEXTURE_FADE_H
#define TEXTURE_FADE_H

#include "Mesh/IMesh.h"

#include <string>

/**
* Class to make fades with texture.
* It is made to work with an orthogonal camera.
*/

class TextureFade
{
protected:
	D3DXMATRIX identityTransformMatrix;

	const IMesh *mesh;

	Effect shader;
	const static std::string shaderName;
	const static std::string techniqueName;

	Texture texture;

public:
	TextureFade();

	virtual ~TextureFade() {
		OutputDebugString("Destroying TextureFade...\n");

		// Object does not destroy its Mesh, MeshFactory does!!

		// EffectFactory is who destrois the effect (shader)!!

		// TextureFactory is who destrois the texture!!
	};

	void create( const std::string &textureName );

	void render( float alpha ) ;

	void reloadShader( );
};

#endif //TEXTURE_FADE_H
