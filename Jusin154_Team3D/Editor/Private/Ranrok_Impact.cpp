#include "pch.h"
#include "Ranrok_Impact.h"

#include "GameInstance.h"
#include "EditEffect.h"
#include "Wand.h"
#include "Player.h"
#include "InfoInstance.h"

CRanrok_Impact::CRanrok_Impact(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CEffect_Container{ pDevice, pContext }
{
}

CRanrok_Impact::CRanrok_Impact(const CRanrok_Impact& rhs)
	: CEffect_Container(rhs),
	m_pInfoInstance(CInfoInstance::GetInstance())
{
	SAFE_ADDREF(m_pInfoInstance);
}

HRESULT CRanrok_Impact::Initialize_Prototype()
{


	return S_OK;

}

HRESULT CRanrok_Impact::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components(pArg)))
		return E_FAIL;


	if (FAILED(Load_Package("../Bin/Resources/Data/Effect/MonsterPackage/Ranrok/RanrokBigHit")))
		return E_FAIL;

	m_wstrEffectName = L"RanrokImpact";



	m_fDuration = 5.f;
	

	return S_OK;
}

void CRanrok_Impact::Priority_Update(_float fTimeDelta)
{
	__super::Priority_Update(fTimeDelta);

}

void CRanrok_Impact::Update(_float fTimeDelta)
{
	if (m_bVisible == false)
		return;

	__super::Update(fTimeDelta);

	Update_Event(fTimeDelta);




	
}

void CRanrok_Impact::Late_Update(_float fTimeDelta)
{
	if (m_bVisible == false)
		return;

	__super::Late_Update(fTimeDelta);


}

HRESULT CRanrok_Impact::Pre_Setting(CGameObject* pObject, void* pArg)
{
	if (FAILED(__super::Pre_Setting(pObject, nullptr)))
		return E_FAIL;


	 /* 초기 객체 비지블*/
	CEditEffect* pHit_PT0 = Get_PartObject<CEditEffect>("Hit_PT0");
	CEditEffect* pHit_Pulse = Get_PartObject<CEditEffect>("Hit_Pulse");
	CEditEffect* pRock_PT = Get_PartObject<CEditEffect>("Rock_PT");
	CEditEffect* pSplash_Hit = Get_PartObject<CEditEffect>("Splash_Hit");
	CEditEffect* pSpline0 = Get_PartObject<CEditEffect>("Spline0");
	CEditEffect* pSpline1 = Get_PartObject<CEditEffect>("Spline1");

	pHit_PT0->Set_Visible(true);
	pHit_Pulse->Set_Visible(true);
	pRock_PT->Set_Visible(true);
	pSplash_Hit->Set_Visible(true);
	pSpline0->Set_Visible(true);
	pSpline1->Set_Visible(true);

	/* 초기 객체 위치 초기화*/

	_vector vOwnerPos = m_pOwner->Get_Component<CCharacter_Controller>()->Get_Position();

	pHit_PT0->Get_Component<CTransform>()->Set_State(STATE::POSITION, vOwnerPos);
	pHit_Pulse->Get_Component<CTransform>()->Set_State(STATE::POSITION, vOwnerPos);
	pRock_PT->Get_Component<CTransform>()->Set_State(STATE::POSITION, vOwnerPos);
	pSplash_Hit->Get_Component<CTransform>()->Set_State(STATE::POSITION, vOwnerPos);
	pSpline0->Get_Component<CTransform>()->Set_State(STATE::POSITION, vOwnerPos);
	pSpline1->Get_Component<CTransform>()->Set_State(STATE::POSITION, vOwnerPos);

	return S_OK;
}

HRESULT CRanrok_Impact::Ready_Components(void* pArg)
{
	if (FAILED(__super::Ready_Components(pArg))) {
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CRanrok_Impact::Ready_Child()
{
	return S_OK;
}

CRanrok_Impact* CRanrok_Impact::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CRanrok_Impact* pInstance = new CRanrok_Impact(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CRanrok_Impact");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}


CGameObject* CRanrok_Impact::Clone(void* pArg, CGameObject* pOwner)
{
	CRanrok_Impact* pInstance = new CRanrok_Impact(*this);
	pInstance->m_pOwner = pOwner;
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CRanrok_Impact");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

void CRanrok_Impact::OnCollision(CGameObject* pOther, void* pDesc)
{
	if (m_bHit == false)
		return;

	ON_COLLISION_INFO CollisionDesc = *static_cast<ON_COLLISION_INFO*>(pDesc);

	_vector vPos = XMLoadFloat4(&CollisionDesc.vWorldPos);


}

void CRanrok_Impact::Free()
{
	__super::Free();

	SAFE_RELEASE(m_pInfoInstance);


}
#ifdef _DEBUG
void CRanrok_Impact::Describe_Entity()
{

}
#endif

HRESULT CRanrok_Impact::Bind_ShaderResources()
{
	return S_OK;
}


