#include "pch.h"
#include "Ranrok_Pulse.h"

#include "GameInstance.h"
#include "EffectParts.h"
#include "Wand.h"
#include "Player.h"
#include "InfoInstance.h"

CRanrok_Pulse::CRanrok_Pulse(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CEffect_Container{ pDevice, pContext }
{
}

CRanrok_Pulse::CRanrok_Pulse(const CRanrok_Pulse& rhs)
	: CEffect_Container(rhs),
	m_pInfoInstance(CInfoInstance::GetInstance())
{
	SAFE_ADDREF(m_pInfoInstance);
}

HRESULT CRanrok_Pulse::Initialize_Prototype()
{

	if (FAILED(Load_Package("../Bin/Resources/Data/Effect/MonsterPackage/Ranrok/RanrokPulse")))
		return E_FAIL;

	return S_OK;

}

HRESULT CRanrok_Pulse::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components(pArg)))
		return E_FAIL;

	if (FAILED(Create_Effect()))
		return E_FAIL;

	m_wstrEffectName = L"RanrokPulse";



	m_fDuration = 15.f;
	

	return S_OK;
}

void CRanrok_Pulse::Priority_Update(_float fTimeDelta)
{
	__super::Priority_Update(fTimeDelta);

}

void CRanrok_Pulse::Update(_float fTimeDelta)
{
	if (m_bVisible == false)
		return;

	__super::Update(fTimeDelta);

	Update_Event(fTimeDelta);




	
}

void CRanrok_Pulse::Late_Update(_float fTimeDelta)
{
	if (m_bVisible == false)
		return;

	__super::Late_Update(fTimeDelta);


}

HRESULT CRanrok_Pulse::Pre_Setting(CGameObject* pObject, void* pArg)
{
	if (FAILED(__super::Pre_Setting(pObject, nullptr)))
		return E_FAIL;


	 /* 초기 객체 비지블*/

	CEffectParts* pBottomDecal = Get_PartObject<CEffectParts>("Bottom_Decal");
	CEffectParts* pBottomSplatter2 = Get_PartObject<CEffectParts>("BottomSplatter2");
	CEffectParts* pRed_Sphere = Get_PartObject<CEffectParts>("Red_Sphere");
	CEffectParts* pRock_PT = Get_PartObject<CEffectParts>("Rock_PT");
	CEffectParts* pSmoke = Get_PartObject<CEffectParts>("Smoke");

	pBottomDecal->Set_Visible(true);
	//pBottomSplatter->Set_Visible(true);
	pBottomSplatter2->Set_Visible(true);
	pRock_PT->Set_Visible(true);
	pSmoke->Set_Visible(true);

	pRed_Sphere->Set_Visible(true);


	 /* 초기 객체 위치 초기화*/

	_vector vOwnerPos = m_pOwner->Get_Component<CCharacter_Controller>()->Get_Position();

	pRed_Sphere->Get_Component<CTransform>()->Set_WorldMatrix(m_pOwner->Get_Component<CTransform>()->Get_XMWorldMatrix());
	pRed_Sphere->Get_Component<CTransform>()->Set_State(STATE::POSITION, vOwnerPos);

	CUnit* pPlayer = m_pInfoInstance->Get_NearestPlayerAlly(m_pOwner->Get_WorldPostion()).first;

	if (pPlayer == nullptr)
		return E_FAIL;

	_vector vPlayer_FootPos = pPlayer->Get_Component<CCharacter_Controller>()->Get_FootPosition();

	XMStoreFloat4(&m_vTargetPos, vPlayer_FootPos);

	_vector vCompute_Pos = XMVectorSetY(vOwnerPos, XMVectorGetY(vPlayer_FootPos));

	pBottomDecal->Get_Component<CTransform>()->Set_State(STATE::POSITION, vCompute_Pos);
	pBottomSplatter2->Get_Component<CTransform>()->Set_State(STATE::POSITION, vCompute_Pos);
	pRock_PT->Get_Component<CTransform>()->Set_State(STATE::POSITION, vCompute_Pos);
	pSmoke->Get_Component<CTransform>()->Set_State(STATE::POSITION, vCompute_Pos);


	return S_OK;
}

HRESULT CRanrok_Pulse::Ready_Components(void* pArg)
{
	if (FAILED(__super::Ready_Components(pArg))) {
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CRanrok_Pulse::Ready_Child()
{
	return S_OK;
}

CRanrok_Pulse* CRanrok_Pulse::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CRanrok_Pulse* pInstance = new CRanrok_Pulse(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CRanrok_Pulse");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}


CGameObject* CRanrok_Pulse::Clone(void* pArg, CGameObject* pOwner)
{
	CRanrok_Pulse* pInstance = new CRanrok_Pulse(*this);
	pInstance->m_pOwner = pOwner;
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CRanrok_Pulse");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

void CRanrok_Pulse::OnCollision(CGameObject* pOther, void* pDesc)
{
	if (m_bHit == false)
		return;

	ON_COLLISION_INFO CollisionDesc = *static_cast<ON_COLLISION_INFO*>(pDesc);

	_vector vPos = XMLoadFloat4(&CollisionDesc.vWorldPos);


}

void CRanrok_Pulse::Free()
{
	__super::Free();

	SAFE_RELEASE(m_pInfoInstance);


}
#ifdef _DEBUG
void CRanrok_Pulse::Describe_Entity()
{

}
#endif

HRESULT CRanrok_Pulse::Bind_ShaderResources()
{
	return S_OK;
}


