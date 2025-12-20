#include "pch.h"
#include "Ranrok.h"

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
#include "State_Rush.h"
#include "State_Throw.h"
#pragma endregion

CRanrok::CRanrok(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CMonster(pDevice, pContext)
{
}

CRanrok::CRanrok(const CRanrok& Prototype)
	: CMonster(Prototype)
{
}

HRESULT CRanrok::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CRanrok::Initialize(void* pArg)
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

	if (NEXT_LEVEL == ENUM_CLASS(LEVEL::FIELD))
	{
		m_pCharacter_Controller->Set_Position(XMVectorSet(20.226f, 6.46f, 142.8f, 1.f));
		m_pTransformCom->Set_State(STATE::POSITION, XMVectorSet(20.226f, 6.46f, 142.8f, 1.f));
	}
	else {

		m_pCharacter_Controller->Set_Position(XMVectorSet(-44.704f, -2.860f, 16.071f, 1.f));
		m_pTransformCom->Set_State(STATE::POSITION, XMVectorSet(-44.704f, -2.860f, 16.071f, 1.f));
	}


	Set_Points();

	return S_OK;
}

void CRanrok::Priority_Update(_float fTimeDelta)
{
	__super::Priority_Update(fTimeDelta);
}

void CRanrok::Update(_float fTimeDelta)
{
	if (m_pGameInstance->Key_Up(DIK_H))
	{
		m_ePhase = ENUM_CLASS(RANROK_PHASE::PHASE_GROUND);
		m_pFSM->Change_State(FSMSTATE::LAND);
	}
	if (m_pGameInstance->Key_Up(DIK_Y))
	{
		m_ePhase = ENUM_CLASS(RANROK_PHASE::PHASE_AIR);
		m_pFSM->Change_State(FSMSTATE::HOVER);
	}
	if (m_pGameInstance->Key_Up(DIK_I))
	{
		m_pFSM->Change_State(FSMSTATE::TUCKED);
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


	for (_uint i = 0; i < ENUM_CLASS(RANROK_SKILL::END); i++)
		m_fSkillCoolTime[i] = max(0.f, m_fSkillCoolTime[i] - fTimeDelta);

#pragma endregion
}

void CRanrok::Late_Update(_float fTimeDelta)
{
	__super::Late_Update(fTimeDelta);
	if (true == m_pCharacter_Controller->IsActive()) {
		m_pTransformCom->Set_State(STATE::POSITION, m_pCharacter_Controller->Get_FootPosition());
	}
	else if (true == m_pRigidBody->IsActive()) {
		m_pTransformCom->Set_WorldMatrix(m_pRigidBody->Get_Actor()->getGlobalPose());
	}

	if (!m_pFSM->IsEnable(FSMSTATE::TUCKED))
	{
		if (true == m_bLookAt) {
			m_pTransformCom->LookAt_Horizontal_Lerp(XMLoadFloat4(&m_vTargetPos), fTimeDelta, 3.f);
		}
	}

	m_pGameInstance->Add_RenderGroup(RENDER::NONBLEND, this);
	Set_Shadow(m_pGameInstance->IsIn_ShadowViewFrustum(m_pTransformCom->Get_State(STATE::POSITION), m_pTransformCom->Get_Radius()));
}

HRESULT CRanrok::Render()
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

	if (0.f < m_fDeadRatio) {
		_bool bDisolve = false;
		if (FAILED(m_pShaderCom->Bind_RawValue("g_bDisolve", &bDisolve, sizeof(_bool)))) {
			return E_FAIL;
		}
	}

	{
		_bool bDisolve = false;
		_float zero = 0.f;
		m_pShaderCom->Bind_RawValue("g_bDisolve", &bDisolve, sizeof(_bool));
		m_pShaderCom->Bind_RawValue("g_fDisolveRatio", &zero, sizeof(_float));
	}

	return S_OK;
}

HRESULT CRanrok::Render_Shadow(SHADOW eType)
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

_vector CRanrok::Get_LockOnPos()
{
	if (nullptr != m_pCharacter_Controller && true == m_pCharacter_Controller->IsActive()) {
		return m_pCharacter_Controller->Get_Position();
	}
	else if (nullptr != m_pRigidBody) {
		return m_pRigidBody->Get_Position();
	}
	return Get_WorldPostion();
}

void CRanrok::OnCollision(CGameObject* pOther, void* pDesc)
{
	if (true == m_bDead) {
		return;
	}
	if (m_bFireBurst)
		return;

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
			m_eHitSpell = ENUM_CLASS(SKILL_TYPE::BOMBARDA);
			break;
		case ENUM_CLASS(SKILL_TYPE::JAP):
			m_eHitSpell = ENUM_CLASS(SKILL_TYPE::JAP);
			break;
		case ENUM_CLASS(SKILL_TYPE::LEVIOSO):
			m_eHitSpell = ENUM_CLASS(SKILL_TYPE::LEVIOSO);
			break;
		case ENUM_CLASS(SKILL_TYPE::ACCIO):
			m_eHitSpell = ENUM_CLASS(SKILL_TYPE::ACCIO);
			break;
		case ENUM_CLASS(SKILL_TYPE::STUPEFY):
			m_eHitSpell = ENUM_CLASS(SKILL_TYPE::STUPEFY);
			break;
		}
	}
	else
	{
		damagePair = Get_Damage(m_pInfoInstance->Get_Spell_Damage(ENUM_CLASS(SKILL_TYPE::ANCIENT_MAGIC_THROW)));

		CMapElement_Interactable* pProps = dynamic_cast<CMapElement_Interactable*>(pOther);

		if (pProps != nullptr)
		{
			m_eHitSpell = ENUM_CLASS(SKILL_TYPE::ANCIENT_MAGIC_THROW);
		}
	}


	m_DamageInfo.fDamage = damagePair.first;
	m_pInfoInstance->Event_CallBack(TEXT("Monster_Hit"), &m_DamageInfo);
	if (0 == damagePair.second) {
		m_pFSM->Change_State(FSMSTATE::DEAD);
		return;
	}
	if (damagePair.second <= Get_Hp().y / 2.f && m_ePhase == ENUM_CLASS(RANROK_PHASE::PHASE_AIR))
	{
		m_pFSM->Change_State(FSMSTATE::LAND);
		return;
	}
	m_pFSM->Change_State(FSMSTATE::HIT);
}

void CRanrok::OnHit(CGameObject* pOther, CGameObject* pCaller)
{
}

HRESULT CRanrok::Ready_Components()
{
	CTransform::TRANSFORM_DESC Desc = {};

	Desc.fSpeedPerSec = 10.f;
	Desc.fRotationPerSec = XMConvertToRadians(180.0f);
	Desc.fRadius = 10.f;

	__super::Ready_Components(&Desc);

	m_strModelPrototypeTag = TEXT("Prototype_Component_Ranrok_Model");

	/* Com_Model */
	if (FAILED(__super::Add_Asset_Component(g_iStaticLevel, m_strModelPrototypeTag,
		reinterpret_cast<CComponent**>(&m_pModelCom)))) {
		return E_FAIL;
	}

	{ // CCT
		CCharacter_Controller::Character_Controller_DESC Desc{};

		Desc.iSubKind = ENUM_CLASS(PXOBJECT::RANROK);
		Desc.pTransform = m_pTransformCom;
		Desc.eBodyType = ACTOR::CAPSULE;
		Desc.fContactOffset = 0.001f;
		Desc.fMaterial = { 1.2f, 1.0f, 0.0f };
		Desc.bAutoStepping = { false };
		Desc.fStepOffset = { 0.12f };
		Desc.fRadius = 2.f;
		Desc.fHeight = 2.f;
		Desc.pCallback_HitReport = m_pCallBack_HitReport = CCallBack_Monster_HitReport::Create();
		Desc.pCallback_Behavior = m_pCallBack_Behavior = CCallBack_Monster_Behavior::Create();
		Desc.eClimbingMode = PSX::PxCapsuleClimbingMode::eEASY;
		Desc.fWalkableSlope = 45.f;
		if (FAILED(Add_Asset_Component(g_iStaticLevel, TEXT("PHYSX_CCT_CAPSULE"), (CComponent**)&m_pCharacter_Controller, &Desc))) {
			return E_FAIL;
		}

		m_pCharacter_Controller->SetGravity(false);
	}
	m_pCharacter_Controller->Set_Position(m_pTransformCom->Get_State(STATE::POSITION));
	{ // DO
		CRigidBody_Dynamic::RIGIDBODY_DYNAMIC_DESC Desc{};
		Desc.iSubKind = ENUM_CLASS(PXOBJECT::RANROK);
		if (FAILED(Add_Asset_Component(g_iStaticLevel, TEXT("PHYSX_DYNAMIC_BOX"), (CComponent**)&m_pRigidBody, &Desc))) {
			return E_FAIL;
		}
		m_pGameInstance->Detach_Actor(*m_pRigidBody->Get_Actor());
	}

	if (FAILED(Add_Asset_Component(g_iStaticLevel, TEXT("STAT_RANROK"), (CComponent**)&m_pStat))) {
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CRanrok::Ready_Parts()
{
	return S_OK;
}

HRESULT CRanrok::Bind_ShaderResources()
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

void CRanrok::Set_Points()
{
	m_Points =
	{
		{
			XMVectorSet(-23.129f, 5.847f, 120.080f, 1.f),
			XMVectorSet(-35.026f, 2.671f, 144.041f, 1.f),
			XMVectorSet(-0.334f, -18.565f, 186.488f, 1.f),
		},

		{
			XMVectorSet(16.141f, -50.299f, 221.423f, 1.f),
			XMVectorSet(41.967f, -53.684f, 246.943f,1.f),
			XMVectorSet(11.950f, -51.993f, 263.268f,1.f),
			XMVectorSet(-17.090f, -51.099f, 236.738f,1.f),
			XMVectorSet(-13.968f, -53.362f, 201.439f,1.f),
			XMVectorSet(-13.968f, -53.362f, 201.439f,1.f),
			XMVectorSet(35.842f, -48.150f, 189.827f,1.f),
			XMVectorSet(51.975f, -47.209f, 207.986f,1.f),
			XMVectorSet(40.875f, -48.956f, 256.929f,1.f)
		},

		{
			XMVectorSet(14.121f, -41.890f, 248.021f,1.f),
			XMVectorSet(-12.558f, -28.878f, 236.451f,1.f),
			XMVectorSet(-20.145f, -48.688f, 216.771f,1.f),
			XMVectorSet(11.854f, -50.864f, 182.233f,1.f),
			XMVectorSet(44.661f, -52.001f, 202.435f,1.f),
			XMVectorSet(29.840f, -60.349f, 212.004f,1.f),
			XMVectorSet(23.993f, -58.307f, 220.465f,1.f)
		}
	};
}

void CRanrok::MoveTo(_float fTimeDelta)
{
	if (!m_pFSM->IsEnable(FSMSTATE::TUCKED))
		return;

	if (m_Points.empty())
		return;

	if (m_iCurrentFlow >= m_Points.size())
	{
		m_iCurrentFlow = 0;
	}

	_vector Target;
	if (m_iCurrentFlow == 1)
	{
		_float fRandom = m_pGameInstance->Real_Random_Float(-20.f, 20.f);
		Target = m_Points[m_iCurrentFlow][m_iCurrentPoint];
		Target += XMVectorSet(0.f, fRandom, 0.f, 0.f);
	}
	else
	{
		Target = m_Points[m_iCurrentFlow][m_iCurrentPoint];
	}

	_vector CurPos = m_pCharacter_Controller->Get_Position();

	_vector toTarget = Target - CurPos;
	_float fDist = XMVectorGetX(XMVector3Length(toTarget));

	if (fDist < 10.f)
	{
		if (m_iCurrentPoint == m_Points[m_iCurrentFlow].size() - 1)
		{
			m_bTucked = true;
			m_pFSM->Disable_State(FSMSTATE::TUCKED);
		}
		m_iCurrentPoint = (m_iCurrentPoint + 1) % m_Points[m_iCurrentFlow].size();
		return;
	}

	/*if (m_iCurrentFlow == 1)
	{
		AroundPoint(fTimeDelta);
		return;
	}*/

	_vector vDir = XMVector3Normalize(toTarget);

	m_vMoveDir = XMVectorLerp(m_vMoveDir, vDir,fTimeDelta * 4.f);

	m_vMoveDir = XMVector3Normalize(m_vMoveDir);

	m_pTransformCom->LookAt_Horizontal_Lerp(CurPos + m_vMoveDir,fTimeDelta,4.f);

	_float speed = 50.f;
	m_pCharacter_Controller->Set_Position(CurPos + m_vMoveDir * speed * fTimeDelta);
}



void CRanrok::AroundPoint(_float fTimeDelta)
{
	m_fAroundTime += fTimeDelta;
	if (m_fAroundTime >= 6.f)
	{
		m_fAroundTime = 0.f;
		m_iCurrentFlow++;
	}
	else {
		_vector Center = XMVectorSet(16.141f, -50.299f, 221.423f, 1.f);

		m_fAroundAngle += m_fAroundSpeed * fTimeDelta;
		if (m_fAroundAngle > XM_2PI) m_fAroundAngle -= XM_2PI;

		_float x = cosf(m_fAroundAngle) * m_fAroundRadius;
		_float z = sinf(m_fAroundAngle) * m_fAroundRadius;

		_vector TargetPos = Center + XMVectorSet(x, 0.f, z, 0.f);

		_vector CurPos = m_pCharacter_Controller->Get_Position();
		_vector vDir = TargetPos - CurPos;
		_float dist = XMVectorGetX(XMVector3Length(vDir));

		if (dist > 0.001f)
		{
			vDir = XMVector3Normalize(vDir);

			_float moveSpeed = 50.f;
			_vector NextPos = CurPos + vDir * (moveSpeed * fTimeDelta);

			m_pCharacter_Controller->Set_Position(NextPos);
		}

		m_pTransformCom->LookAt_Horizontal_Lerp(Center, fTimeDelta, 3.f);
	}
}




CRanrok* CRanrok::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CRanrok* pInstance = new CRanrok(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CRanrak");
		SAFE_RELEASE(pInstance);
	}
	return pInstance;
}

CGameObject* CRanrok::Clone(void* pArg, CGameObject* pOwner)
{
	CRanrok* pInstance = new CRanrok(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CRanrak");
		SAFE_RELEASE(pInstance);
	}
	return pInstance;
}

void CRanrok::Free()
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

void CRanrok::Describe_Entity()
{
	GUI::Begin("UNIT", 0, IMGUI_GLOBAL_BEGIN_FLAG);
	if (GUI::CollapsingHeader("Ranrak")) {
		__super::Describe_Entity();

		GUI::Text("Degree %.2f", m_fDegree);
		GUI::Text("CurrFlow %d", m_iCurrentFlow);

		_float3 Pos;
		XMStoreFloat3(&Pos, Get_WorldPostion());

		if (GUI::DragFloat3("Pos", (_float*)&Pos, 0.01f))
		{
			m_pCharacter_Controller->Set_Position(XMVectorSetW(XMLoadFloat3(&Pos), 1.f));
		}

		ImGui::SameLine();

		if (GUI::SmallButton("Copy"))
		{
			char buf[64];
			sprintf_s(buf, "%.3ff, %.3ff, %.3ff", Pos.x, Pos.y, Pos.z);
			ImGui::SetClipboardText(buf);
		}

		if (GUI::Button("MovePos"))
		{
			m_pCharacter_Controller->Set_Position(XMVectorSet(m_pGameInstance->Get_CamPosition()->x, m_pGameInstance->Get_CamPosition()->y, m_pGameInstance->Get_CamPosition()->z,1.f));
		}
		
		m_pTransformCom->Describe_Entity();
	}
	GUI::End();
}

#endif // _DEBUG
