#include "PadButtonsRender.h"
#include "Global/GlobalFunctions.h"
#include "Global/GlobalVariables.h"
#include "Source/Game.h"
#include <cassert>

const std::string PadButtonsRender::textureName = "pad_buttons.dds";
const std::string PadButtonsRender::techniqueName = "basic_tech";

const D3DXVECTOR2 PadButtonsRender::letterTexture(64.0f/256.0f, 64.0f/256.0f);

PadButtonsRender::PadButtonsRender( )
: IStringRender( )
{
}

PadButtonsRender::~PadButtonsRender( ) {
	OutputDebugString("Destroying PadButtonsRender...\n");
}

void PadButtonsRender::print( float x, float y, float letter_width, float letter_height, const char *msg, ... ) {
	char txt[512];
	va_list ap;
	va_start(ap, msg);
	vsnprintf_s(txt, sizeof( txt ), 511, msg, ap);
	va_end (ap);

	assert( mesh && shader && texture );
	assert( letter_width>=0 && letter_height>=0 );

	int txtLen = strlen( txt );
	if( txtLen == 0 )
		return;
	else if( txtLen > 2 ) {
		assert( !"Wrong print in PadButtonsRender" );
		return;
	}

	D3DXHANDLE handleParam = NULL;

	// Set texture coordinates according to the letter
	int number = atoi(txt);
	D3DXVECTOR2 textureCoordinates;
	bool found = obtainTextureCoodinatesOf( number, textureCoordinates );
	if( !found )
		return;
	handleParam = shader->GetParameterByName( NULL, "texture_coords" );
	if( handleParam )
		shader->SetFloatArray( handleParam, textureCoordinates, 2 );

	Global::normalize( x, x, -1.0f, 1.0f, 0.0f, 1.0f );
	Global::normalize( y, y, -1.0f, 1.0f, 0.0f, 1.0f );
	letter_height *= letterTexture.y/letterTexture.x * Game::configOptions.aspectRatio; // For having the aspect of the letter

	// Set the width and the height of a letter in screen
	handleParam = shader->GetParameterByName( NULL, "letter_screen_width" );
	if( handleParam )
		shader->SetFloat( handleParam, letter_width );

	handleParam = shader->GetParameterByName( NULL, "letter_screen_height" );
	if( handleParam )
		shader->SetFloat( handleParam, letter_height );

	// Set the width and the height of a letter in the texture
	handleParam = shader->GetParameterByName( NULL, "letter_texture" );
	if( handleParam )
		shader->SetFloatArray( handleParam, letterTexture, 2 );

	// Calculate World View Projection matrix
	// We suppose that the orthogonal camera is activated
	const D3DXMATRIX &matView = Global::camera->getMatView();
	const D3DXMATRIX &matProj = Global::camera->getMatProj();
	D3DXMATRIX matViewProj;
	D3DXMatrixMultiply(&matViewProj, &matView, &matProj);

	// Go to the right to write the next letter
	D3DXMATRIX transformMatrix = identityTransformMatrix;
	transformMatrix(3,0) = x;
	transformMatrix(3,1) = y;
	Global::world.update(transformMatrix);
	D3DXMATRIX matWorlViewProj;
	D3DXMatrixMultiply(&matWorlViewProj, &(Global::world.getMatWorld()), &matViewProj);

	// Update matrix wvp of shader
	handleParam = shader->GetParameterByName( NULL, "world_view_proj" );
	if( handleParam )
		shader->SetMatrix( handleParam, &matWorlViewProj );

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

bool PadButtonsRender::obtainTextureCoodinatesOf( int number, D3DXVECTOR2 &outTextureCoordinates ) const {
	
	if( number > 15 || number < 0 )
		return false;

	int row = number/4;
	int column = number%4;
	outTextureCoordinates = D3DXVECTOR2( column*letterTexture.x, row*letterTexture.y );

	return true;
}
