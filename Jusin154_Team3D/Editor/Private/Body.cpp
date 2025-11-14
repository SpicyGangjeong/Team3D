#include "pch.h"
#include "Body.h"

#include "GameInstance.h"
#include "RootModelPart.h"

CBody::CBody(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CModelParts(pDevice, pContext)
{
}

CBody::CBody(const CBody& Prototype)
	: CModelParts(Prototype)
{
}

HRESULT CBody::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CBody::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	return S_OK;
}

void CBody::Priority_Update(_float fTimeDelta)
{

}

void CBody::Update(_float fTimeDelta)
{
	__super::Update(fTimeDelta);
}

void CBody::Late_Update(_float fTimeDelta)
{
	_float4 vPos;
	XMStoreFloat4(&vPos, Get_WorldPostion());
	m_pGameInstance->Add_RenderGroup(RENDER::BLEND, this, vPos, 20.f);
}

HRESULT CBody::Render()
{
	__super::Render();

	return S_OK;
}


HRESULT CBody::Ready_Components()
{
	if (m_strModelPrototypeTag != L"")
	{
		/* Com_Model */
		if (FAILED(__super::Add_Asset_Component(g_iStaticLevel, m_strModelPrototypeTag,
			reinterpret_cast<CComponent**>(&m_pModelCom))))
			return E_FAIL;
	}

	return S_OK;
}

CBody* CBody::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CBody* pInstance = new CBody(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CBody");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

CGameObject* CBody::Clone(void* pArg, CGameObject* pOwner)
{
	CBody* pInstance = new CBody(*this);
	pInstance->m_pOwner = pOwner;
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CBody");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

void CBody::Free()
{
	__super::Free();


}

void CBody::Describe_Entity()
{
}
