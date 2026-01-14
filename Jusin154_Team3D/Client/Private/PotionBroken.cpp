#include "pch.h"
#include "PotionBroken.h"

#include "GameInstance.h"
#include "EffectParts.h"
#include "Wand.h"
#include "Player.h"
#include "Goblin_BattleAxe.h"
#include "TrailObject.h"

CPotionBroken::CPotionBroken(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CEffect_Container{ pDevice, pContext }
{
}

CPotionBroken::CPotionBroken(const CPotionBroken& rhs)
	: CEffect_Container(rhs)
{
}

HRESULT CPotionBroken::Initialize_Prototype()
{

	if (FAILED(Load_Package("../Bin/Resources/Data/Effect/Package/PotionEffect")))
		return E_FAIL;

	return S_OK;

}

HRESULT CPotionBroken::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components(pArg)))
		return E_FAIL;

	if (FAILED(Create_Effect()))
		return E_FAIL;




	m_fDuration = 5.f;

	return S_OK;
}

void CPotionBroken::Priority_Update(_float fTimeDelta)
{
	__super::Priority_Update(fTimeDelta);

}

void CPotionBroken::Update(_float fTimeDelta)
{
	if (m_bVisible == false)
		return;

	__super::Update(fTimeDelta);

	Update_Event(fTimeDelta);


}

void CPotionBroken::Late_Update(_float fTimeDelta)
{
	if (m_bVisible == false)
		return;

	__super::Late_Update(fTimeDelta);


}

HRESULT CPotionBroken::Pre_Setting(CGameObject* pObject, void* pArg)
{
	if (FAILED(__super::Pre_Setting(pObject, nullptr)))
		return E_FAIL;

	CEffectParts* pPotionEffect = Get_PartObject<CEffectParts>("PotionEffect");
	CEffectParts* pPotionSmoke = Get_PartObject<CEffectParts>("PotionSmoke");

	_vector vPosVector = m_pOwner->Get_Component<CTransform>()->Get_State(STATE::POSITION);

	pPotionEffect->Get_Component<CTransform>()->Set_State(STATE::POSITION, vPosVector);
	pPotionSmoke->Get_Component<CTransform>()->Set_State(STATE::POSITION, vPosVector);

	pPotionEffect->Set_Visible(true);
	pPotionSmoke->Set_Visible(true);


	return S_OK;
}

HRESULT CPotionBroken::Ready_Components(void* pArg)
{
	if (FAILED(__super::Ready_Components(pArg))) {
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CPotionBroken::Ready_Child()
{
	return S_OK;
}

CPotionBroken* CPotionBroken::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CPotionBroken* pInstance = new CPotionBroken(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CPotionBroken");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}


CGameObject* CPotionBroken::Clone(void* pArg, CGameObject* pOwner)
{
	CPotionBroken* pInstance = new CPotionBroken(*this);
	pInstance->m_pOwner = pOwner;
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CPotionBroken");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

void CPotionBroken::OnCollision(CGameObject* pOther, void* pDesc)
{

}

void CPotionBroken::Free()
{
	__super::Free();


}
#ifdef _DEBUG
void CPotionBroken::Describe_Entity()
{

}
#endif

HRESULT CPotionBroken::Bind_ShaderResources()
{
	return S_OK;
}


