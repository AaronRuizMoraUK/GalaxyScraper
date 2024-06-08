#ifndef D3DXMath_H
#define D3DXMath_H

#include "SimpleMath.h"

typedef DirectX::SimpleMath::Vector2 D3DXVECTOR2;
typedef DirectX::SimpleMath::Vector3 D3DXVECTOR3;
typedef DirectX::SimpleMath::Vector4 D3DXVECTOR4;
typedef DirectX::SimpleMath::Matrix D3DXMATRIX;
typedef DirectX::SimpleMath::Quaternion D3DXQUATERNION;

static const float D3DX_PI = 3.14159265359f;

inline void D3DXMatrixIdentity(D3DXMATRIX* matrix)
{
    *matrix = D3DXMATRIX();
}

inline void D3DXMatrixMultiply(D3DXMATRIX* out, const D3DXMATRIX* lhs, const D3DXMATRIX* rhs)
{
    *out = *lhs * *rhs;
}

inline float D3DXVec2Length(const D3DXVECTOR2* vector)
{
    return vector->Length();
}

inline float D3DXVec3Length(const D3DXVECTOR3* vector)
{
    return vector->Length();
}

inline void D3DXVec3Cross(D3DXVECTOR3* out, const D3DXVECTOR3* lhs, const D3DXVECTOR3* rhs)
{
    *out = lhs->Cross(*rhs);
}

inline float D3DXVec3Dot(const D3DXVECTOR3* lhs, const D3DXVECTOR3* rhs)
{
    return lhs->Dot(*rhs);
}

inline void D3DXVec3TransformCoord(D3DXVECTOR3* out, const D3DXVECTOR3* vector, const D3DXMATRIX* matrix)
{
    D3DXVECTOR3::Transform(*vector, *matrix, *out);
}

inline void D3DXVec3TransformNormal(D3DXVECTOR3* out, const D3DXVECTOR3* vector, const D3DXMATRIX* matrix)
{
    D3DXVECTOR3::TransformNormal(*vector, *matrix, *out);
}

inline void D3DXVec3Normalize(D3DXVECTOR3* out, const D3DXVECTOR3* vector)
{
    vector->Normalize(*out);
}

inline void D3DXQuaternionNormalize(D3DXQUATERNION* out, const D3DXQUATERNION* quat)
{
    quat->Normalize(*out);
}

inline float D3DXQuaternionDot(const D3DXQUATERNION* lhs, const D3DXQUATERNION* rhs)
{
    return lhs->Dot(*rhs);
}

inline void D3DXQuaternionRotationMatrix(D3DXQUATERNION* out, const D3DXMATRIX* matrix)
{
    *out = D3DXQUATERNION::CreateFromRotationMatrix(*matrix);
}

inline void D3DXMatrixRotationQuaternion(D3DXMATRIX* out, const D3DXQUATERNION* quat)
{
    *out = D3DXMATRIX::CreateFromQuaternion(*quat);
}

inline void D3DXQuaternionInverse(D3DXQUATERNION* out, const D3DXQUATERNION* quat)
{
    quat->Inverse(*out);
}

inline void D3DXMatrixOrthoOffCenterRH(D3DXMATRIX* out, float left, float right, float bottom, float top, float zNearPlane, float zFarPlane)
{
    *out = D3DXMATRIX::CreateOrthographicOffCenter(left, right, bottom, top, zNearPlane, zFarPlane);
}

inline void D3DXMatrixPerspectiveFovRH(D3DXMATRIX* out, float fovY, float aspectRatio, float nearPlane, float farPlane)
{
    *out = D3DXMATRIX::CreatePerspectiveFieldOfView(fovY, aspectRatio, nearPlane, farPlane);
}

inline void D3DXMatrixLookAtRH(D3DXMATRIX* out, const D3DXVECTOR3* eye, const D3DXVECTOR3* target, const D3DXVECTOR3* up)
{
    *out = D3DXMATRIX::CreateLookAt(*eye, *target, *up);
}

inline void D3DXMatrixRotationX(D3DXMATRIX* out, float radians)
{
    *out = D3DXMATRIX::CreateRotationX(radians);
}

inline void D3DXMatrixRotationY(D3DXMATRIX* out, float radians)
{
    *out = D3DXMATRIX::CreateRotationY(radians);
}

inline void D3DXMatrixRotationZ(D3DXMATRIX* out, float radians)
{
    *out = D3DXMATRIX::CreateRotationZ(radians);
}

inline void D3DXMatrixRotationAxis(D3DXMATRIX* out, const D3DXVECTOR3* axis, float radians)
{
    *out = D3DXMATRIX::CreateFromAxisAngle(*axis, radians);
}

inline void D3DXMatrixScaling(D3DXMATRIX* out, float xs, float ys, float zs)
{
    *out = D3DXMATRIX::CreateScale(xs, ys, zs);
}

inline void D3DXMatrixInverse(D3DXMATRIX* out, float* det, const D3DXMATRIX* matrix)
{
    matrix->Invert(*out);
}

#endif //D3DXMath_H
