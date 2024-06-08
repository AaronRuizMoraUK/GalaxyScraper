#include "Fade.h"
#include "Factories/QuadFactory.h"
#include "Factories/EffectFactory.h"
#include "Global/GlobalFunctions.h"
#include "Global/GlobalVariables.h"

const std::string Fade::shaderName = "fade.fx";
const std::string Fade::techniqueName = "fade_tech";

Fade::Fade()
: alpha( -1.0f )
, mesh( NULL )
, shader( NULL )
, duration( 0.0f )
, time( 0.0f )
, initedFadeIn( false )
, initedFadeOut( false )
, initedMediumFadeOut( false )
, initAlphaFadeIn( 0.0f )
, initAlphaFadeOut( 0.0f )
{
	D3DXMatrixIdentity(&identityTransformMatrix);
}

void Fade::create( ) {
	initVariables( );

	mesh = QuadFactory::getQuadFactory()->createQuad(1.0f, 1.0f);
	assert(mesh);

	shader = EffectFactory::getEffectFactory()->createEffect(shaderName);
	assert(shader);
}

void Fade::initVariables( ) {
	alpha = -1.0f;
	duration = 0.0f;
	time = 0.0f;
	initedFadeIn = false;
	initedFadeOut = false;
	initedMediumFadeOut = false;
	initAlphaFadeIn = 0.0f;
	initAlphaFadeOut = 0.0f;
}

void Fade::update( float deltaTime ) { 
	time += deltaTime;
	if( time>duration )
		time = duration;
}

void Fade::renderFadeIn( float pDuration, const D3DXVECTOR3 &color ) {
	if( !initedFadeIn ) {
		initedFadeIn = true;
		initedFadeOut = initedMediumFadeOut = false;

		time = 0.0f;
		duration = pDuration;

		if( alpha == 0.0f ) {
			time = duration;
			return;
		}
		if( alpha == -1.0f )
			alpha = 1.0f;

		initAlphaFadeIn = alpha;
	}

	// Update alpha from initAlphaFadeIn to 0 in duration seconds
	alpha = Global::clamp( initAlphaFadeIn * ((duration-time)/duration), 0.0f, 1.0f );

	if( alpha > 0.0f )
		render( color );
}

void Fade::renderFadeOut( float pDuration, const D3DXVECTOR3 &color ) {
	if( !initedFadeOut ) {
		initedFadeOut = true;
		initedFadeIn = initedMediumFadeOut = false;

		time = 0.0f;
		duration = pDuration;

		if( alpha == 1.0f ) {
			time = duration;
			return;
		}
		else if( alpha == -1.0f )
			alpha = 0.0f;

		initAlphaFadeOut = alpha;
	}

	// Update alpha from initAlphaFadeOut to 1 in duration seconds
	alpha = Global::clamp( initAlphaFadeOut + (1.0f-initAlphaFadeOut) * (time/duration), 0.0f, 1.0f );

	if( alpha > 0.0f )
		render( color );
}

void Fade::renderMediumFadeOut( float pDuration, const D3DXVECTOR3 &color ) {
	if( !initedMediumFadeOut ) {
		initedMediumFadeOut = true;
		initedFadeIn = initedFadeOut = false;

		time = 0.0f;
		duration = pDuration;

		if( alpha == -1.0f )
			alpha = 0.0f;

		assert( alpha == 0.0f );
	}

	// Update alpha from 0 to 0.5 in duration seconds
	alpha = Global::clamp( 0.5f * (time/duration), 0.0f, 1.0f );

	if( alpha > 0.0f )
		render( color );
}

void Fade::render( const D3DXVECTOR3 &color ) const {
	assert( mesh && shader );

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
		D3DXVECTOR4 colorAlpha( color, alpha);
		shader->SetVector( handleParam, &colorAlpha);
	}

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

void Fade::reloadShader() {
	if(mesh != NULL) {
		shader = EffectFactory::getEffectFactory()->reloadEffect( shaderName );
		assert(shader);
	}
}
