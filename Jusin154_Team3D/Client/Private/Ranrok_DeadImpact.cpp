#include "pch.h"
#include "Ranrok_DeadImpact.h"

#include "GameInstance.h"
#include "EffectParts.h"
#include "Wand.h"
#include "Player.h"
#include "InfoInstance.h"

CRanrok_DeadImpact::CRanrok_DeadImpact(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CEffect_Container{ pDevice, pContext }
{
}

CRanrok_DeadImpact::CRanrok_DeadImpact(const CRanrok_DeadImpact& rhs)
	: CEffect_Container(rhs),
	m_pInfoInstance(CInfoInstance::GetInstance())
{
	SAFE_ADDREF(m_pInfoInstance);
}

HRESULT CRanrok_DeadImpact::Initialize_Prototype()
{


	if (FAILED(Load_Package("../Bin/Resources/Data/Effect/MonsterPackage/Ranrok/RanrokDeadImpact")))
		return E_FAIL;

	return S_OK;

}

HRESULT CRanrok_DeadImpact::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components(pArg)))
		return E_FAIL;

	if (FAILED(Create_Effect()))
		return E_FAIL;

	m_wstrEffectName = L"RanrokDeadImpact";



	m_fDuration = 5.f;
	

	return S_OK;
}

void CRanrok_DeadImpact::Priority_Update(_float fTimeDelta)
{
	__super::Priority_Update(fTimeDelta);

}

void CRanrok_DeadImpact::Update(_float fTimeDelta)
{
	if (m_bVisible == false)
		return;

	__super::Update(fTimeDelta);

	Update_Event(fTimeDelta);




	
}

void CRanrok_DeadImpact::Late_Update(_float fTimeDelta)
{
	if (m_bVisible == false)
		return;

	__super::Late_Update(fTimeDelta);


}

HRESULT CRanrok_DeadImpact::Pre_Setting(CGameObject* pObject, void* pArg)
{
	if (FAILED(__super::Pre_Setting(pObject, nullptr)))
		return E_FAIL;

	 /* 초기 객체 비지블*/

	CEffectParts* pDistortion = Get_PartObject<CEffectParts>("Distortion");
	CEffectParts* pDistortion_Small = Get_PartObject<CEffectParts>("Distortion_Small");
	CEffectParts* pPulse_White = Get_PartObject<CEffectParts>("Pulse_White");
	CEffectParts* pSplash_Black = Get_PartObject<CEffectParts>("Splash_Black");
	CEffectParts* pSplash_Red = Get_PartObject<CEffectParts>("Splash_Red");
	CEffectParts* pSpline0 = Get_PartObject<CEffectParts>("Spline0");
	CEffectParts* pSpline1 = Get_PartObject<CEffectParts>("Spline1");
	CEffectParts* pSide_Pt = Get_PartObject<CEffectParts>("Side_Pt");
	CEffectParts* pCircle_0 = Get_PartObject<CEffectParts>("Circle_0");
	
	pDistortion->Set_Visible(true);
	pDistortion_Small->Set_Visible(true);
	pPulse_White->Set_Visible(true);
	pSplash_Black->Set_Visible(true);
	pSplash_Red->Set_Visible(true);
	pSpline0->Set_Visible(true);
	pSpline1->Set_Visible(true);
	pSide_Pt->Set_Visible(true);
	pCircle_0->Set_Visible(true);
	 /* 초기 객체 위치 초기화*/

	_vector vPos = m_pOwner->Get_Component<CCharacter_Controller>()->Get_HeadPosition();


	pDistortion->Get_Component<CTransform>()->Set_State(STATE::POSITION, vPos);
	pDistortion_Small->Get_Component<CTransform>()->Set_State(STATE::POSITION, vPos);
	pPulse_White->Get_Component<CTransform>()->Set_State(STATE::POSITION, vPos);
	pSplash_Black->Get_Component<CTransform>()->Set_State(STATE::POSITION, vPos);
	pSplash_Red->Get_Component<CTransform>()->Set_State(STATE::POSITION, vPos);
	pSpline0->Get_Component<CTransform>()->Set_State(STATE::POSITION, vPos);
	pSpline1->Get_Component<CTransform>()->Set_State(STATE::POSITION, vPos);
	pSide_Pt->Get_Component<CTransform>()->Set_State(STATE::POSITION, vPos);
	pCircle_0->Get_Component<CTransform>()->Set_State(STATE::POSITION, vPos);

	return S_OK;
}

HRESULT CRanrok_DeadImpact::Ready_Components(void* pArg)
{
	if (FAILED(__super::Ready_Components(pArg))) {
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CRanrok_DeadImpact::Ready_Child()
{
	return S_OK;
}

CRanrok_DeadImpact* CRanrok_DeadImpact::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CRanrok_DeadImpact* pInstance = new CRanrok_DeadImpact(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CRanrok_DeadImpact");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}


CGameObject* CRanrok_DeadImpact::Clone(void* pArg, CGameObject* pOwner)
{
	CRanrok_DeadImpact* pInstance = new CRanrok_DeadImpact(*this);
	pInstance->m_pOwner = pOwner;
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CRanrok_DeadImpact");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

void CRanrok_DeadImpact::OnCollision(CGameObject* pOther, void* pDesc)
{
	if (m_bHit == false)
		return;

	ON_COLLISION_INFO CollisionDesc = *static_cast<ON_COLLISION_INFO*>(pDesc);

	_vector vPos = XMLoadFloat4(&CollisionDesc.vWorldPos);


}

void CRanrok_DeadImpact::Free()
{
	__super::Free();

	SAFE_RELEASE(m_pInfoInstance);


}
#ifdef _DEBUG
void CRanrok_DeadImpact::Describe_Entity()
{

}
#endif

HRESULT CRanrok_DeadImpact::Bind_ShaderResources()
{
	return S_OK;
}


