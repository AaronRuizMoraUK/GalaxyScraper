#ifndef D3DXEffect_H
#define D3DXEffect_H

#include <string>

typedef int D3DXHANDLE;

typedef LPDIRECT3DVERTEXSHADER9 VertexShader;
typedef LPDIRECT3DPIXELSHADER9  PixelShader;

class D3DXEffect
{
public:
    D3DXEffect() = default;

    void Release() {}

    D3DXHANDLE GetParameterByName(D3DXHANDLE _notUsed, const std::string& name)
    {
        return -1;
    }
    HRESULT SetFloat(D3DXHANDLE technique, float value)
    {
        return D3D_OK;
    }
    HRESULT SetFloatArray(D3DXHANDLE technique, const float* values, int arrayCount)
    {
        return D3D_OK;
    }
    HRESULT SetFloatArray(D3DXHANDLE technique, const D3DXVECTOR2& vector, int arrayCount)
    {
        assert(arrayCount == 2);
        return D3D_OK;
    }
    HRESULT SetVector(D3DXHANDLE technique, const D3DXVECTOR4* vector)
    {
        return D3D_OK;
    }
    HRESULT SetVectorArray(D3DXHANDLE technique, const D3DXVECTOR4* vector, int arrayCount)
    {
        return D3D_OK;
    }
    HRESULT SetMatrix(D3DXHANDLE technique, const D3DXMATRIX* matrix)
    {
        return D3D_OK;
    }
    HRESULT SetTexture(D3DXHANDLE technique, const Texture texture)
    {
        return D3D_OK;
    }

    void CommitChanges()
    {
    }

    D3DXHANDLE GetTechniqueByName(const std::string& name)
    {
        return -1;
    }
    HRESULT SetTechnique(D3DXHANDLE technique)
    {
        return D3D_OK;
    }

    void Begin(UINT* cPasses, UINT iPass) { *cPasses = 1; }
    void BeginPass(UINT iPass) {}
    void EndPass() {}
    void End() {}
};

typedef D3DXEffect*  Effect;

#endif //D3DXEffect_H
