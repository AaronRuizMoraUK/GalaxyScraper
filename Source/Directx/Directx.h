#ifndef DIRECTX_H
#define DIRECTX_H

//#define D3DX_FOUND

#include <d3d9.h>
#include <d3d9types.h> // For D3DFORMAT types

#ifdef D3DX9_SUPPORTED
#include <d3dx9.h>
#include <d3dx9tex.h> // For D3DXSaveTextureToFile() function
#else
#include <DirectXMath.h>
#include <string>
#endif

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

#ifdef D3DX9_SUPPORTED
static const bool RenderToSurfaceSupported = true;
typedef LPD3DXRENDERTOSURFACE			RenderToSurface;
typedef LPD3DXEFFECT					Effect;
#else

static const bool RenderToSurfaceSupported = false;
typedef void* RenderToSurface;

enum D3DXIMAGE_FILEFORMAT
{
    D3DXIFF_JPG
};

// DirectXMath
typedef DirectX::XMFLOAT2 D3DXVECTOR2;
typedef DirectX::XMFLOAT3 D3DXVECTOR3;
typedef DirectX::XMFLOAT4 D3DXVECTOR4;
typedef DirectX::XMMATRIX D3DXMATRIX;
typedef DirectX::XMVECTOR D3DXQUATERNION;
inline void D3DXMatrixIdentity(D3DXMATRIX* matrix)
{
    *matrix = DirectX::XMMatrixIdentity();
}

// Effect
typedef int D3DXHANDLE;
typedef LPDIRECT3DVERTEXSHADER9 VertexShader;
typedef LPDIRECT3DPIXELSHADER9  PixelShader;

class Effect
{
public:
    Effect() = default;

    D3DXHANDLE GetParameterByName(D3DXHANDLE _notUsed, const std::string& name)
    {
        return -1;
    }
    HRESULT SetVector(D3DXHANDLE technique, D3DXVECTOR4* vector)
    {
        return D3D_OK;
    }
    HRESULT SetMatrix(D3DXHANDLE technique, D3DXMATRIX* matrix)
    {
        return D3D_OK;
    }

    D3DXHANDLE GetTechniqueByName(const std::string& name)
    {
        return -1;
    }
    HRESULT SetTechniqueByName(D3DXHANDLE technique)
    {
        return D3D_OK;
    }

    void Begin(UINT* cPasses, UINT iPass) { *cPasses = 1; }
    void BeginPass(UINT iPass) {}
    void EndPass() {}
    void End() {}
};

#endif

DeviceCreator createDeviceCreator();

#endif //DIRECTX_H
