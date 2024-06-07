#include "Material.h"
#include <cassert>
#include "Global/GlobalFunctions.h"
#include "Global/GlobalVariables.h"
#include "Factories/EffectFactory.h"
#include "Particles/OwnParticles/ParticleEmitter.h"
#include "Factories/MeshFactory.h"
#include "Managers/PlanetManager.h"
#include "Managers/IuranusManagers/IuranusShadowManager.h"

// VERY VERY VERY OPTIONAL!!!
Material::Material(const std::string &filename, const std::string &pShaderName, const std::string &pTechniqueName)
: shader(NULL)
, shaderName(pShaderName)
, techniqueName(pTechniqueName)
{
	D3DXMatrixIdentity(&matWorlViewProj);

	additionalColor.ambientColorEnable=false;
	additionalColor.color=0xFF000000;

	// Create a material --> OPTIONAL
	ambientColor.ambientColorEnable = false;
	ambientColor.color = 0xFF103220;

	// Texture 1
	TextureMap * textureMap = new TextureMap();
	textureMap->mapEnable=true;
	textureMap->mapFilename=filename;

	// Create the texture
	textureMap->texture = TextureFactory::getTextureFactory()->createTexture(filename);
	if(textureMap->texture == NULL)
		return;

	if( techniqueName == "normal_tech" || techniqueName == "skinning_normal_tech" || techniqueName == "lava_normal_tech" ) {
		std::string filenameNormal = filename;
		std::string::size_type pos = filenameNormal.find(".dds");
		filenameNormal.insert(pos, "_normalmap");

		textureMap->normalMap = TextureFactory::getTextureFactory()->createTexture(filenameNormal);
		if(textureMap->normalMap == NULL)
			return;
	}

	textureMap->ownCoordinates=true;
	if(filename=="bush.tga")
		textureMap->hasAplha=true;
	else
		textureMap->hasAplha=false;

	// Include textures in vector
	textureMapVector.push_back(textureMap);

	// Create the Shader TODO OPTIONAL
	shader = EffectFactory::getEffectFactory()->createEffect(shaderName);
	if(shader == NULL)
		return;
}

// VERY VERY VERY OPTIONAL!!!
Material::Material(Texture texture, const std::string &pShaderName, const std::string &pTechniqueName)
: shader(NULL)
, shaderName(pShaderName)
, techniqueName(pTechniqueName)
{
	D3DXMatrixIdentity(&matWorlViewProj);

	additionalColor.ambientColorEnable=false;
	additionalColor.color=0xFF000000;

	// Create a material --> OPTIONAL
	ambientColor.ambientColorEnable = false;
	ambientColor.color = 0xFF103220;

	// Texture 1
	TextureMap * textureMap = new TextureMap();
	textureMap->mapEnable=true;
	textureMap->mapFilename="";

	// Create the texture
	textureMap->texture = texture;
	if(textureMap->texture == NULL)
		return;

	assert( techniqueName != "normal_tech" && techniqueName != "skinning_normal_tech" && techniqueName != "lava_normal_tech" );

	textureMap->ownCoordinates=true;
	textureMap->hasAplha=false;

	// Include textures in vector
	textureMapVector.push_back(textureMap);

	// Create the Shader TODO OPTIONAL
	shader = EffectFactory::getEffectFactory()->createEffect(shaderName);
	if(shader == NULL)
		return;
}

void Material::activateShadow( const ShadowInformation *shadowInfs, int numShadowInformations ) const
{
	D3DXHANDLE handleParam = NULL;

	D3DXHANDLE handle_param = shader->GetParameterByName( NULL, "particle_data" );
	if( handle_param )  {
		shader->SetVectorArray( handle_param, (D3DXVECTOR4 *)shadowInfs, numShadowInformations * 2);
	}

	handleParam = shader->GetParameterByName( NULL, "planet_center" );
	if( handleParam ) {
		const D3DXVECTOR3 &planetCenter = PlanetManager::getPlanetManager()->getCurrentPlanet()->getCenter();
		D3DXVECTOR4 planetCenter4( planetCenter.x, planetCenter.y, planetCenter.z, 0.0f );
		shader->SetVector( handleParam, &planetCenter4);
	}
}

void Material::activateParticle( const ParticleEmitter &particle ) const
{
	D3DXHANDLE handleParam = NULL;

	struct TMatrix2x4 {
		float r00, r01, r02, r03;
		float r04, r05, r06, r07;
	};

//	size_t cnt = IParticles::particleContext.GetGroupCount();
	size_t cnt = particle.getNumQuads();
	
	if(cnt < 1)
		return;

	TMatrix2x4 matrices[ParticleEmitter::MAX_PARTICLES];
	//memset( &matrices[0].r00, 0x00, sizeof(TMatrix2x4)*ParticleEmitter::MAX_PARTICLES );

	assert( cnt <= ParticleEmitter::MAX_PARTICLES ); 

	for (size_t i = 0; i < cnt; i++)
	{
		const D3DXVECTOR3 &pos = particle.getParticlePosition(i);

		D3DXVECTOR3 realPos;

		if (particle.isLocal)
			D3DXVec3TransformCoord( &realPos, &pos , &(particle.getParent()->getTransformMatrix()) );
		else
			realPos = pos;

		float size  = particle.getSize(i);
		const D3DXVECTOR4 &color = particle.getColor(i);
		
		matrices[i].r00 = realPos.x;
		matrices[i].r01 = realPos.y;
		matrices[i].r02 = realPos.z;
		matrices[i].r03 = size;
		matrices[i].r04 = color.x;
		matrices[i].r05 = color.y;
		matrices[i].r06 = color.z;
		matrices[i].r07 = color.w;
	}

	D3DXHANDLE handle_param = shader->GetParameterByName( NULL, "particle_data" );
	if( handle_param ) 
	{
		shader->SetVectorArray( handle_param, (D3DXVECTOR4 *) matrices, cnt * 2);
	}


	handleParam = shader->GetParameterByName( NULL, "camera_up" );
	if( handleParam ) {
		const D3DXVECTOR3 & cameraUp = Global::camera->getLocalUpVector();
		D3DXVECTOR4 cameraUp4( cameraUp.x, cameraUp.y, cameraUp.z, 0.0f );
		shader->SetVector( handleParam, &cameraUp4);
	}

	handleParam = shader->GetParameterByName( NULL, "camera_left" );
	if( handleParam ) {
		const D3DXVECTOR3 & cameraLeft = Global::camera->getLocalLeftVector();
		D3DXVECTOR4 cameraLeft4( -cameraLeft.x, -cameraLeft.y, -cameraLeft.z, 0.0f );
		shader->SetVector( handleParam, &cameraLeft4);
	}
}

void Material::activateCal(const CalModel *calModel) const
{
	D3DXHANDLE handleParam = NULL;

	struct TMatrix3x4 {
		float r00, r01, r02, tx;
		float r10, r11, r12, ty;
		float r20, r21, r22, tz;
	};

	TMatrix3x4 matrices[ MeshFactory::MAX_BONES ];
	//memset( &matrices[0].r00, 0x00, sizeof(TMatrix3x4)*MeshFactory::MAX_BONES );

	typedef std::vector< CalBone * > VBones;
	const VBones &bones = calModel->getSkeleton( )->getVectorBone( );
	assert( bones.size( ) <= MeshFactory::MAX_BONES );

	for( size_t i=0; i<bones.size( ); ++i ) {
		const CalMatrix &mtx = bones[ i ]->getTransformMatrix( );
		const CalVector &trans = bones[ i ]->getTranslationBoneSpace( );
		// Convertir la matrix de cal a una de 3x4 traspuesta
		matrices[ i ].r00 = mtx.dxdx;
		matrices[ i ].r01 = mtx.dxdy;
		matrices[ i ].r02 = mtx.dxdz;
		matrices[ i ].tx  = trans.x;

		matrices[ i ].r10 = mtx.dydx;
		matrices[ i ].r11 = mtx.dydy;
		matrices[ i ].r12 = mtx.dydz;
		matrices[ i ].ty  = trans.y;

		matrices[ i ].r20 = mtx.dzdx;
		matrices[ i ].r21 = mtx.dzdy;
		matrices[ i ].r22 = mtx.dzdz;
		matrices[ i ].tz  = trans.z;
	}

	handleParam = shader->GetParameterByName( NULL, "Bones" );
	if( handleParam ) 
		shader->SetFloatArray( handleParam, &matrices[ 0 ].r00, bones.size() * 12 );
}

void Material::activate() const {

	D3DXHANDLE handleParam = NULL;

#ifdef DEBUG
	// Status Color DEBUG
	handleParam = shader->GetParameterByName( NULL, "status_color" );
	if( handleParam ) {
		if( additionalColor.ambientColorEnable ) {
			D3DXVECTOR4 statusColor;
			Global::formatColorDWordToVector4(statusColor, additionalColor.color);
			shader->SetVector( handleParam, &statusColor );
		}
		else
			shader->SetVector( handleParam, &D3DXVECTOR4(0,0,0,1) );
	}
#endif //DEBUG

	// Update matrix wvp of shader
	handleParam = shader->GetParameterByName( NULL, "world_view_proj" );
	if( handleParam )
		shader->SetMatrix( handleParam, &matWorlViewProj );

	handleParam = shader->GetParameterByName( NULL, "world" );
	if( handleParam )
		shader->SetMatrix( handleParam, &(Global::world.getMatWorld()) );

	// Activate diffuse texture
	handleParam = shader->GetParameterByName( NULL, "diffuse_texture" );
	if( handleParam ) 
		shader->SetTexture( handleParam, textureMapVector[0]->texture );

	if( techniqueName == "quad_tech" || techniqueName == "title_tech" ||
		techniqueName == "lava_normal_tech" || techniqueName == "render_texture_tech"
		|| techniqueName == "godray_tech") {
		// Set the time
		handleParam = shader->GetParameterByName( NULL, "time" );
		if( handleParam ) {
			shader->SetFloat( handleParam, Global::time);
		}
	}

	if ( techniqueName == "basic_tech" 
		|| techniqueName == "normal_tech" 
		|| techniqueName == "skinning_normal_tech" 
		|| techniqueName == "lava_normal_tech" 
		|| techniqueName == "skinning_tech"
		|| techniqueName == "godray_tech" ) {
		// Set the light position
		handleParam = shader->GetParameterByName( NULL, "light_pos" );
		if( handleParam ) {
			D3DXVECTOR4 lightPos4( Global::lightPosition.x, Global::lightPosition.y, Global::lightPosition.z, 1.0f );
			shader->SetVector( handleParam, &lightPos4);
		}

		// Set the camera position
		handleParam = shader->GetParameterByName( NULL, "camera_pos" );
		if( handleParam ) {
			const D3DXVECTOR3 & cameraPosition = Global::camera->getPosition();
			D3DXVECTOR4 cameraPos4( cameraPosition.x, cameraPosition.y, cameraPosition.z, 1.0f );
			shader->SetVector( handleParam, &cameraPos4);
		}

		if(techniqueName == "normal_tech" || techniqueName == "skinning_normal_tech" || techniqueName == "lava_normal_tech") {
			// Activate normal map texture
			// TODO Look for the normal map in the Factory
			//std::string normalTextureFilename = "iuranus_surface_normalmap.dds";
			//Texture normalTexture = TextureFactory::getTextureFactory()->createTexture(normalTextureFilename);
			handleParam = shader->GetParameterByName( NULL, "normal_texture" );
			if( handleParam ) 
				shader->SetTexture( handleParam, textureMapVector[0]->normalMap );
		}


	}

	// Activate technique
	handleParam = shader->GetTechniqueByName( techniqueName.c_str() );
	if( handleParam ) {
		HRESULT hr = shader->SetTechnique( handleParam );
		assert( hr == D3D_OK );
	}
}

void Material::reloadShader() {
	shader = EffectFactory::getEffectFactory()->reloadEffect( shaderName );
	assert(shader);
}

/*Global::device->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_DISABLE );
Global::device->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_DISABLE );
Global::device->SetTextureStageState( 2, D3DTSS_COLOROP, D3DTOP_DISABLE );
Global::device->SetTexture( 0, NULL );
Global::device->SetTexture( 1, NULL );

// Draw transparent thing
Global::world.initiate();
Global::device->SetTexture( 0, texture3 );
Global::device->SetTexture( 1, texture4 );
// Para definir las operaciones que se hacen con las texturas
// Stage -> etapas de textura
// Modulate == multiplicar
// Para definir la operacion de Color (ColorOP)
Global::device->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE ); // Operacion para etapa 0 multiplicar
Global::device->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE ); // Primer argumento es la textura de la etapa 0
Global::device->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE ); // Segundo argumento es el color del vertice
// Operacion de alpha, disable, no hacer nada
Global::device->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_DISABLE );

Global::device->SetTextureStageState( 1, D3DTSS_TEXCOORDINDEX, 0 );
Global::device->SetTextureStageState( 1, D3DTSS_COLOROP, D3DTOP_MODULATE );
Global::device->SetTextureStageState( 1, D3DTSS_COLORARG1, D3DTA_CURRENT ); // Primer argumento es la textura de la etapa 0
Global::device->SetTextureStageState( 1, D3DTSS_COLORARG2, D3DTA_TEXTURE ); // Segundo argumento es el color del vertice
//Global::device->SetTextureStageState( 1, D3DTSS_ALPHAOP,   D3DTOP_DISABLE );
Global::device->SetTextureStageState( 1, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1 );
Global::device->SetTextureStageState( 1, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );

//Global::device->SetRenderState(D3DRS_TEXTUREFACTOR, 0xFF0000FF);  //-> TFACTOR

//Global::device->SetTextureStageState( 2, D3DTSS_TEXCOORDINDEX, 1 );
//Global::device->SetTextureStageState( 2, D3DTSS_COLOROP, D3DTOP_ADD );
//Global::device->SetTextureStageState( 2, D3DTSS_COLORARG1, D3DTA_CURRENT ); // Primer argumento es la textura de la etapa 0
//Global::device->SetTextureStageState( 2, D3DTSS_COLORARG2, D3DTA_TFACTOR ); // Segundo argumento es el color del vertice
//Global::device->SetTextureStageState( 2, D3DTSS_ALPHAOP,   D3DTOP_DISABLE );

Global::device->SetVertexDeclaration( CUSTOMVERTEX_TEXTURE2::getVertexDeclaration() );
Global::device->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, quad, sizeof(CUSTOMVERTEX_TEXTURE2));

Global::device->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_DISABLE );
Global::device->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_DISABLE );
//Global::device->SetTextureStageState( 2, D3DTSS_COLOROP,   D3DTOP_DISABLE );
Global::device->SetTextureStageState( 1, D3DTSS_TEXCOORDINDEX, 1  );
//Global::device->SetTextureStageState( 2, D3DTSS_TEXCOORDINDEX, 2  );
Global::device->SetTexture( 0, NULL );
Global::device->SetTexture( 1, NULL );*/

/*Material::Material()
{
//additionalColor.ambientColorEnable=false;
//additionalColor.color=0xFF000000;

// Create a material --> OPTIONAL
ambientColor.ambientColorEnable = true;
ambientColor.color = 0xFF000042;

// Texture 1
TextureMap * textureMap = new TextureMap();
textureMap->mapEnable=true;
//strcpy_s(textureMap->mapFilename, sizeof(textureMap->mapFilename), "./data/ELMLEAF.TGA");
textureMap->mapFilename="./data/ELMLEAF.TGA";

// Use D3DX to create a texture from a file based image
if( FAILED( D3DXCreateTextureFromFile( Global::device,
textureMap->mapFilename.c_str(), &(textureMap->texture) ) ) )
{
char msg[100];
strcpy_s(msg, sizeof(msg), "Could not find texture ");
strcat_s(msg, sizeof(msg), textureMap->mapFilename.c_str());
MessageBox(NULL, msg, "Demo.exe", MB_OK);
return;
}
textureMap->ownCoordinates=true;
textureMap->hasAplha=false;

// Texture 2
TextureMap * textureMap2 = new TextureMap();
textureMap2->mapEnable=true;
textureMap2->mapFilename="./data/green.tga";

// Use D3DX to create a texture from a file based image
if( FAILED( D3DXCreateTextureFromFile( Global::device,
textureMap2->mapFilename.c_str(), &(textureMap2->texture) ) ) )
{
char msg[100];
strcpy(msg, "Could not find texture ");
strcat(msg, textureMap2->mapFilename.c_str());
MessageBox(NULL, msg, "Demo.exe", MB_OK);
return;
}
textureMap2->ownCoordinates=false;
textureMap2->hasAplha=false;

// Include textures in vector
textureMapVector.push_back(textureMap);
textureMapVector.push_back(textureMap2);*/






// Create a material --> OPTIONAL
/*ambientColor.ambientColorEnable = true;
ambientColor.color = 0xFF523513;

// Texture 1
TextureMap * textureMap = new TextureMap();
textureMap->mapEnable=true;
//strcpy_s(textureMap->mapFilename, sizeof(textureMap->mapFilename), "./data/cuerpo1.tga");
textureMap->mapFilename="./data/iuranus_surface.tga";

// Use D3DX to create a texture from a file based image
if( FAILED( D3DXCreateTextureFromFile( Global::device,
textureMap->mapFilename.c_str(), &(textureMap->texture) ) ) )
{
char msg[100];
strcpy_s(msg, sizeof(msg), "Could not find texture ");
strcat_s(msg, sizeof(msg), textureMap->mapFilename.c_str());
MessageBox(NULL, msg, "Demo.exe", MB_OK);
return;
}
textureMap->ownCoordinates=true;
textureMap->hasAplha=true;

// Include textures in vector
textureMapVector.push_back(textureMap);
}*/

/*void Material::activate() const {
// Texture Stage
int stage = 0;

// Texture Coordinates Set to get from vertex
int textureCoordinatesSet=0;

TextureMapVector::const_iterator it = textureMapVector.begin();
while( it != textureMapVector.end() ) {
if ( (*it)->mapEnable ) {
// Set the texture
Global::device->SetTexture( stage, (*it)->texture );

// Define operations to do with textures
if(stage==0) {
// Set the operation
Global::device->SetTextureStageState( stage, D3DTSS_COLOROP,   D3DTOP_MODULATE );
Global::device->SetTextureStageState( stage, D3DTSS_COLORARG1, D3DTA_TEXTURE );
Global::device->SetTextureStageState( stage, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
}
else {
// Set the coordinates index
if( (*it)->ownCoordinates )
Global::device->SetTextureStageState( stage, D3DTSS_TEXCOORDINDEX, ++textureCoordinatesSet );
else
Global::device->SetTextureStageState( stage, D3DTSS_TEXCOORDINDEX, textureCoordinatesSet );

// Set the operation
Global::device->SetTextureStageState( stage, D3DTSS_COLOROP,   D3DTOP_MODULATE );
Global::device->SetTextureStageState( stage, D3DTSS_COLORARG1, D3DTA_TEXTURE );
Global::device->SetTextureStageState( stage, D3DTSS_COLORARG2, D3DTA_CURRENT );
}

// Opacity
if( (*it)->hasAplha ) {
// Alpha operation enabled. Get the alpha value of texture.
Global::device->SetTextureStageState( stage, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1 );
Global::device->SetTextureStageState( stage, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
}
else {
// Alpha operation disabled
Global::device->SetTextureStageState( stage, D3DTSS_ALPHAOP, D3DTOP_DISABLE );
}

stage++;
}

it++;
}

if( ambientColor.ambientColorEnable ) {
// Set the texture factor color
Global::device->SetRenderState(D3DRS_TEXTUREFACTOR, ambientColor.color);

// Set the coordinates index
Global::device->SetTextureStageState( stage, D3DTSS_TEXCOORDINDEX, textureCoordinatesSet );

// In other stages the color is the current one modulate with the texture color.
Global::device->SetTextureStageState( stage, D3DTSS_COLOROP,   D3DTOP_ADD );
Global::device->SetTextureStageState( stage, D3DTSS_COLORARG1, D3DTA_CURRENT );
Global::device->SetTextureStageState( stage, D3DTSS_COLORARG2, D3DTA_TFACTOR );

stage++;
}

if( additionalColor.ambientColorEnable ) {
// Set the texture factor color
Global::device->SetRenderState(D3DRS_TEXTUREFACTOR, additionalColor.color);

// Set the coordinates index
Global::device->SetTextureStageState( stage, D3DTSS_TEXCOORDINDEX, textureCoordinatesSet );

// In other stages the color is the current one modulate with the texture color.
Global::device->SetTextureStageState( stage, D3DTSS_COLOROP,   D3DTOP_ADD );
Global::device->SetTextureStageState( stage, D3DTSS_COLORARG1, D3DTA_CURRENT );
Global::device->SetTextureStageState( stage, D3DTSS_COLORARG2, D3DTA_TFACTOR );

stage++;
}

// Next Stage everything NULL
//Global::device->SetTexture( stage, NULL );
// Color operation disabled
Global::device->SetTextureStageState( stage, D3DTSS_COLOROP, D3DTOP_DISABLE );
// Alpha operation disabled
Global::device->SetTextureStageState( stage, D3DTSS_ALPHAOP, D3DTOP_DISABLE );
}*/