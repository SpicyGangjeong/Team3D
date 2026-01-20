#include "pch.h"
#include "Ranrok_DeadSplash.h"

#include "GameInstance.h"
#include "EditEffect.h"
#include "Wand.h"
#include "Player.h"
#include "InfoInstance.h"

CRanrok_DeadSplash::CRanrok_DeadSplash(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CEffect_Container{ pDevice, pContext }
{
}

CRanrok_DeadSplash::CRanrok_DeadSplash(const CRanrok_DeadSplash& rhs)
	: CEffect_Container(rhs),
	m_pInfoInstance(CInfoInstance::GetInstance())
{
	SAFE_ADDREF(m_pInfoInstance);
}

HRESULT CRanrok_DeadSplash::Initialize_Prototype()
{


	return S_OK;

}

HRESULT CRanrok_DeadSplash::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components(pArg)))
		return E_FAIL;


	if (FAILED(Load_Package("../Bin/Resources/Data/Effect/MonsterPackage/Ranrok/RanrokDeadSplash")))
		return E_FAIL;

	m_wstrEffectName = L"RanrokDeadSplash";



	m_fDuration = 5.f;
	

	return S_OK;
}

void CRanrok_DeadSplash::Priority_Update(_float fTimeDelta)
{
	__super::Priority_Update(fTimeDelta);

}

void CRanrok_DeadSplash::Update(_float fTimeDelta)
{
	if (m_bVisible == false)
		return;

	__super::Update(fTimeDelta);

	Update_Event(fTimeDelta);




	
}

void CRanrok_DeadSplash::Late_Update(_float fTimeDelta)
{
	if (m_bVisible == false)
		return;

	__super::Late_Update(fTimeDelta);


}

HRESULT CRanrok_DeadSplash::Pre_Setting(CGameObject* pObject, void* pArg)
{
	if (FAILED(__super::Pre_Setting(pObject, nullptr)))
		return E_FAIL;


	_float4 vPos = *static_cast<_float4*>(pArg);
	_vector vPosition = XMLoadFloat4(&vPos);
	 /* 초기 객체 비지블*/

	CEditEffect* pDead_Decal = Get_PartObject<CEditEffect>("Dead_Decal");
	CEditEffect* pRock_PT = Get_PartObject<CEditEffect>("Rock_PT");
	CEditEffect* pRock_Splash = Get_PartObject<CEditEffect>("Rock_Splash");
	CEditEffect* pSplash = Get_PartObject<CEditEffect>("Splash");

	pDead_Decal->Set_Visible(true);
	pRock_PT->Set_Visible(true);

	pRock_Splash->Set_Visible(true);
	pSplash->Set_Visible(true);

	 /* 초기 객체 위치 초기화*/

	pRock_PT->Get_Component<CTransform>()->Set_State(STATE::POSITION, vPosition);
	pSplash->Get_Component<CTransform>()->Set_State(STATE::POSITION, vPosition);

	CUnit* pPlayer = m_pInfoInstance->Get_NearestPlayerAlly(m_pOwner->Get_WorldPostion()).first;

	if (pPlayer == nullptr)
		return E_FAIL;

	_vector vPlayer_FootPos = pPlayer->Get_Component<CCharacter_Controller>()->Get_FootPosition();


	_vector vCompute_Pos = XMVectorSetY(vPosition, XMVectorGetY(vPlayer_FootPos));
	pRock_Splash->Get_Component<CTransform>()->Set_State(STATE::POSITION, vCompute_Pos);

	vCompute_Pos = XMVectorSetY(m_pOwner->Get_WorldPostion(), XMVectorGetY(vPlayer_FootPos));

	pDead_Decal->Get_Component<CTransform>()->Set_State(STATE::POSITION, vCompute_Pos);

	return S_OK;
}

HRESULT CRanrok_DeadSplash::Ready_Components(void* pArg)
{
	if (FAILED(__super::Ready_Components(pArg))) {
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CRanrok_DeadSplash::Ready_Child()
{
	return S_OK;
}

CRanrok_DeadSplash* CRanrok_DeadSplash::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CRanrok_DeadSplash* pInstance = new CRanrok_DeadSplash(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CRanrok_DeadSplash");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}


CGameObject* CRanrok_DeadSplash::Clone(void* pArg, CGameObject* pOwner)
{
	CRanrok_DeadSplash* pInstance = new CRanrok_DeadSplash(*this);
	pInstance->m_pOwner = pOwner;
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CRanrok_DeadSplash");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

void CRanrok_DeadSplash::OnCollision(CGameObject* pOther, void* pDesc)
{
	if (m_bHit == false)
		return;

	ON_COLLISION_INFO CollisionDesc = *static_cast<ON_COLLISION_INFO*>(pDesc);

	_vector vPos = XMLoadFloat4(&CollisionDesc.vWorldPos);


}

void CRanrok_DeadSplash::Free()
{
	__super::Free();

	SAFE_RELEASE(m_pInfoInstance);


}
#ifdef _DEBUG
void CRanrok_DeadSplash::Describe_Entity()
{

}
#endif

HRESULT CRanrok_DeadSplash::Bind_ShaderResources()
{
	return S_OK;
}


