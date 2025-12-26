#include "pch.h"
#include "Mage_Nomal_Attack.h"

#include "GameInstance.h"
#include "EffectParts.h"
#include "Wand.h"
#include "Player.h"
#include "InfoInstance.h"

CMage_Nomal_Attack::CMage_Nomal_Attack(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CEffect_Container{ pDevice, pContext }
{
}

CMage_Nomal_Attack::CMage_Nomal_Attack(const CMage_Nomal_Attack& rhs)
	: CEffect_Container(rhs),
	m_pInfoInstance(CInfoInstance::GetInstance())
{
	SAFE_ADDREF(m_pInfoInstance);
}

HRESULT CMage_Nomal_Attack::Initialize_Prototype()
{
	if (FAILED(Load_Package("../Bin/Resources/Data/Effect/MonsterPackage/GoblinMage/NomalAttack")))
		return E_FAIL;

	return S_OK;

}

HRESULT CMage_Nomal_Attack::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components(pArg)))
		return E_FAIL;

	if (FAILED(Create_Effect()))
		return E_FAIL;


	m_wstrEffectName = L"Mage_Nomal_Attack";

	m_pCircle_Proj = Get_PartObject<CEffectParts>("Circle_Proj");
	m_pProj_PT1 = Get_PartObject<CEffectParts>("Proj_PT");
	m_pProj_PT2 = Get_PartObject<CEffectParts>("Proj_PT2");

	SAFE_ADDREF(m_pCircle_Proj);
	SAFE_ADDREF(m_pProj_PT1);
	SAFE_ADDREF(m_pProj_PT2);

	m_fDuration = 3.2f;

	return S_OK;
}

void CMage_Nomal_Attack::Priority_Update(_float fTimeDelta)
{
	__super::Priority_Update(fTimeDelta);

	XMStoreFloat4(&m_vStartPos, m_pCircle_Proj->Get_WorldPostion());
}

void CMage_Nomal_Attack::Update(_float fTimeDelta)
{
	if (m_bVisible == false)
		return;

	__super::Update(fTimeDelta);

	Update_Event(fTimeDelta);


	m_pCircle_Proj->Get_Component<CTransform>()->Translation(XMLoadFloat3(&m_vLook) * m_fLinearSpeed);
	m_pProj_PT1->Get_Component<CTransform>()->Translation(XMLoadFloat3(&m_vLook) * m_fLinearSpeed);
	m_pProj_PT2->Get_Component<CTransform>()->Translation(XMLoadFloat3(&m_vLook) * m_fLinearSpeed);

	if (false == m_bHit) {
		_vector vStartPos = XMLoadFloat4(&m_vStartPos);
		_vector vEndPos = m_pCircle_Proj->Get_WorldPostion();
		ON_COLLISION_INFO CollisionInfo = MonsterSweepTarget(vStartPos, vEndPos, 0.2f);

		OnCollision(this, &CollisionInfo);
	}
	
}

void CMage_Nomal_Attack::Late_Update(_float fTimeDelta)
{
	if (m_bVisible == false)
		return;

	__super::Late_Update(fTimeDelta);


}

HRESULT CMage_Nomal_Attack::Pre_Setting(CGameObject* pObject, void* pArg)
{
	if (FAILED(__super::Pre_Setting(pObject, nullptr)))
		return E_FAIL;


	/* 초기 객체 비지블*/
	m_pCircle_Proj->Set_Visible(true);
	m_pProj_PT1->Set_Visible(true);
	m_pProj_PT2->Set_Visible(true);

	/* 초기 객체 위치 초기화*/

	_matrix BoneMat = XMLoadFloat4x4(m_pOwner->Get_Component<CModel>()->Get_BoneMatrixPtr("RightHand"));

	for (int i = 0; i < 3; ++i) {
		BoneMat.r[i] = XMVector3Normalize(BoneMat.r[i]);
	}

	_matrix WorldMat = m_pOwner->Get_Component<CTransform>()->Get_XMWorldMatrix();

	_matrix CombinedMat = BoneMat * WorldMat;

	_vector vStartPos = CombinedMat.r[3];

	 XMStoreFloat4(&m_vStartPos, vStartPos);

	 m_pCircle_Proj->Get_Component<CTransform>()->Set_State(STATE::POSITION, vStartPos);
	 m_pProj_PT1->Get_Component<CTransform>()->Set_State(STATE::POSITION, vStartPos);
	 m_pProj_PT2->Get_Component<CTransform>()->Set_State(STATE::POSITION, vStartPos);



	 /* 플레이어 위치 받기 */
	 CUnit* pPlayer = m_pInfoInstance->Get_NearestPlayerAlly(m_pOwner->Get_WorldPostion()).first;

	 if (pPlayer == nullptr)
		 return E_FAIL;

	 /* 초기 방향 */
	 XMStoreFloat4(&m_vTargetPos, pPlayer->Get_Component<CCharacter_Controller>()->Get_Position());
	 XMStoreFloat3(&m_vLook, XMVector3Normalize(XMLoadFloat4(&m_vTargetPos) - XMLoadFloat4(&m_vStartPos)));


	return S_OK;
}

HRESULT CMage_Nomal_Attack::Ready_Components(void* pArg)
{
	if (FAILED(__super::Ready_Components(pArg))) {
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CMage_Nomal_Attack::Ready_Child()
{
	return S_OK;
}

CMage_Nomal_Attack* CMage_Nomal_Attack::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CMage_Nomal_Attack* pInstance = new CMage_Nomal_Attack(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CMage_Nomal_Attack");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}


CGameObject* CMage_Nomal_Attack::Clone(void* pArg, CGameObject* pOwner)
{
	CMage_Nomal_Attack* pInstance = new CMage_Nomal_Attack(*this);
	pInstance->m_pOwner = pOwner;
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CMage_Nomal_Attack");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

void CMage_Nomal_Attack::OnCollision(CGameObject* pOther, void* pDesc)
{
	if (m_bHit == false)
		return;

	ON_COLLISION_INFO CollisionDesc = *static_cast<ON_COLLISION_INFO*>(pDesc);

	_vector vPos = XMLoadFloat4(&CollisionDesc.vWorldPos);


	for (auto& pPair : m_PartObjects)
	{
		pPair.second->Set_Visible(true);
		pPair.second->Get_Component<CTransform>()->Set_State(STATE::POSITION, vPos);
	}


	m_pCircle_Proj->Set_Visible(false);
	m_pProj_PT1->Set_Visible(false);
	m_pProj_PT2->Set_Visible(false);


	CPlayer* pPlayer = static_cast<CPlayer*>(m_pInfoInstance->Get_NearestPlayerAlly(m_pOwner->Get_WorldPostion()).first);


	if (pPlayer == nullptr)
		return;

	pPlayer->Start_CameraShake(0.3f, 2.f);

}

void CMage_Nomal_Attack::Free()
{
	__super::Free();

	SAFE_RELEASE(m_pInfoInstance);

	SAFE_RELEASE(m_pCircle_Proj);
	SAFE_RELEASE(m_pProj_PT1);
	SAFE_RELEASE(m_pProj_PT2);
}
#ifdef _DEBUG
void CMage_Nomal_Attack::Describe_Entity()
{

}
#endif

HRESULT CMage_Nomal_Attack::Bind_ShaderResources()
{
	return S_OK;
}


