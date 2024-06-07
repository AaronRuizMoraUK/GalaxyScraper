#ifndef DIRECTX_H
#define DIRECTX_H

#include <d3dx9.h>
#include <d3d9types.h> // For D3DFORMAT types
#include <d3dx9tex.h> // For D3DXSaveTextureToFile() function

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
typedef LPD3DXRENDERTOSURFACE			RenderToSurface;
typedef D3DVERTEXELEMENT9				VertexElement;
typedef LPDIRECT3DVERTEXDECLARATION9	VertexDeclaration;
typedef LPD3DXEFFECT					Effect;

DeviceCreator createDeviceCreator();

#endif //DIRECTX_H
