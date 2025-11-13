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

void CPipeLine::Update()
{
	for (_uint i = ENUM_CLASS(D3DTS::VIEW_INV); i < ENUM_CLASS(D3DTS::END); ++i)
	{
		XMStoreFloat4x4(&m_TransformStateMatrices[i], XMMatrixInverse(nullptr, Get_Transform_Matrix(static_cast<D3DTS>(i-2))));
	}

	memcpy(&m_vCamPosition, &m_TransformStateMatrices[ENUM_CLASS(D3DTS::VIEW_INV)].m[3], sizeof(_float4));
}

CPipeLine* CPipeLine::Create()
{
	return new CPipeLine();
}
void CPipeLine::Free()
{
	__super::Free();
}
