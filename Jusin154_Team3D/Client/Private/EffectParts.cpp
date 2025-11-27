#include "pch.h"
#include "EffectParts.h"

#include "GameInstance.h"
#include "Instance_Model.h"

CEffectParts::CEffectParts(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CEffectObject{ pDevice, pContext }
{
}

CEffectParts::CEffectParts(const CEffectParts& rhs)
	: CEffectObject(rhs)
{
}

HRESULT CEffectParts::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CEffectParts::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components(pArg)))
		return E_FAIL;

	
	Set_Visible(false);

	return S_OK;
}

void CEffectParts::Priority_Update(_float fTimeDelta)
{

}

void CEffectParts::Update(_float fTimeDelta)
{

	if (m_bVisible == false)
		return;

	if (m_pInstance_ModelCom == nullptr)
		return;

	m_pInstance_ModelCom->Drop(fTimeDelta);

	if (m_EffectInfo.isBillboard)
		m_pGameInstance->BillBoard(m_pTransformCom);


}

void CEffectParts::Late_Update(_float fTimeDelta)
{

	if (m_bVisible == false)
		return;

	if (m_pInstance_ModelCom == nullptr)
		return;

	if (m_EffectInfo.isBillboard)
		m_pGameInstance->BillBoard(m_pTransformCom);


	if (m_EffectInfo.isBlur == true)
	{
		m_pGameInstance->Add_RenderGroup(RENDER::BLUR, this);
	}

	if (m_EffectInfo.isOnlyBlur == true)
		return;

	m_pGameInstance->Add_RenderGroup(m_EffectInfo.eRenderOrder, this);

}



HRESULT CEffectParts::Ready_Components(void* pArg)
{
	if (FAILED(__super::Ready_Components(pArg)))
	{
		return E_FAIL;
	}

	if (FAILED(Add_Asset_Component(g_iStaticLevel, FX_INSTANCE_MODEL, (CComponent**)&m_pShaderCom, nullptr)))
	{
		return E_FAIL;
	}
	return S_OK;
}


CEffectParts* CEffectParts::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CEffectParts* pInstance = new CEffectParts(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CEffectParts");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

CGameObject* CEffectParts::Clone(void* pArg, CGameObject* pOwner)
{
	CEffectParts* pInstance = new CEffectParts(*this);
	pInstance->m_pOwner = pOwner;
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CEffectParts");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

void CEffectParts::Free()
{
	__super::Free();

}
#ifdef _DEBUG

void CEffectParts::Describe_Entity()
{

}

#endif // _DEBUG
