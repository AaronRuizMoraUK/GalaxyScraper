#include "IStringRender.h"
#include "Factories/QuadFactory.h"
#include "Factories/TextureFactory.h"
#include "Factories/EffectFactory.h"

const std::string IStringRender::shaderName = "string_render.fx";

IStringRender::IStringRender( )
: mesh( NULL )
, texture( NULL )
, shader( NULL )
{
	D3DXMatrixIdentity( &identityTransformMatrix );
}

IStringRender::~IStringRender( ) {
	OutputDebugString("Destroying IStringRender...\n");
}

void IStringRender::create( ) {
	mesh = QuadFactory::getQuadFactory()->createQuad(0.5f, 0.5f);
	assert( mesh );

	texture = TextureFactory::getTextureFactory()->createTexture( getTextureName() );
	assert( texture );

	shader = EffectFactory::getEffectFactory()->createEffect( shaderName );
	assert( shader );
}

void IStringRender::reloadShader() {
	if(mesh != NULL) {
		shader = EffectFactory::getEffectFactory()->reloadEffect( shaderName );
		assert(shader);
	}
}