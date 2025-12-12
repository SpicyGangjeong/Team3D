#include "pch.h"
#include "Shadow.h"
#include "Shader.h"

CShadow::CShadow()
{

}

HRESULT CShadow::Ready_Shadow_Light(const SHADOW_LIGHT_DESC& Desc)
{
	m_Desc = Desc;
	XMStoreFloat4x4(&m_TransformationMatrices[ENUM_CLASS(D3DTS::VIEW)],
		XMMatrixLookAtLH(XMLoadFloat4(&m_Desc.vEye), XMLoadFloat4(&m_Desc.vAt), XMVectorSet(0.f, 1.f, 0.f, 1.f)));

	XMStoreFloat4x4(&m_TransformationMatrices[ENUM_CLASS(D3DTS::PROJ)],
		XMMatrixOrthographicLH(m_Desc.fWidth, m_Desc.fHeight, m_Desc.fNear, m_Desc.fFar));

	XMStoreFloat4x4(&m_TransformationMatrices[ENUM_CLASS(D3DTS::VIEW_INV)],
		XMMatrixInverse(nullptr, XMLoadFloat4x4(&m_TransformationMatrices[ENUM_CLASS(D3DTS::VIEW)])));

	XMStoreFloat4x4(&m_TransformationMatrices[ENUM_CLASS(D3DTS::PROJ_INV)],
		XMMatrixInverse(nullptr, XMLoadFloat4x4(&m_TransformationMatrices[ENUM_CLASS(D3DTS::PROJ)])));

	return S_OK;
}

HRESULT CShadow::Bind_Shadow_Resource(CShader* pShader, const _char* pConstantName, D3DTS eType) const
{
	return pShader->Bind_Matrix(pConstantName, &m_TransformationMatrices[ENUM_CLASS(eType)]);
}

CShadow* CShadow::Create()
{
	return new CShadow();
}


void CShadow::Free()
{
	__super::Free();
}
