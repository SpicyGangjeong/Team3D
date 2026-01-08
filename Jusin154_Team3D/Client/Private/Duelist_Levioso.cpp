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

	m_pLeviosoPJ_0 = Get_PartObject<CEffectParts>("Levioso_PJ0");
	m_pTrail_PT_0 = Get_PartObject<CEffectParts>("Trail_PT0");

	m_pLevioso_Rotate0 = Get_PartObject<CEffectParts>("Levioso_Rotate0");
	m_pLevioso_Rotate1 = Get_PartObject<CEffectParts>("Levioso_Rotate1");

	SAFE_ADDREF(m_pLevioso_Rotate0);
	SAFE_ADDREF(m_pLevioso_Rotate1);

	SAFE_ADDREF(m_pLeviosoPJ_0);
	SAFE_ADDREF(m_pTrail_PT_0);

	m_wstrEffectName = L"Levioso";

	m_fTurnSpeed = 10.f;
	m_fRange = 1.f;

	m_fDuration = 2.f;



	return S_OK;
}

void CDuelist_Levioso::Priority_Update(_float fTimeDelta)
{
	__super::Priority_Update(fTimeDelta);

	XMStoreFloat4(&m_vStartPos, m_pLeviosoPJ_0->Get_WorldPostion());

}

void CDuelist_Levioso::Update(_float fTimeDelta)
{
	if (m_bVisible == false)
		return;

	__super::Update(fTimeDelta);

	Update_Event(fTimeDelta);

	m_pLeviosoPJ_0->Get_Component<CTransform>()->Translation(XMLoadFloat3(&m_vCameraLook) * m_fLinearSpeed);
	m_pTrail_PT_0->Get_Component<CTransform>()->Translation(XMLoadFloat3(&m_vCameraLook) * m_fLinearSpeed);


	_matrix WorldMat = m_pTrail_PT_0->Get_Component<CTransform>()->Get_XMWorldMatrix();

	if (m_bHit == true && m_pEnemyTransform != nullptr)
	{
		_vector vPos = m_pEnemyTransform->Get_State(STATE::POSITION);

		m_pLevioso_Rotate0->Get_Component<CTransform>()->Set_State(STATE::POSITION, vPos);
		m_pLevioso_Rotate1->Get_Component<CTransform>()->Set_State(STATE::POSITION, vPos);
	}

}

void CDuelist_Levioso::Late_Update(_float fTimeDelta)
{
	if (m_bVisible == false)
		return;

	XMStoreFloat4(&m_vEndPos, m_pLeviosoPJ_0->Get_WorldPostion());

	_vector vDir = XMLoadFloat4(&m_vEndPos) - XMLoadFloat4(&m_vStartPos);

	if (true == m_pGameInstance->SphereCast(0.25f, XMLoadFloat4(&m_vStartPos), XMVector3Normalize(vDir), XMVectorGetX(XMVector3Length(vDir))
		, PSX::PxHitFlag::ePOSITION | PSX::PxHitFlag::eNORMAL, PSX::PxQueryFlag::eDYNAMIC | PSX::PxQueryFlag::eSTATIC, m_Hitbuffer))
	{

	}

	if (false == m_bHit) {
		_vector vStartPos = XMLoadFloat4(&m_vStartPos);
		_vector vEndPos = XMLoadFloat4(&m_vEndPos);
		ON_COLLISION_INFO CollisionInfo = MonsterSweepTarget(vStartPos, vEndPos, 0.02f);

		OnCollision(this, &CollisionInfo);
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

	CEffectParts* pWandSmoke = Get_PartObject<CEffectParts>("Levioso_Wand0");
	CEffectParts* pWandLight = Get_PartObject<CEffectParts>("Levioso_Wand_Light");

	m_pLeviosoPJ_0->Get_Component<CTransform>()->Set_State(STATE::POSITION, WandPos);
	m_pTrail_PT_0->Get_Component<CTransform>()->Set_State(STATE::POSITION, WandPos);
	pWandSmoke->Get_Component<CTransform>()->Set_State(STATE::POSITION, pDuelist->Get_PartObject<CWand>()->Get_WorldPostion());
	pWandLight->Get_Component<CTransform>()->Set_State(STATE::POSITION, pDuelist->Get_PartObject<CWand>()->Get_WorldPostion());

	m_pLeviosoPJ_0->Set_Visible(true);
	m_pTrail_PT_0->Set_Visible(true);
	pWandSmoke->Set_Visible(true);
	pWandLight->Set_Visible(true);


	m_vOwnerLook = XMVector3Normalize(m_pOwner->Get_Component<CTransform>()->Get_State(STATE::LOOK));

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

	_vector vDir = XMVector3Normalize(XMLoadFloat3(&m_vCameraLook));

	_vector vRight = XMVector3Normalize(XMVector3Cross(XMVectorSet(0.f, 1.f, 0.f, 0.f), vDir));

	_vector vUp = XMVector3Normalize(XMVector3Cross(vRight, vDir));

	_matrix ProjMat = _matrix(vRight, vUp, vDir, vStartPos);

	m_pLeviosoPJ_0->Get_Component<CTransform>()->Set_WorldMatrix(ProjMat);

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


	for (auto& pPair : m_PartObjects)
	{
		pPair.second->Set_Visible(true);
		pPair.second->Get_Component<CTransform>()->Set_State(STATE::POSITION, vPos);
	}

	m_fAccTime = 0.f;
	m_fDuration = 3.5f; //적중하면 지속시간 3초


	/* 바람과 파티클은 풋포지션 */
	CCharacter_Controller* pHitCCT = CollisionDesc.pObject->Get_Component<CCharacter_Controller>();

	if (pHitCCT != nullptr)
	{
		_vector vFootPos = pHitCCT->Get_FootPosition();

		CEffectParts* pLevioso_Tornado = Get_PartObject<CEffectParts>("Levioso_Tornado");
		CEffectParts* pLevioso_Bottom_Wind = Get_PartObject<CEffectParts>("Levioso_Bottom_Wind");
		CEffectParts* pLevioso_Bottom_PT = Get_PartObject<CEffectParts>("Levioso_Bottom_PT");

		m_pEnemyTransform = CollisionDesc.pObject->Get_Component<CTransform>();

		pLevioso_Tornado->Get_Component<CTransform>()->Set_State(STATE::POSITION, vFootPos);
		pLevioso_Bottom_Wind->Get_Component<CTransform>()->Set_State(STATE::POSITION, vFootPos);
		pLevioso_Bottom_PT->Get_Component<CTransform>()->Set_State(STATE::POSITION, vFootPos);

	}



	//Get_PartObject<CEffectParts>("Levioso_Tornado")
	//CEffectParts* pLevioso_Hit =  Get_PartObject<CEffectParts>("Levioso_Hit");
	//CEffectParts* pLevioso_Bottom_Wind = Get_PartObject<CEffectParts>("Levioso_Bottom_Wind");



	m_pLeviosoPJ_0->Set_Visible(false);
	m_pTrail_PT_0->Set_Visible(false);
	//m_pLeviosoTrail->Set_Visible(false);
}

void CDuelist_Levioso::Free()
{
	__super::Free();

	SAFE_RELEASE(m_pLeviosoPJ_0);
	SAFE_RELEASE(m_pTrail_PT_0);

	SAFE_RELEASE(m_pLevioso_Rotate0);
	SAFE_RELEASE(m_pLevioso_Rotate1);
}
#ifdef _DEBUG

void CDuelist_Levioso::Describe_Entity()
{
	GUI::Begin("LEVIOSO", 0, IMGUI_GLOBAL_BEGIN_FLAG);

	GUI::DragFloat("TURN SPEED", &m_fTurnSpeed);
	GUI::DragFloat("RANGE", &m_fRange);
	GUI::End();

}

#endif // _DEBUG

HRESULT CDuelist_Levioso::Bind_ShaderResources()
{
	return S_OK;
}


