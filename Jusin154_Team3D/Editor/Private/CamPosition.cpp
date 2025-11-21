#include "pch.h"

#include "CamPosition.h"


CCamPosition::CCamPosition(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CPartObject(pDevice, pContext)
{
}
CCamPosition::CCamPosition(const CCamPosition& rhs)
	:CPartObject(rhs)
{
}
HRESULT CCamPosition::Initialize_Prototype()
{
	return S_OK;
}
HRESULT CCamPosition::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg))) {
		return E_FAIL;
	}
	return S_OK;
}
HRESULT CCamPosition::Ready_Components(void* pArg)
{
	if (FAILED(__super::Ready_Components(pArg))) {
		return E_FAIL;
	}
	return S_OK;
}
void CCamPosition::Free()
{
	__super::Free();
}

HRESULT CCamPosition::Bind_ShaderResources()
{
	return E_NOTIMPL;
}

CGameObject* CCamPosition::Clone(void* pArg, CGameObject* pOwner)
{
	return nullptr;
}

void CCamPosition::Describe_Entity()
{
}
