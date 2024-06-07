#ifndef FADE_H
#define FADE_H

#include "Mesh/IMesh.h"

/**
* Class to make fades out, int, mediums out, mediums int, etc.
* It is made to work with an orthogonal camera.
*/

class Fade
{
protected:
	// Variable to control alpha value
	// alpha = 1.0f -> The Black Quad is shown, so the screen is black
	// alpha = 0.0f -> The Black Quad is not shown, so the screen shows the render
	float alpha;

	D3DXMATRIX identityTransformMatrix;

	const IMesh *mesh;

	Effect shader;
	const static std::string shaderName;
	const static std::string techniqueName;
	
	float duration;
	float time;

	bool initedFadeIn;
	bool initedFadeOut;
	bool initedMediumFadeOut;

	float initAlphaFadeIn;
	float initAlphaFadeOut;

public:
	Fade();

	virtual ~Fade() {
		OutputDebugString("Destroying Fade...\n");

		// Object does not destroy its Mesh, MeshFactory does!!

		// EffectFactory is who destrois the effect (shader)!!
	};

	void create( );
	bool isFinished( ) const { return (time >= duration); };

	void update( float deltaTime=(1.0f/60.0f) );

	void renderFadeIn( float pDuration, const D3DXVECTOR3 &color=D3DXVECTOR3(0,0,0) );
	void renderFadeOut( float pDuration, const D3DXVECTOR3 &color=D3DXVECTOR3(0,0,0) );
	void renderMediumFadeOut( float pDuration, const D3DXVECTOR3 &color=D3DXVECTOR3(0,0,0) );

	void reloadShader( );

private:
	void initVariables( );

	void render( const D3DXVECTOR3 &color ) const;
};

#endif //FADE_H
