#ifndef DIRECTX_H
#define DIRECTX_H

#if __has_include(<d3dx9.h>)
#define D3DX9_SUPPORTED
#endif

#include <d3d9.h>
#include <d3d9types.h> // For D3DFORMAT types

//-----------------------------------------------------------------------------
// Definitions of Directx Types
//-----------------------------------------------------------------------------
typedef LPDIRECT3D9						DeviceCreator;
typedef LPDIRECT3DDEVICE9				Device;
typedef LPDIRECT3DVERTEXBUFFER9			VertexBuffer;
typedef LPDIRECT3DINDEXBUFFER9			IndexBuffer;
typedef LPDIRECT3DTEXTURE9				Texture;
typedef LPDIRECT3DSURFACE9				Surface;
typedef D3DSURFACE_DESC					SurfaceDescription;
typedef D3DVERTEXELEMENT9				VertexElement;
typedef LPDIRECT3DVERTEXDECLARATION9	VertexDeclaration;

DeviceCreator createDeviceCreator();

#ifdef D3DX9_SUPPORTED

#include <d3dx9.h>
#include <d3dx9tex.h> // For D3DXSaveTextureToFile() function

static const bool RenderToSurfaceSupported = true;
typedef LPD3DXRENDERTOSURFACE			RenderToSurface;
typedef LPD3DXEFFECT					Effect;

#else // D3DX9_SUPPORTED

#include "D3DX/D3DXMath.h"
#include "D3DX/D3DXEffect.h"
#include "D3DX/DDSTextureLoader9.h"

#define D3DX_DEFAULT   ((UINT)-1)

static const bool RenderToSurfaceSupported = false;
typedef void* RenderToSurface;

enum D3DXIMAGE_FILEFORMAT
{
    D3DXIFF_JPG
};

#endif // D3DX9_SUPPORTED

#endif //DIRECTX_H
