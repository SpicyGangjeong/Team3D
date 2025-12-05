#include "pch.h"
#include "PipeLine.h"
CPipeLine::CPipeLine()
{
}

void CPipeLine::Set_Transform(D3DTS eState, _fmatrix TransformStateMatrix)
{
	XMStoreFloat4x4(&m_TransformStateMatrices[ENUM_CLASS(eState)], TransformStateMatrix);
}

const _float4x4* CPipeLine::Get_Transform_Float4x4(D3DTS eState)
{
	return &m_TransformStateMatrices[ENUM_CLASS(eState)];
}

_matrix CPipeLine::Get_Transform_Matrix(D3DTS eState)
{
	return XMLoadFloat4x4(&m_TransformStateMatrices[ENUM_CLASS(eState)]);
}

const _float4* CPipeLine::Get_CamPosition()
{
	return &m_vCamPosition;
}

const _vector CPipeLine::Get_CamXMPosition()
{
	return XMLoadFloat4(&m_vCamPosition);
}

void CPipeLine::Transform_Frustum_ToLocalSpace(_fmatrix WorldMatrixInverse)
{
	_float4		vLocalPoints[8] = {};
	for (size_t i = 0; i < 8; i++)
	{
		XMStoreFloat4(&vLocalPoints[i],
			XMVector3TransformCoord(XMLoadFloat4(&m_vWorldPoints[i]), WorldMatrixInverse));
	}

	Make_Planes(vLocalPoints, m_vLocalPlanes);
}

_bool CPipeLine::isIn_WorldFrustum(_fvector vWorldPos, _float fRadius)
{
	for (size_t i = 0; i < 6; i++)
	{
		if (fRadius < XMVectorGetX(XMPlaneDotCoord(XMLoadFloat4(&m_vWorldPlanes[i]), vWorldPos)))
			return false;
	}

	return true;
}

_bool CPipeLine::isIn_LocalFrustum(_fvector vLocalPos, _float fRadius)
{
	for (size_t i = 0; i < 6; i++)
	{
		if (fRadius < XMVectorGetX(XMPlaneDotCoord(XMLoadFloat4(&m_vLocalPlanes[i]), vLocalPos)))
			return false;
	}

	return true;
}

void CPipeLine::Update()
{
	for (_uint i = ENUM_CLASS(D3DTS::VIEW_INV); i < ENUM_CLASS(D3DTS::END); ++i)
	{
		XMStoreFloat4x4(&m_TransformStateMatrices[i], XMMatrixInverse(nullptr, Get_Transform_Matrix(static_cast<D3DTS>(i-2))));
	}

	memcpy(&m_vCamPosition, &m_TransformStateMatrices[ENUM_CLASS(D3DTS::VIEW_INV)].m[3], sizeof(_float4));
	_matrix ProjInvMatrix = XMLoadFloat4x4(&m_TransformStateMatrices[ENUM_CLASS(D3DTS::PROJ_INV)]);
	_matrix ViewInvMatrix = XMLoadFloat4x4(&m_TransformStateMatrices[ENUM_CLASS(D3DTS::VIEW_INV)]);
	for (size_t i = 0; i < 8; i++)
	{
		XMStoreFloat4(&m_vWorldPoints[i], XMVector3TransformCoord(XMLoadFloat4(&m_vOriginalPoints[i]),	ProjInvMatrix));
		XMStoreFloat4(&m_vWorldPoints[i], XMVector3TransformCoord(XMLoadFloat4(&m_vWorldPoints[i]),		ViewInvMatrix));
	}

	Make_Planes(m_vWorldPoints, m_vWorldPlanes);
}

HRESULT CPipeLine::Initialize()
{
	m_vOriginalPoints[0] = _float4(-1.f,	1.f,	0.f, 1.f); // LT
	m_vOriginalPoints[1] = _float4(1.f,		1.f,	0.f, 1.f); // RT
	m_vOriginalPoints[2] = _float4(1.f,		-1.f,	0.f, 1.f); // RB
	m_vOriginalPoints[3] = _float4(-1.f,	-1.f,	0.f, 1.f); // LB

	m_vOriginalPoints[4] = _float4(-1.f,	1.f,	1.f, 1.f); // LT
	m_vOriginalPoints[5] = _float4(1.f,		1.f,	1.f, 1.f); // RT
	m_vOriginalPoints[6] = _float4(1.f,		-1.f,	1.f, 1.f); // RB
	m_vOriginalPoints[7] = _float4(-1.f,	-1.f,	1.f, 1.f); // LB

	return S_OK;
}

void CPipeLine::Make_Planes(const _float4* pPoints, _float4* pPlanes)
{
	XMStoreFloat4(&pPlanes[0], XMPlaneFromPoints(XMLoadFloat4(&pPoints[1]), XMLoadFloat4(&pPoints[5]), XMLoadFloat4(&pPoints[6])));
	XMStoreFloat4(&pPlanes[1], XMPlaneFromPoints(XMLoadFloat4(&pPoints[4]), XMLoadFloat4(&pPoints[0]), XMLoadFloat4(&pPoints[3])));
	XMStoreFloat4(&pPlanes[2], XMPlaneFromPoints(XMLoadFloat4(&pPoints[4]), XMLoadFloat4(&pPoints[5]), XMLoadFloat4(&pPoints[1])));
	XMStoreFloat4(&pPlanes[3], XMPlaneFromPoints(XMLoadFloat4(&pPoints[3]), XMLoadFloat4(&pPoints[2]), XMLoadFloat4(&pPoints[6])));
	XMStoreFloat4(&pPlanes[4], XMPlaneFromPoints(XMLoadFloat4(&pPoints[5]), XMLoadFloat4(&pPoints[4]), XMLoadFloat4(&pPoints[7])));
	XMStoreFloat4(&pPlanes[5], XMPlaneFromPoints(XMLoadFloat4(&pPoints[0]), XMLoadFloat4(&pPoints[1]), XMLoadFloat4(&pPoints[2])));
}

CPipeLine* CPipeLine::Create()
{
	CPipeLine* pInstance = new CPipeLine();

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CPipeLine");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}
void CPipeLine::Free()
{
	__super::Free();
}
