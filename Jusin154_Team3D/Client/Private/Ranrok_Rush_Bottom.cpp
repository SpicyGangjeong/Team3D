#include "pch.h"
#include "Ranrok_Rush_Bottom.h"

#include "GameInstance.h"
#include "EffectParts.h"
#include "Wand.h"
#include "Player.h"
#include "InfoInstance.h"

CRanrok_Rush_Bottom::CRanrok_Rush_Bottom(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CEffect_Container{ pDevice, pContext }
{
}

CRanrok_Rush_Bottom::CRanrok_Rush_Bottom(const CRanrok_Rush_Bottom& rhs)
	: CEffect_Container(rhs),
	m_pInfoInstance(CInfoInstance::GetInstance())
{
	SAFE_ADDREF(m_pInfoInstance);
}

HRESULT CRanrok_Rush_Bottom::Initialize_Prototype()
{


	if (FAILED(Load_Package("../Bin/Resources/Data/Effect/MonsterPackage/Ranrok/RanrokRushBottom")))
		return E_FAIL;

	return S_OK;

}

HRESULT CRanrok_Rush_Bottom::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components(pArg)))
		return E_FAIL;

	if (FAILED(Create_Effect()))
		return E_FAIL;


	m_wstrEffectName = L"Ranrok_Rush_Bottom";



	m_fDuration = 3.f;
	

	return S_OK;
}

void CRanrok_Rush_Bottom::Priority_Update(_float fTimeDelta)
{
	__super::Priority_Update(fTimeDelta);

}

void CRanrok_Rush_Bottom::Update(_float fTimeDelta)
{
	if (m_bVisible == false)
		return;

	__super::Update(fTimeDelta);

	Update_Event(fTimeDelta);




	
}

void CRanrok_Rush_Bottom::Late_Update(_float fTimeDelta)
{
	if (m_bVisible == false)
		return;

	__super::Late_Update(fTimeDelta);


}

HRESULT CRanrok_Rush_Bottom::Pre_Setting(CGameObject* pObject, void* pArg)
{
	if (FAILED(__super::Pre_Setting(pObject, nullptr)))
		return E_FAIL;


	 /* 초기 객체 비지블*/

	CEffectParts* pRush_Decal = Get_PartObject<CEffectParts>("Rush_Decal");
	CEffectParts* pRock_PT = Get_PartObject<CEffectParts>("Rock_PT");
	CEffectParts* pSmoke = Get_PartObject<CEffectParts>("Smoke");


	pRush_Decal->Set_Visible(true);
	pSmoke->Set_Visible(true);
	pRock_PT->Set_Visible(true);


	 /* 초기 객체 위치 초기화*/


	_vector vBonePos = XMLoadFloat4(static_cast<_float4*>(pArg));


	CUnit* pPlayer = m_pInfoInstance->Get_NearestPlayerAlly(m_pOwner->Get_WorldPostion()).first;

	if (pPlayer == nullptr)
		return E_FAIL;

	_vector vPlayer_FootPos = pPlayer->Get_Component<CCharacter_Controller>()->Get_FootPosition();

	XMStoreFloat4(&m_vTargetPos, vPlayer_FootPos);


	_vector vCompute_Pos = XMVectorSetY(vBonePos, XMVectorGetY(vPlayer_FootPos));

	pRush_Decal->Get_Component<CTransform>()->Set_State(STATE::POSITION, vCompute_Pos);
	pSmoke->Get_Component<CTransform>()->Set_State(STATE::POSITION, vCompute_Pos);
	pRock_PT->Get_Component<CTransform>()->Set_State(STATE::POSITION, vCompute_Pos);

	return S_OK;
}

HRESULT CRanrok_Rush_Bottom::Ready_Components(void* pArg)
{
	if (FAILED(__super::Ready_Components(pArg))) {
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CRanrok_Rush_Bottom::Ready_Child()
{
	return S_OK;
}

CRanrok_Rush_Bottom* CRanrok_Rush_Bottom::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CRanrok_Rush_Bottom* pInstance = new CRanrok_Rush_Bottom(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CRanrok_Rush_Bottom");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}


CGameObject* CRanrok_Rush_Bottom::Clone(void* pArg, CGameObject* pOwner)
{
	CRanrok_Rush_Bottom* pInstance = new CRanrok_Rush_Bottom(*this);
	pInstance->m_pOwner = pOwner;
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CRanrok_Rush_Bottom");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

void CRanrok_Rush_Bottom::OnCollision(CGameObject* pOther, void* pDesc)
{
	if (m_bHit == false)
		return;

	ON_COLLISION_INFO CollisionDesc = *static_cast<ON_COLLISION_INFO*>(pDesc);

	_vector vPos = XMLoadFloat4(&CollisionDesc.vWorldPos);


}

void CRanrok_Rush_Bottom::Free()
{
	__super::Free();

	SAFE_RELEASE(m_pInfoInstance);


}
#ifdef _DEBUG
void CRanrok_Rush_Bottom::Describe_Entity()
{

}
#endif

HRESULT CRanrok_Rush_Bottom::Bind_ShaderResources()
{
	return S_OK;
}


