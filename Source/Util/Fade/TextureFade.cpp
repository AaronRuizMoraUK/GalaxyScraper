#include "TextureFade.h"
#include "Factories/QuadFactory.h"
#include "Factories/EffectFactory.h"
#include "Global/GlobalVariables.h"
#include "Game/Game.h"

const std::string TextureFade::shaderName = "fade.fx";
const std::string TextureFade::techniqueName = "texture_fade_tech";

TextureFade::TextureFade()
: mesh( NULL )
, shader( NULL )
, texture( NULL )
{
	D3DXMatrixIdentity(&identityTransformMatrix);
}

void TextureFade::create( const std::string &textureName ) {
	mesh = QuadFactory::getQuadFactory()->createQuad(1.0f / Game::configOptions.aspectRatio, 1.0f);
	assert( mesh );

	shader = EffectFactory::getEffectFactory()->createEffect(shaderName);
	assert( shader );

	texture = TextureFactory::getTextureFactory()->createTexture( textureName );
	assert( texture );
}

void TextureFade::render( float alpha ) {
	assert( mesh && shader && texture );

	Global::world.update(identityTransformMatrix);

	// Calculate World View Projection matrix
	// We suppose that the orthogonal camera is activated
	const D3DXMATRIX &matView = Global::camera->getMatView();
	const D3DXMATRIX &matProj = Global::camera->getMatProj();
	D3DXMATRIX matWorlViewProj;
	D3DXMatrixMultiply(&matWorlViewProj, &matView, &matProj);
	D3DXMatrixMultiply(&matWorlViewProj, &(Global::world.getMatWorld()), &matWorlViewProj);

	D3DXHANDLE handleParam = NULL;

	// Update matrix wvp of shader
	handleParam = shader->GetParameterByName( NULL, "world_view_proj" );
	if( handleParam )
		shader->SetMatrix( handleParam, &matWorlViewProj );

	// Set the color and alpha value
	handleParam = shader->GetParameterByName( NULL, "color_alpha" );
	if( handleParam ) {
		D3DXVECTOR4 colorAlpha( 0.0f, 0.0f, 0.0f, alpha);
		shader->SetVector( handleParam, &colorAlpha);
	}

	// Activate diffuse texture
	handleParam = shader->GetParameterByName( NULL, "diffuse_texture" );
	if( handleParam ) 
		shader->SetTexture( handleParam, texture );

	// Activate technique
	handleParam = shader->GetTechniqueByName( techniqueName.c_str() );
	if( handleParam ) {
		HRESULT hr = shader->SetTechnique( handleParam );
		assert( hr == D3D_OK );
	}

	UINT cPasses, iPass;
	shader->Begin( &cPasses, 0 ); // How many passes has the technique?
	for( iPass = 0; iPass < cPasses; ++iPass ) { // For each pass
		shader->BeginPass( iPass );	// Begin pass
	
		// Do the real rendering of geometry
		mesh->renderStrip(0);

		shader->EndPass( );	// End Pass
	}
	shader->End( );
}

void TextureFade::reloadShader() {
	if(mesh != NULL) {
		shader = EffectFactory::getEffectFactory()->reloadEffect( shaderName );
		assert(shader);
	}
}
