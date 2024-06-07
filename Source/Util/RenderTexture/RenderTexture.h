#ifndef RENDER_TEXTURE_H
#define RENDER_TEXTURE_H

#include "Directx/Directx.h"
#include <string>

/**
* This class make possible render to a texture
*/

class RenderTexture
{
protected:
	Surface			surface;
	Texture			texture;
	D3DFORMAT		colorFormat;
	RenderToSurface	renderToSurface;

	D3DFORMAT		stencilZBufferFormat;
	bool			useDepthBuffer;

	int				width;
	int				height;

	bool			initiated;

public:
	RenderTexture( );
	~RenderTexture( );

	bool create( int pWidth, int pHeight, D3DFORMAT pColorFormat=D3DFMT_A8R8G8B8, D3DFORMAT pStencilZBufferFormat=D3DFMT_D24S8 );
	void destroy( );

	void save( const std::string &filename, D3DXIMAGE_FILEFORMAT format=D3DXIFF_JPG ) const;

	void begin( );
	void end( );
	
	const Texture getTexture( ) const { return texture; };
	D3DFORMAT getColorFormat( ) const { return colorFormat; };

	D3DFORMAT getStencilZBufferFormat( ) const { return stencilZBufferFormat; };
	bool isUsingDepthBuffer( ) const { return useDepthBuffer; };

	int	getWidth( ) const { return width; };
	int getHeight( ) const { return height; };

	bool isInitiated( ) const { return initiated; };
};

#endif // RENDER_TEXTURE_H
