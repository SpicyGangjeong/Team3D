#include "pch.h"
#include "Duelist_Levioso.h"

#include "GameInstance.h"
#include "EffectParts.h"
#include "Human_Duelist.h"
#include "TrailObject.h"
#include "Wand.h"
#include "InfoInstance.h"


CDuelist_Levioso::CDuelist_Levioso(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CEffect_Container{ pDevice, pContext }
{
}

CDuelist_Levioso::CDuelist_Levioso(const CDuelist_Levioso& rhs)
	: CEffect_Container(rhs),
	m_pInfoInstance(CInfoInstance::GetInstance())
{
}

HRESULT CDuelist_Levioso::Initialize_Prototype()
{

	if (FAILED(Load_Package("../Bin/Resources/Data/Effect/Package/Levioso")))
		return E_FAIL;

	return S_OK;

}

HRESULT CDuelist_Levioso::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components(pArg)))
		return E_FAIL;

	if (FAILED(Create_Effect()))
		return E_FAIL;

	m_iSkillType = ENUM_CLASS(SKILL_TYPE::DUELIST_LEVIOSO);

	m_pLeviosoPJ = Get_PartObject<CEffectParts>("Levioso_PJ");

	SAFE_ADDREF(m_pLeviosoPJ);

	m_wstrEffectName = L"Levioso";


	m_fDuration = 6.f;



	return S_OK;
}

void CDuelist_Levioso::Priority_Update(_float fTimeDelta)
{
	__super::Priority_Update(fTimeDelta);

	XMStoreFloat4(&m_vStartPos, m_pLeviosoPJ->Get_WorldPostion());

}

void CDuelist_Levioso::Update(_float fTimeDelta)
{
	if (m_bVisible == false)
		return;

	__super::Update(fTimeDelta);

	Update_Event(fTimeDelta);


	m_pLeviosoPJ->Get_Component<CTransform>()->Translation(XMLoadFloat3(&m_vCameraLook) * m_fLinearSpeed);

}

void CDuelist_Levioso::Late_Update(_float fTimeDelta)
{
	if (m_bVisible == false)
		return;

	XMStoreFloat4(&m_vEndPos, m_pLeviosoPJ->Get_WorldPostion());

	_vector vDir = XMLoadFloat4(&m_vEndPos) - XMLoadFloat4(&m_vStartPos);

	if (false == m_bHit) {

		_vector vStartPos = XMLoadFloat4(&m_vStartPos);
		_vector vEndPos = XMLoadFloat4(&m_vEndPos);

		if (false == m_bHit) {
			_vector vStartPos = XMLoadFloat4(&m_vStartPos);
			_vector vEndPos = XMLoadFloat4(&m_vEndPos);
			if (false == XMVector3NearEqual(vEndPos, XMVectorZero(), XMVectorReplicate(FLT_EPSILON5)))
			{
				ON_COLLISION_INFO CollisionInfo = MonsterSweepTarget(vStartPos, vEndPos, 0.02f);
				OnCollision(this, &CollisionInfo);
			}
		}
	}

	__super::Late_Update(fTimeDelta);
}

HRESULT CDuelist_Levioso::Pre_Setting(CGameObject* pObject, void* pArg)
{

	if (FAILED(__super::Pre_Setting(pObject, nullptr)))
		return E_FAIL;

	CHuman_Duelist* pDuelist = static_cast<CHuman_Duelist*>(m_pOwner);

	if (pDuelist == nullptr)
		return E_FAIL;

	_vector WandPos = pDuelist->Get_WandPos().r[3];

	CEffectParts* pWandSmoke = Get_PartObject<CEffectParts>("Wand_Smoke");
	CEffectParts* pWandLight = Get_PartObject<CEffectParts>("Wand_Light_P");
	CEffectParts* pSpread_Circle = Get_PartObject<CEffectParts>("Spread_Circle");


	m_pLeviosoPJ->Get_Component<CTransform>()->Set_State(STATE::POSITION, WandPos);

	pWandSmoke->Get_Component<CTransform>()->Set_State(STATE::POSITION, pDuelist->Get_PartObject<CWand>()->Get_WorldPostion());
	pWandLight->Get_Component<CTransform>()->Set_State(STATE::POSITION, pDuelist->Get_PartObject<CWand>()->Get_WorldPostion());
	pSpread_Circle->Get_Component<CTransform>()->Set_State(STATE::POSITION, pDuelist->Get_PartObject<CWand>()->Get_WorldPostion());

	m_pLeviosoPJ->Set_Visible(true);
	pWandSmoke->Set_Visible(true);
	pWandLight->Set_Visible(true);
	pSpread_Circle->Set_Visible(true);

	XMStoreFloat3(&m_vCameraLook, XMVector3Normalize(m_pOwner->Get_Component<CTransform>()->Get_State(STATE::LOOK)));

	_vector vDirection = m_pOwner->Get_Component<CTransform>()->Get_State(STATE::LOOK);
	XMStoreFloat3(&m_vCameraLook, vDirection);

	_vector vStartPos = WandPos;
	XMStoreFloat4(&m_vStartPos, vStartPos);

	{ /* 대상 위치 지정 */

		CUnit* pPlayer = m_pInfoInstance->Get_NearestPlayerAlly(m_pOwner->Get_WorldPostion()).first;

		if (pPlayer == nullptr)
			return E_FAIL;

		_vector vPlayerPos = pPlayer->Get_Component<CCharacter_Controller>()->Get_Position();

		if (nullptr != pPlayer) {
			XMStoreFloat4(&m_vTargetPos, vPlayerPos);

			XMStoreFloat3(&m_vCameraLook, XMVector3Normalize(XMLoadFloat4(&m_vTargetPos) - XMLoadFloat4(&m_vStartPos)));
		}
		else {
			// 타겟이 없다면 현재위치 -> 카메라 룩벡터 * duration간 예상 이동거리 를 대상으로 지정
			XMStoreFloat4(&m_vTargetPos, vStartPos + vDirection * m_fLinearSpeed * 0.5f);
		}

	}


	return S_OK;
}

HRESULT CDuelist_Levioso::Ready_Components(void* pArg)
{
	if (FAILED(__super::Ready_Components(pArg))) {
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CDuelist_Levioso::Ready_Child()
{

	return S_OK;
}

CDuelist_Levioso* CDuelist_Levioso::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CDuelist_Levioso* pInstance = new CDuelist_Levioso(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CDuelist_Levioso");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}


CGameObject* CDuelist_Levioso::Clone(void* pArg, CGameObject* pOwner)
{
	CDuelist_Levioso* pInstance = new CDuelist_Levioso(*this);
	pInstance->m_pOwner = pOwner;
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CDuelist_Levioso");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

void CDuelist_Levioso::OnCollision(CGameObject* pOther, void* pDesc)
{
	if (m_bHit == false)
		return;



	ON_COLLISION_INFO CollisionDesc = *static_cast<ON_COLLISION_INFO*>(pDesc);

	_vector vPos = XMLoadFloat4(&CollisionDesc.vWorldPos);

	m_fAccTime = 0.f;
	m_fDuration = 3.5f; //적중하면 지속시간 3초

	_vector vHitPos = XMLoadFloat4(&CollisionDesc.vWorldPos);


	/* 바람과 파티클은 풋포지션 */
	CCharacter_Controller* pHitCCT = CollisionDesc.pObject->Get_Component<CCharacter_Controller>();

	if (pHitCCT != nullptr)
	{
		_vector vFootPos = pHitCCT->Get_FootPosition();

		CEffectParts* pLevioso_Tornado = Get_PartObject<CEffectParts>("Levioso_Tornado");
		CEffectParts* pLevioso_Rotate0 = Get_PartObject<CEffectParts>("Levioso_Rotate0");
		CEffectParts* pLevioso_Rotate1 = Get_PartObject<CEffectParts>("Levioso_Rotate1");
		CEffectParts* pLevioso_Bottom_PT = Get_PartObject<CEffectParts>("Levioso_Bottom_PT");
		CEffectParts* pLevioso_Spline = Get_PartObject<CEffectParts>("Levioso_Spline");
		CEffectParts* pLevioso_Hit0 = Get_PartObject<CEffectParts>("Levioso_Hit0");

		pLevioso_Tornado->Get_Component<CTransform>()->Set_State(STATE::POSITION, vFootPos);
		pLevioso_Rotate0->Get_Component<CTransform>()->Set_State(STATE::POSITION, vFootPos);
		pLevioso_Rotate1->Get_Component<CTransform>()->Set_State(STATE::POSITION, vFootPos);
		pLevioso_Bottom_PT->Get_Component<CTransform>()->Set_State(STATE::POSITION, vFootPos);

		pLevioso_Hit0->Get_Component<CTransform>()->Set_State(STATE::POSITION, vHitPos);
		pLevioso_Spline->Get_Component<CTransform>()->Set_State(STATE::POSITION, vHitPos);

		pLevioso_Hit0->Set_Visible(true);
		pLevioso_Spline->Set_Visible(true);

		pLevioso_Tornado->Set_Visible(true);
		pLevioso_Rotate0->Set_Visible(true);
		pLevioso_Rotate1->Set_Visible(true);
		pLevioso_Bottom_PT->Set_Visible(true);
		pLevioso_Spline->Set_Visible(true);
	}



}

void CDuelist_Levioso::Free()
{
	__super::Free();

	SAFE_RELEASE(m_pLeviosoPJ);
}
#ifdef _DEBUG

void CDuelist_Levioso::Describe_Entity()
{
}

#endif // _DEBUG

HRESULT CDuelist_Levioso::Bind_ShaderResources()
{
	return S_OK;
}


