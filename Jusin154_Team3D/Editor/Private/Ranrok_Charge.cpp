#include "pch.h"
#include "Ranrok_Charge.h"

#include "GameInstance.h"
#include "EditEffect.h"
#include "Wand.h"
#include "Player.h"
#include "InfoInstance.h"

CRanrok_Charge::CRanrok_Charge(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CEffect_Container{ pDevice, pContext }
{
}

CRanrok_Charge::CRanrok_Charge(const CRanrok_Charge& rhs)
	: CEffect_Container(rhs),
	m_pInfoInstance(CInfoInstance::GetInstance())
{
	SAFE_ADDREF(m_pInfoInstance);
}

HRESULT CRanrok_Charge::Initialize_Prototype()
{


	return S_OK;

}

HRESULT CRanrok_Charge::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components(pArg)))
		return E_FAIL;


	if (FAILED(Load_Package("../Bin/Resources/Data/Effect/MonsterPackage/Ranrok/RanrokCharge")))
		return E_FAIL;

	m_wstrEffectName = L"RanrokCharge";



	m_fDuration = 4.f;


	

	return S_OK;
}

void CRanrok_Charge::Priority_Update(_float fTimeDelta)
{
	__super::Priority_Update(fTimeDelta);

}

void CRanrok_Charge::Update(_float fTimeDelta)
{
	if (m_bVisible == false)
		return;

	__super::Update(fTimeDelta);

	Update_Event(fTimeDelta);




	
}

void CRanrok_Charge::Late_Update(_float fTimeDelta)
{
	if (m_bVisible == false)
		return;

	__super::Late_Update(fTimeDelta);


}

HRESULT CRanrok_Charge::Pre_Setting(CGameObject* pObject, void* pArg)
{
	if (FAILED(__super::Pre_Setting(pObject, nullptr)))
		return E_FAIL;


	 /* 초기 객체 비지블*/

	CEditEffect* pCharge_Goo = Get_PartObject<CEditEffect>("Charge_Goo");
	CEditEffect* pCombinedPT = Get_PartObject<CEditEffect>("CombinedPT");

	pCharge_Goo->Set_Visible(true);
	pCombinedPT->Set_Visible(true);

	 /* 초기 객체 위치 초기화*/

	_vector vOwnerPos = m_pOwner->Get_Component<CCharacter_Controller>()->Get_HeadPosition();


	pCharge_Goo->Get_Component<CTransform>()->Set_State(STATE::POSITION, vOwnerPos);
	pCombinedPT->Get_Component<CTransform>()->Set_State(STATE::POSITION, vOwnerPos);



	return S_OK;
}

HRESULT CRanrok_Charge::Ready_Components(void* pArg)
{
	if (FAILED(__super::Ready_Components(pArg))) {
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CRanrok_Charge::Ready_Child()
{
	return S_OK;
}

CRanrok_Charge* CRanrok_Charge::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CRanrok_Charge* pInstance = new CRanrok_Charge(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CRanrok_Charge");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}


CGameObject* CRanrok_Charge::Clone(void* pArg, CGameObject* pOwner)
{
	CRanrok_Charge* pInstance = new CRanrok_Charge(*this);
	pInstance->m_pOwner = pOwner;
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CRanrok_Charge");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

void CRanrok_Charge::OnCollision(CGameObject* pOther, void* pDesc)
{
	if (m_bHit == false)
		return;

	ON_COLLISION_INFO CollisionDesc = *static_cast<ON_COLLISION_INFO*>(pDesc);

	_vector vPos = XMLoadFloat4(&CollisionDesc.vWorldPos);


}

void CRanrok_Charge::Free()
{
	__super::Free();

	SAFE_RELEASE(m_pInfoInstance);


}
#ifdef _DEBUG
void CRanrok_Charge::Describe_Entity()
{

}
#endif

HRESULT CRanrok_Charge::Bind_ShaderResources()
{
	return S_OK;
}


