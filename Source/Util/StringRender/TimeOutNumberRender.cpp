#include "TimeOutNumberRender.h"
#include "Global/GlobalFunctions.h"
#include "Global/GlobalVariables.h"
#include "Source/Game.h"
#include <cassert>

const std::string TimeOutNumberRender::textureName = "timeout_numbers.dds";
const std::string TimeOutNumberRender::techniqueName = "timeout_tech";

const D3DXVECTOR2 TimeOutNumberRender::letterTexture(64.0f/512.0f, 128.0f/512.0f);

TimeOutNumberRender::TimeOutNumberRender( )
: IStringRender( )
, lastLetter("")
, show_time( 0.0f )
, last_global_time( 0.0f )
{
}

TimeOutNumberRender::~TimeOutNumberRender( ) {
	OutputDebugString("Destroying TimeOutNumberRender...\n");
}

void TimeOutNumberRender::print( float x, float y, float letter_width, float letter_height, const char *msg, ... ) {
	char txt[512];
	va_list ap;
	va_start(ap, msg);
	vsnprintf_s(txt, sizeof( txt ), 511, msg, ap);
	va_end (ap);

	assert( mesh && shader && texture );
	assert( letter_width>=0 && letter_height>=0 );

	if( strlen( txt ) == 0 )
		return;

	errno_t err;
	err = _strlwr_s(txt, sizeof(txt)); // Lower-case
	assert( err == 0 );

	Global::normalize( x, x, -1.0f, 1.0f, 0.0f, 1.0f );
	Global::normalize( y, y, -1.0f, 1.0f, 0.0f, 1.0f );
	letter_height *= letterTexture.y/letterTexture.x * Game::configOptions.aspectRatio; // For having the aspect of the letter

	// Calculate World View Projection matrix
	// We suppose that the orthogonal camera is activated
	const D3DXMATRIX &matView = Global::camera->getMatView();
	const D3DXMATRIX &matProj = Global::camera->getMatProj();
	D3DXMATRIX matViewProj;
	D3DXMatrixMultiply(&matViewProj, &matView, &matProj);

	D3DXHANDLE handleParam = NULL;

	// Set the width and the height of a letter in screen
	handleParam = shader->GetParameterByName( NULL, "letter_screen_width" );
	if( handleParam )
		shader->SetFloat( handleParam, letter_width );

	handleParam = shader->GetParameterByName( NULL, "letter_screen_height" );
	if( handleParam )
		shader->SetFloat( handleParam, letter_height );

	// Set time
	handleParam = shader->GetParameterByName( NULL, "time" );
	if( handleParam )
		shader->SetFloat( handleParam, Global::time );

	// Set the width and the height of a letter in the texture
	handleParam = shader->GetParameterByName( NULL, "letter_texture" );
	if( handleParam )
		shader->SetFloatArray( handleParam, letterTexture, 2 );

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

		// Print each letter in the text
		int next_line = 0;
		int next_letter = 0;
		for( size_t i=0; i<strlen( txt ); ++i ) {

			// Set texture coordinates according to the letter
			if( txt[i] == '\n' ) {
				next_line++;
				next_letter = 0;
				continue;
			}
			else {
				D3DXVECTOR2 textureCoordinates;
				bool found = obtainTextureCoodinatesOf( txt[i], textureCoordinates );
				if( !found )
					continue;
				handleParam = shader->GetParameterByName( NULL, "texture_coords" );
				if( handleParam )
					shader->SetFloatArray( handleParam, textureCoordinates, 2 );
			}

			// Go to the right to write the next letter
			D3DXMATRIX transformMatrix = identityTransformMatrix;
			transformMatrix(3,0) = x + letter_width*next_letter;
			transformMatrix(3,1) = y - letter_height*next_line;
			Global::world.update(transformMatrix);
			D3DXMATRIX matWorlViewProj;
			D3DXMatrixMultiply(&matWorlViewProj, &(Global::world.getMatWorld()), &matViewProj);

			// Update matrix wvp of shader
			handleParam = shader->GetParameterByName( NULL, "world_view_proj" );
			if( handleParam )
				shader->SetMatrix( handleParam, &matWorlViewProj );

			// Only call CommitChanges if any state changes have happened
			// after BeginPass is called
			shader->CommitChanges();

			// Do the real rendering of geometry
			mesh->renderStrip(0);

			++next_letter;
		}

		shader->EndPass( );	// End Pass
	}
	shader->End( );
}

void TimeOutNumberRender::printClosing( float x, float y, float letter_width, float letter_height, const char *msg, ... ) {
	char txt[512];
	va_list ap;
	va_start(ap, msg);
	vsnprintf_s(txt, sizeof( txt ), 511, msg, ap);
	va_end (ap);

	assert( mesh && shader && texture );
	assert( letter_width>=0 && letter_height>=0 );

	if( strlen( txt ) == 0 )
		return;

	errno_t err;
	err = _strlwr_s(txt, sizeof(txt)); // Lower-case
	assert( err == 0 );

	Global::normalize( x, x, -1.0f, 1.0f, 0.0f, 1.0f );
	Global::normalize( y, y, -1.0f, 1.0f, 0.0f, 1.0f );
	letter_height *= letterTexture.y/letterTexture.x * Game::configOptions.aspectRatio; // For having the aspect of the letter

	// Calculate World View Projection matrix
	// We suppose that the orthogonal camera is activated
	const D3DXMATRIX &matView = Global::camera->getMatView();
	const D3DXMATRIX &matProj = Global::camera->getMatProj();
	D3DXMATRIX matViewProj;
	D3DXMatrixMultiply(&matViewProj, &matView, &matProj);

	D3DXHANDLE handleParam = NULL;

	// Show time
	if( strcmp( lastLetter.c_str(), txt ) != 0 )
		show_time = 0.0f;
	else
		show_time += 3.0f * (Global::time - last_global_time);
	lastLetter = txt;
	last_global_time = Global::time;
	show_time = Global::clamp(show_time, 0.0f, 1.0f);

	// Set the width and the height of a letter in screen
	handleParam = shader->GetParameterByName( NULL, "letter_screen_width" );
	if( handleParam )
		shader->SetFloat( handleParam, letter_width * show_time );

	handleParam = shader->GetParameterByName( NULL, "letter_screen_height" );
	if( handleParam )
		shader->SetFloat( handleParam, letter_height * show_time );

	// Set time
	handleParam = shader->GetParameterByName( NULL, "time" );
	if( handleParam )
		shader->SetFloat( handleParam, Global::time );

	// Set the width and the height of a letter in the texture
	handleParam = shader->GetParameterByName( NULL, "letter_texture" );
	if( handleParam )
		shader->SetFloatArray( handleParam, letterTexture, 2 );

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

		// Print each letter in the text
		int next_line = 0;
		int next_letter = 0;
		for( size_t i=0; i<strlen( txt ); ++i ) {

			// Set texture coordinates according to the letter
			if( txt[i] == '\n' ) {
				next_line++;
				next_letter = 0;
				continue;
			}
			else {
				D3DXVECTOR2 textureCoordinates;
				bool found = obtainTextureCoodinatesOf( txt[i], textureCoordinates );
				if( !found )
					continue;
				handleParam = shader->GetParameterByName( NULL, "texture_coords" );
				if( handleParam )
					shader->SetFloatArray( handleParam, textureCoordinates, 2 );
			}

			// Go to the right to write the next letter
			D3DXMATRIX transformMatrix = identityTransformMatrix;
			transformMatrix(3,0) = x + letter_width*next_letter;
			transformMatrix(3,1) = y - letter_height*next_line;
			Global::world.update(transformMatrix);
			D3DXMATRIX matWorlViewProj;
			D3DXMatrixMultiply(&matWorlViewProj, &(Global::world.getMatWorld()), &matViewProj);

			// Update matrix wvp of shader
			handleParam = shader->GetParameterByName( NULL, "world_view_proj" );
			if( handleParam )
				shader->SetMatrix( handleParam, &matWorlViewProj );

			// Only call CommitChanges if any state changes have happened
			// after BeginPass is called
			shader->CommitChanges();

			// Do the real rendering of geometry
			mesh->renderStrip(0);

			++next_letter;
		}

		shader->EndPass( );	// End Pass
	}
	shader->End( );
}

bool TimeOutNumberRender::obtainTextureCoodinatesOf( const char letter, D3DXVECTOR2 &outTextureCoordinates ) const {
	
	if( letter == '0' )
		outTextureCoordinates = D3DXVECTOR2( 0.0f, 0.0f );
	else if( letter == '1' )
		outTextureCoordinates = D3DXVECTOR2( letterTexture.x, 0.0f );
	else if( letter == '2' )
		outTextureCoordinates = D3DXVECTOR2( 2.0f*letterTexture.x , 0.0f );
	else if( letter == '3' )
		outTextureCoordinates = D3DXVECTOR2( 3.0f*letterTexture.x, 0.0f );
	else if( letter == '4' )
		outTextureCoordinates = D3DXVECTOR2( 4.0f*letterTexture.x, 0.0f );
	else if( letter == '5' )
		outTextureCoordinates = D3DXVECTOR2( 5.0f*letterTexture.x, 0.0f );
	else if( letter == '6' )
		outTextureCoordinates = D3DXVECTOR2( 6.0f*letterTexture.x, 0.0f );
	else if( letter == '7' )
		outTextureCoordinates = D3DXVECTOR2( 7.0f*letterTexture.x, 0.0f );
	else if( letter == '8' )
		outTextureCoordinates = D3DXVECTOR2( 0.0f, letterTexture.y);
	else if( letter == '9' )
		outTextureCoordinates = D3DXVECTOR2( letterTexture.x, letterTexture.y);
	else if( letter == ' ' )
		outTextureCoordinates = D3DXVECTOR2( 2.0f*letterTexture.x, letterTexture.y);
	else if( letter == 'g' )
		outTextureCoordinates = D3DXVECTOR2( 3.0f*letterTexture.x, letterTexture.y);
	else if( letter == 'o' )
		outTextureCoordinates = D3DXVECTOR2( 4.0f*letterTexture.x, letterTexture.y);
	else if( letter == 't' )
		outTextureCoordinates = D3DXVECTOR2( 5.0f*letterTexture.x, letterTexture.y);
	else if( letter == 'a' )
		outTextureCoordinates = D3DXVECTOR2( 6.0f*letterTexture.x, letterTexture.y);
	else if( letter == 's' )
		outTextureCoordinates = D3DXVECTOR2( 7.0f*letterTexture.x, letterTexture.y);
	else if( letter == 'r' )
		outTextureCoordinates = D3DXVECTOR2( 0.0f, 2.0f*letterTexture.y);
	else if( letter == 'u' )
		outTextureCoordinates = D3DXVECTOR2( letterTexture.x, 2.0f*letterTexture.y);
	else if( letter == 'n' )
		outTextureCoordinates = D3DXVECTOR2( 2.0f*letterTexture.x, 2.0f*letterTexture.y);
	else if( letter == '!' )
		outTextureCoordinates = D3DXVECTOR2( 3.0f*letterTexture.x, 2.0f*letterTexture.y);
	else if( letter == 'h' )
		outTextureCoordinates = D3DXVECTOR2( 4.0f*letterTexture.x, 2.0f*letterTexture.y);
	else if( letter == 'e' )
		outTextureCoordinates = D3DXVECTOR2( 5.0f*letterTexture.x, 2.0f*letterTexture.y);
	else
		return false;

	return true;
}
