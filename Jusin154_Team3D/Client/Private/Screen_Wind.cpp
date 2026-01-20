#include "pch.h"
#include "Screen_Wind.h"

#include "GameInstance.h"
#include "EffectParts.h"
#include "Wand.h"

CScreen_Wind::CScreen_Wind(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CEffect_Container{ pDevice, pContext }
{
}

CScreen_Wind::CScreen_Wind(const CScreen_Wind& rhs)
	: CEffect_Container(rhs)
{
}

HRESULT CScreen_Wind::Initialize_Prototype()
{

	return S_OK;
}

HRESULT CScreen_Wind::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Child()))
		return E_FAIL;

	m_wstrEffectName = L"Screen_Wind";

	return S_OK;
}

void CScreen_Wind::Priority_Update(_float fTimeDelta)
{
	__super::Priority_Update(fTimeDelta);

}

void CScreen_Wind::Update(_float fTimeDelta)
{
	if (m_bVisible == false)
		return;

	__super::Update(fTimeDelta);

	Update_Event(fTimeDelta);


}

void CScreen_Wind::Late_Update(_float fTimeDelta)
{
	if (m_bVisible == false)
		return;

	__super::Late_Update(fTimeDelta);


}

HRESULT CScreen_Wind::Pre_Setting(CGameObject* pObject, void* pArg)
{
	if (FAILED(__super::Pre_Setting(pObject)))
		return E_FAIL;

	m_isLoop = true;

	m_pWindEffect->Set_Visible(true);

	return S_OK;
}

HRESULT CScreen_Wind::Ready_Components(void* pArg)
{
	if (FAILED(__super::Ready_Components(pArg))) {
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CScreen_Wind::Ready_Child()
{
	CPartObject::PARTOBJECT_DESC PartsDesc{};

	PartsDesc.pParentTransform = m_pTransformCom;

	if (FAILED(Add_PartObject<CEffectParts>("Wind_Screen", g_iStaticLevel, &m_pWindEffect, &PartsDesc)))
	{
		return E_FAIL;
	}

	m_pWindEffect->Load("../Bin/Resources/Data/Effect/ScreenFX/Wind", static_cast<LEVEL>(g_iStaticLevel));

	return S_OK;
}

CScreen_Wind* CScreen_Wind::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CScreen_Wind* pInstance = new CScreen_Wind(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CScreen_Wind");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}


CGameObject* CScreen_Wind::Clone(void* pArg, CGameObject* pOwner)
{
	CScreen_Wind* pInstance = new CScreen_Wind(*this);
	pInstance->m_pOwner = pOwner;
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CScreen_Wind");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

void CScreen_Wind::OnCollision(CGameObject* pOther, void* pDesc)
{
	//CTransform* pOtherTransform = p
	ON_COLLISION_INFO* CollisionDesc = static_cast<ON_COLLISION_INFO*>(pDesc);

}

void CScreen_Wind::Free()
{
	__super::Free();

	SAFE_RELEASE(m_pWindEffect);
}
#ifdef _DEBUG

void CScreen_Wind::Describe_Entity()
{

}

#endif // _DEBUG

HRESULT CScreen_Wind::Bind_ShaderResources()
{
	return S_OK;
}


