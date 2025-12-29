#include "pch.h"
#include "Ranrok_FireBall.h"

#include "GameInstance.h"
#include "EffectParts.h"
#include "Wand.h"
#include "Player.h"
#include "InfoInstance.h"

CRanrok_FireBall::CRanrok_FireBall(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CEffect_Container{ pDevice, pContext }
{
}

CRanrok_FireBall::CRanrok_FireBall(const CRanrok_FireBall& rhs)
	: CEffect_Container(rhs),
	m_pInfoInstance(CInfoInstance::GetInstance())
{
	SAFE_ADDREF(m_pInfoInstance);
}

HRESULT CRanrok_FireBall::Initialize_Prototype()
{

	if (FAILED(Load_Package("../Bin/Resources/Data/Effect/MonsterPackage/Ranrok/RanrokFireBall")))
		return E_FAIL;

	return S_OK;

}

HRESULT CRanrok_FireBall::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components(pArg)))
		return E_FAIL;

	if (FAILED(Create_Effect()))
		return E_FAIL;


	m_wstrEffectName = L"RanrokFireBall";


	m_pFireBall = Get_PartObject<CEffectParts>("FireBall");
	m_pFireBall_PT = Get_PartObject<CEffectParts>("FireBall_PT");
	m_pFireBall_Surface = Get_PartObject<CEffectParts>("FireBall_Surface");


	SAFE_ADDREF(m_pFireBall);
	SAFE_ADDREF(m_pFireBall_PT);
	SAFE_ADDREF(m_pFireBall_Surface);

	m_fDuration = 4.f;


	return S_OK;
}

void CRanrok_FireBall::Priority_Update(_float fTimeDelta)
{
	__super::Priority_Update(fTimeDelta);

	XMStoreFloat4(&m_vStartPos, m_pFireBall->Get_WorldPostion());
}

void CRanrok_FireBall::Update(_float fTimeDelta)
{
	if (m_bVisible == false)
		return;

	__super::Update(fTimeDelta);

	Update_Event(fTimeDelta);


	m_pFireBall->Get_Component<CTransform>()->Translation(XMLoadFloat3(&m_vLook) * m_fLinearSpeed);
	m_pFireBall_PT->Get_Component<CTransform>()->Translation(XMLoadFloat3(&m_vLook) * m_fLinearSpeed);
	m_pFireBall_Surface->Get_Component<CTransform>()->Translation(XMLoadFloat3(&m_vLook) * m_fLinearSpeed);
	//m_pFireBall_Tail->Get_Component<CTransform>()->Translation(XMLoadFloat3(&m_vLook) * m_fLinearSpeed);

	if (false == m_bHit) {
		_vector vStartPos = XMLoadFloat4(&m_vStartPos);
		_vector vEndPos = m_pFireBall->Get_WorldPostion();
		ON_COLLISION_INFO CollisionInfo = MonsterSweepTarget(vStartPos, vEndPos, 0.2f, true);
		OnCollision(this, &CollisionInfo);
	}

}

void CRanrok_FireBall::Late_Update(_float fTimeDelta)
{
	if (m_bVisible == false)
		return;

	__super::Late_Update(fTimeDelta);


}

HRESULT CRanrok_FireBall::Pre_Setting(CGameObject* pObject, void* pArg)
{
	if (FAILED(__super::Pre_Setting(pObject, nullptr)))
		return E_FAIL;

	CEffectParts* pBlack_Smoke = Get_PartObject<CEffectParts>("Black_Smoke");
	CEffectParts* pFireBall_Ring = Get_PartObject<CEffectParts>("FireBall_Ring");

	/* 초기 객체 비지블*/
	m_pFireBall->Set_Visible(true);
	m_pFireBall_PT->Set_Visible(true);
	m_pFireBall_Surface->Set_Visible(true);

	pBlack_Smoke->Set_Visible(true);
	pFireBall_Ring->Set_Visible(true);

	/* 초기 객체 위치 초기화*/

	_matrix BoneMat = XMLoadFloat4x4(m_pOwner->Get_Component<CModel>()->Get_BoneMatrixPtr("jaw"));

	for (int i = 0; i < 3; ++i) {
		BoneMat.r[i] = XMVector3Normalize(BoneMat.r[i]);
	}

	_matrix WorldMat = m_pOwner->Get_Component<CTransform>()->Get_XMWorldMatrix();

	_matrix CombinedMat = BoneMat * WorldMat;

	_vector vStartPos = CombinedMat.r[3];

	XMStoreFloat4(&m_vStartPos, vStartPos);

	/* 플레이어 위치 받기 */
	 CUnit* pPlayer = m_pInfoInstance->Get_NearestPlayerAlly(m_pOwner->Get_WorldPostion()).first;

	if (pPlayer == nullptr)
		return E_FAIL;

	/* 초기 방향 */
	XMStoreFloat4(&m_vTargetPos, pPlayer->Get_Component<CCharacter_Controller>()->Get_Position());
	XMStoreFloat3(&m_vLook, XMVector3Normalize(XMLoadFloat4(&m_vTargetPos) - XMLoadFloat4(&m_vStartPos)));

	m_pFireBall_PT->Get_Component<CTransform>()->Set_State(STATE::POSITION, vStartPos);
	//m_pFireBall_Tail->Get_Component<CTransform>()->Set_State(STATE::POSITION, vStartPos - XMLoadFloat3(&m_vLook) * 5.f);
	pBlack_Smoke->Get_Component<CTransform>()->Set_State(STATE::POSITION, vStartPos);
	pFireBall_Ring->Get_Component<CTransform>()->Set_State(STATE::POSITION, vStartPos);
	/* PJ 월드 구성하기  */


	_vector vLook = XMVector3Normalize(XMLoadFloat3(&m_vLook));
	_vector vRight = XMVector3Normalize(XMVector3Cross(XMVectorSet(0.f, 1.f, 0.f, 0.f), vLook));
	_vector vUp = XMVector3Normalize(XMVector3Cross(vLook, vRight));
	_vector vPos = vStartPos;

	_matrix PJ_WorldMat = { vRight , vUp ,vLook , vPos };

	m_pFireBall->Get_Component<CTransform>()->Set_WorldMatrix(PJ_WorldMat);
	m_pFireBall_Surface->Get_Component<CTransform>()->Set_WorldMatrix(PJ_WorldMat);

	return S_OK;
}

HRESULT CRanrok_FireBall::Ready_Components(void* pArg)
{
	if (FAILED(__super::Ready_Components(pArg))) {
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CRanrok_FireBall::Ready_Child()
{
	return S_OK;
}

CRanrok_FireBall* CRanrok_FireBall::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CRanrok_FireBall* pInstance = new CRanrok_FireBall(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CRanrok_FireBall");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}


CGameObject* CRanrok_FireBall::Clone(void* pArg, CGameObject* pOwner)
{
	CRanrok_FireBall* pInstance = new CRanrok_FireBall(*this);
	pInstance->m_pOwner = pOwner;
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CRanrok_FireBall");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

void CRanrok_FireBall::OnCollision(CGameObject* pOther, void* pDesc)
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


	m_pFireBall->Set_Visible(false);
	m_pFireBall_Surface->Set_Visible(false);

	//m_pFireBall_Tail->Get_Component<CTransform>()->Set_State(STATE::POSITION, XMVectorSet(0.f, -500.f, 0.f, 1.f));
	m_pFireBall_PT->Get_Component<CTransform>()->Set_State(STATE::POSITION, XMVectorSet(0.f, -500.f, 0.f, 1.f));


	CEffectParts* pBlack_Smoke = Get_PartObject<CEffectParts>("Black_Smoke");

	pBlack_Smoke->Set_Visible(false);

	CPlayer* pPlayer = static_cast<CPlayer*>(m_pInfoInstance->Get_NearestPlayerAlly(m_pOwner->Get_WorldPostion()).first);


	if (pPlayer == nullptr)
		return;

	pPlayer->Start_CameraShake(0.3f, 2.f);

}

void CRanrok_FireBall::Free()
{
	__super::Free();

	SAFE_RELEASE(m_pInfoInstance);

	SAFE_RELEASE(m_pFireBall);
	SAFE_RELEASE(m_pFireBall_PT);
	SAFE_RELEASE(m_pFireBall_Surface);
	//SAFE_RELEASE(m_pFireBall_Tail);
}
#ifdef _DEBUG
void CRanrok_FireBall::Describe_Entity()
{

}
#endif

HRESULT CRanrok_FireBall::Bind_ShaderResources()
{
	return S_OK;
}


