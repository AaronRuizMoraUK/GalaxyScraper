#ifndef ISTRING_RENDER_H
#define ISTRING_RENDER_H

#include "Directx/Directx.h"
#include <string>

class IMesh;

/**
* This class make possible print strings with
* letters and numbers coming from a texture
*
* (0,0) is left-bottom. (1,1) is rigth-top.
*/

class IStringRender
{
protected:
	D3DXMATRIX identityTransformMatrix;

	const IMesh *mesh;

	Texture texture;

	Effect shader;
	const static std::string shaderName;

public:
	IStringRender( );
	virtual ~IStringRender( );

	void create( );

	virtual void print( float x, float y, float letter_width, float letter_height, const char *msg, ... ) = 0;

	void reloadShader();

protected:
	virtual const std::string & getTextureName() const = 0;
	virtual const std::string & getTechnicName() const = 0;
};

#endif // ISTRING_RENDER_H
