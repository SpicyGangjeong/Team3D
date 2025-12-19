#include "pch.h"
#include "Ranrak.h"

#include "GameInstance.h"
#include "InfoInstance.h"
#include "Effect_Container.h"
#include "CallBack_Troll_HitReport.h"
#include "EffectParts.h"
#include "EffectPool.h"
#include "Layer.h"
#include "TrailObject.h"
#include "MapElement_Interactable.h"

#pragma region STATE
#include "State_Idle.h"
#include "State_Move.h"
#include "State_Combat.h"
#include "State_Hit.h"
#include "Troll_State_Rush.h"
#include "State_Throw.h"
#pragma endregion

CRanrak::CRanrak(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CMonster(pDevice, pContext)
{
}

CRanrak::CRanrak(const CRanrak& Prototype)
	: CMonster(Prototype)
{
}

HRESULT CRanrak::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CRanrak::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	if (FAILED(Ready_Parts()))
		return E_FAIL;

	Add_FSM();

	Set_Anim();

	{
		CFSM::FSM_DESC FSMDesc{};
		FSMDesc.pStates = &m_States;
		FSMDesc.pStateMask = &m_iStateMask;

		m_pFSM->Bind_States(FSMDesc);
		m_pFSM->Change_State(FSMSTATE::IDLE);
	}

	m_pCallBack_Behavior->Initialize(m_pCharacter_Controller, m_pRigidBody);
	m_pCallBack_HitReport->Initialize(m_pCharacter_Controller, m_pRigidBody);

	m_pCharacter_Controller->Set_Position(XMVectorSet(-52.f, 0.f, 4.f, 1.f));
	m_pTransformCom->Set_State(STATE::POSITION, XMVectorSet(-52.f, 0.f, 4.f, 1.f));

	return S_OK;
}

void CRanrak::Priority_Update(_float fTimeDelta)
{
	__super::Priority_Update(fTimeDelta);
}

void CRanrak::Update(_float fTimeDelta)
{
	if (m_pGameInstance->Key_Up(DIK_H))
	{
		m_ePhase = ENUM_CLASS(RANRAK_PHASE::PHASE_GROUND);
		m_pFSM->Change_State(FSMSTATE::LAND);
	}
	if (m_pGameInstance->Key_Up(DIK_Y))
	{
		m_ePhase = ENUM_CLASS(RANRAK_PHASE::PHASE_AIR);
		m_pFSM->Change_State(FSMSTATE::HOVER);
	}
	if (m_pGameInstance->Key_Up(DIK_I))
	{
		m_pFSM->Change_State(FSMSTATE::SKILL);
	}

	m_pFSM->Update_State(fTimeDelta);

	m_pModelCom->Play_Animation(fTimeDelta, m_pTransformCom);

	__super::Update(fTimeDelta);

	Play_Event();

	if (true == m_pCharacter_Controller->IsActive()) {
		{ // 세트
			m_pCallBack_HitReport->BeginFrame();
			m_pCharacter_Controller->Move(fTimeDelta);
			m_pCallBack_HitReport->Set_CurrentSlop();
		}
		m_vStunTimer.x = 0.f;
	}
	else if (true == m_pRigidBody->IsActive()) {
		if (0.f == m_vStunTimer.x) {
			PSX::PxExtendedVec3 pxControlllerPos = m_pCharacter_Controller->Get_Controller()->getPosition();
			PSX::PxTransform pxTransform((_float)pxControlllerPos.x, (_float)pxControlllerPos.y + 100.f, (_float)pxControlllerPos.z);
			m_pCharacter_Controller->Set_Position(XMLoadFloat3((_float3*)&pxTransform.p));
			//m_pCharacter_Controller->Move(fTimeDelta);
		}
		m_vStunTimer.x += fTimeDelta;
		m_pTransformCom->Set_WorldMatrix(m_pRigidBody->Get_Actor()->getGlobalPose());
		if (m_vStunTimer.y < m_vStunTimer.x) {
			m_pRigidBody->ConvertToCCT(*m_pCharacter_Controller);
		}
	}
#ifdef _DEBUG
	Describe_Entity();
#endif // _DEBUG


	for (_uint i = 0; i < ENUM_CLASS(RANRAK_SKILL::END); i++)
		m_fSkillCoolTime[i] = max(0.f, m_fSkillCoolTime[i] - fTimeDelta);

#pragma endregion
}

void CRanrak::Late_Update(_float fTimeDelta)
{
	__super::Late_Update(fTimeDelta);
	if (true == m_pCharacter_Controller->IsActive()) {
		m_pTransformCom->Set_State(STATE::POSITION, m_pCharacter_Controller->Get_FootPosition());
	}
	else if (true == m_pRigidBody->IsActive()) {
		m_pTransformCom->Set_WorldMatrix(m_pRigidBody->Get_Actor()->getGlobalPose());
	}

	if (true == m_bLookAt) {
		m_pTransformCom->LookAt_Lerp(XMLoadFloat4(&m_vTargetPos), fTimeDelta, 3.f);
	}


	_vector vDir = XMLoadFloat4(&m_vTargetPos) - Get_WorldPostion();
	vDir = XMVector4Normalize(vDir);
	_vector vLook = XMVector3Normalize(
		XMVectorSetY(m_pTransformCom->Get_State(STATE::LOOK), 0.f));
	float dot = XMVectorGetX(XMVector3Dot(vLook, vDir));
	dot = max(-1.f, min(1.f, dot));

	float angle = acosf(dot);
	m_fDegree = XMConvertToDegrees(angle);

	m_fCross = XMVectorGetY(XMVector3Cross(vLook, vDir));

	m_pGameInstance->Add_RenderGroup(RENDER::NONBLEND, this);
	Set_Shadow(m_pGameInstance->IsIn_ShadowViewFrustum(m_pTransformCom->Get_State(STATE::POSITION), m_pTransformCom->Get_Radius()));
}

HRESULT CRanrak::Render()
{
	if (!m_bVisible)
		return S_OK;

	if (FAILED(Bind_ShaderResources())) {
		return E_FAIL;
	}

	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();
	_uint iShaderPass = ENUM_CLASS(SHADER_PASS_ANIM::DEFAULT);
	if (true == m_bDrawOutLine) {
		iShaderPass = ENUM_CLASS(SHADER_PASS_ANIM::OUTLINE_WRITE);
	}
	if (FAILED(Render_DeadDisolve())) {
		return E_FAIL;
	}

	if (FAILED(Render_Disolve())) {
		return E_FAIL;
	}

	for (_uint i = 0; i < iNumMeshes; i++)
	{
		if (FAILED(m_pShaderCom->Bind_Matrices(
			"g_OffsetMatrix",
			m_pModelCom->Get_OffsetMatrix(i).data(),
			(_int)m_pModelCom->Get_OffsetMatrix(i).size()
		)))
		{
			return E_FAIL;
		}

		if (FAILED(m_pModelCom->Bind_Material(i, m_pShaderCom))) {
			return E_FAIL;
		}
		if (FAILED(m_pShaderCom->Begin(iShaderPass))) {
			return E_FAIL;
		}

		m_pModelCom->Bind_OutPut_SRV_VS(31, 0);

		if (FAILED(m_pModelCom->Render(i))) {
			return E_FAIL;
		}
	}

	if (m_bDrawOutLine) {
		Render_OutLine();
	}

#ifdef _DEBUG
	if (true == m_pCharacter_Controller->IsActive()) {
		if (FAILED(m_pCharacter_Controller->Render())) {
			return E_FAIL;
		}
	}
	else if (true == m_pRigidBody->IsActive()) {
		if (FAILED(m_pRigidBody->Render())) {
			return E_FAIL;
		}
	}
#endif

	return S_OK;
}

HRESULT CRanrak::Render_Shadow(SHADOW eType)
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix"))) {
		return E_FAIL;
	}
	if (FAILED(m_pGameInstance->Bind_Shadow_Resource(m_pShaderCom, "g_ViewMatrix", D3DTS::VIEW, eType))) {
		return E_FAIL;
	}
	if (FAILED(m_pGameInstance->Bind_Shadow_Resource(m_pShaderCom, "g_ProjMatrix", D3DTS::PROJ, eType))) {
		return E_FAIL;
	}
	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();
	for (_uint i = 0; i < iNumMeshes; i++)
	{
		if (FAILED(m_pShaderCom->Bind_Matrices(
			"g_OffsetMatrix",
			m_pModelCom->Get_OffsetMatrix(i).data(),
			(_int)m_pModelCom->Get_OffsetMatrix(i).size()
		)))
		{
			return E_FAIL;

		}
		if (FAILED(m_pShaderCom->Begin(ENUM_CLASS(SHADER_PASS_ANIM::SHADOW)))) {
			return E_FAIL;
		}

		m_pModelCom->Bind_OutPut_SRV_VS(31, 0);

		if (FAILED(m_pModelCom->Render(i))) {
			return E_FAIL;
		}
	}

	return S_OK;
}

_vector CRanrak::Get_LockOnPos()
{
	if (nullptr != m_pCharacter_Controller && true == m_pCharacter_Controller->IsActive()) {
		return m_pCharacter_Controller->Get_Position();
	}
	else if (nullptr != m_pRigidBody) {
		return m_pRigidBody->Get_Position();
	}
	return Get_WorldPostion();
}

void CRanrak::OnCollision(CGameObject* pOther, void* pDesc)
{
	if (true == m_bDead) {
		return;
	}

	ON_COLLISION_INFO* CollisionDesc = static_cast<ON_COLLISION_INFO*>(pDesc);


	m_DamageInfo.vTarget_Pos = m_pCharacter_Controller->Get_HeadPosition();

	CEffect_Container* pEffect_Container = dynamic_cast<CEffect_Container*>(pOther);

	pair<_float, _float> damagePair = {};

	if (pEffect_Container != nullptr)
	{
		_uint iSkillType = pEffect_Container->Get_SkillType();
		damagePair = Get_Damage(m_pInfoInstance->Get_Spell_Damage(iSkillType));

		switch (iSkillType)
		{
		case ENUM_CLASS(SKILL_TYPE::DESCENDO):
			m_eHitSpell = ENUM_CLASS(SKILL_TYPE::DESCENDO);
			break;
		case ENUM_CLASS(SKILL_TYPE::BOMBARDA):
			m_eHitSpell = ENUM_CLASS(SKILL_TYPE::DESCENDO);
			break;
		case ENUM_CLASS(SKILL_TYPE::FLIPENDO):
			m_eHitSpell = ENUM_CLASS(SKILL_TYPE::FLIPENDO);
			break;
		case ENUM_CLASS(SKILL_TYPE::JAP):
			m_eHitSpell = ENUM_CLASS(SKILL_TYPE::JAP);
			break;
		default:
			m_eHitSpell = ENUM_CLASS(SKILL_TYPE::END);
			break;
		}
	}
	else
	{
		damagePair = Get_Damage(m_pInfoInstance->Get_Spell_Damage(ENUM_CLASS(SKILL_TYPE::ANCIENT_MAGIC_THROW)));
	}

	CMapElement_Interactable* pProps = dynamic_cast<CMapElement_Interactable*>(pOther);

	if (pProps != nullptr)
	{
		m_eHitSpell = STATEANIM::KNOCKDOWN_FWD;
	}

	m_DamageInfo.fDamage = damagePair.first;
	m_pInfoInstance->Event_CallBack(TEXT("Monster_Hit"), &m_DamageInfo);
	if (0 == damagePair.second) {
		m_pFSM->Change_State(FSMSTATE::DEAD);
		return;
	}
	m_pFSM->Change_State(FSMSTATE::HIT);
}

void CRanrak::OnHit(CGameObject* pOther, CGameObject* pCaller)
{
}

HRESULT CRanrak::Ready_Components()
{
	CTransform::TRANSFORM_DESC Desc = {};

	Desc.fSpeedPerSec = 10.f;
	Desc.fRotationPerSec = XMConvertToRadians(180.0f);
	Desc.fRadius = 10.f;

	__super::Ready_Components(&Desc);

	m_strModelPrototypeTag = TEXT("Prototype_Component_Ranrak_Model");

	/* Com_Model */
	if (FAILED(__super::Add_Asset_Component(g_iStaticLevel, m_strModelPrototypeTag,
		reinterpret_cast<CComponent**>(&m_pModelCom)))) {
		return E_FAIL;
	}

	{ // CCT
		CCharacter_Controller::Character_Controller_DESC Desc{};

		Desc.iSubKind = ENUM_CLASS(PXOBJECT::RANRAK);
		Desc.pTransform = m_pTransformCom;
		Desc.eBodyType = ACTOR::CAPSULE;
		Desc.fContactOffset = 0.001f;
		Desc.fMaterial = { 1.2f, 1.0f, 0.0f };
		Desc.bAutoStepping = { false };
		Desc.fStepOffset = { 0.12f };
		Desc.fRadius = 1.2f;
		Desc.fHeight = 1.5f;
		Desc.pCallback_HitReport = m_pCallBack_HitReport = CCallBack_Monster_HitReport::Create();
		Desc.pCallback_Behavior = m_pCallBack_Behavior = CCallBack_Monster_Behavior::Create();
		Desc.eClimbingMode = PSX::PxCapsuleClimbingMode::eEASY;
		Desc.fWalkableSlope = 45.f;
		if (FAILED(Add_Asset_Component(g_iStaticLevel, TEXT("PHYSX_CCT_CAPSULE"), (CComponent**)&m_pCharacter_Controller, &Desc))) {
			return E_FAIL;
		}
	}
	m_pCharacter_Controller->Set_Position(m_pTransformCom->Get_State(STATE::POSITION));
	{ // DO
		CRigidBody_Dynamic::RIGIDBODY_DYNAMIC_DESC Desc{};
		Desc.iSubKind = ENUM_CLASS(PXOBJECT::RANRAK);
		if (FAILED(Add_Asset_Component(g_iStaticLevel, TEXT("PHYSX_DYNAMIC_BOX"), (CComponent**)&m_pRigidBody, &Desc))) {
			return E_FAIL;
		}
		m_pGameInstance->Detach_Actor(*m_pRigidBody->Get_Actor());
	}

	if (FAILED(Add_Asset_Component(g_iStaticLevel, TEXT("STAT_TROLL"), (CComponent**)&m_pStat))) {
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CRanrak::Ready_Parts()
{
	return S_OK;
}

HRESULT CRanrak::Bind_ShaderResources()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix"))) {
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::VIEW)))) {
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::PROJ)))) {
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Bind_Matrix("g_PrevWorldMatrix", m_pTransformCom->Get_PrevWorldMatrixPtr()))) {
		return E_FAIL;
	}
	if (FAILED(m_pGameInstance->Bind_PrevMatrix(m_pShaderCom, "g_PrevViewMatrix", D3DTS::VIEW))) {
		return E_FAIL;
	}
	if (FAILED(m_pGameInstance->Bind_PrevMatrix(m_pShaderCom, "g_PrevProjMatrix", D3DTS::PROJ))) {
		return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Bind_RawValue("g_fFar", m_pGameInstance->Get_CurrentCameraFar(), sizeof(_float)))) {
		return E_FAIL;
	}
	return S_OK;
}

CRanrak* CRanrak::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CRanrak* pInstance = new CRanrak(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CRanrak");
		SAFE_RELEASE(pInstance);
	}
	return pInstance;
}

CGameObject* CRanrak::Clone(void* pArg, CGameObject* pOwner)
{
	CRanrak* pInstance = new CRanrak(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CRanrak");
		SAFE_RELEASE(pInstance);
	}
	return pInstance;
}

void CRanrak::Free()
{
	__super::Free();

	if (nullptr != m_pCallBack_Behavior) {
		m_pCallBack_Behavior->Finalize();
	}
	if (nullptr != m_pCallBack_HitReport) {
		m_pCallBack_HitReport->Finalize();
	}

	SAFE_RELEASE(m_pCharacter_Controller);
	SAFE_RELEASE(m_pRigidBody);
	Safe_Delete(m_pCallBack_Behavior);
	Safe_Delete(m_pCallBack_HitReport);
}
#ifdef _DEBUG

void CRanrak::Describe_Entity()
{
	GUI::Begin("UNIT", 0, IMGUI_GLOBAL_BEGIN_FLAG);
	if (GUI::CollapsingHeader("Ranrak")) {
		__super::Describe_Entity();

		m_pTransformCom->Describe_Entity();
	}
	GUI::End();
}

#endif // _DEBUG
