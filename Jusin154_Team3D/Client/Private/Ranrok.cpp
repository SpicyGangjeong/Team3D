#include "pch.h"
#include "Ranrok.h"

#include "GameInstance.h"
#include "InfoInstance.h"
#include "CallBack_Ranrok_HitReport.h"
#include "Effect_Container.h"
#include "EffectParts.h"
#include "EffectPool.h"
#include "Layer.h"
#include "TrailObject.h"
#include "MapElement_Interactable.h"
#include "TimeSocket.h"
#include "Layer.h"


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


	Load_AnimXML("../Bin/Resources/Data/AnimList/Ranrok.xml");

	Load_RanrokPos("../Bin/Resources/Data/RanrokPos/RanrokPos.xml");
	{
		CFSM::FSM_DESC FSMDesc{};
		FSMDesc.pStates = &m_States;
		FSMDesc.pStateMask = &m_iStateMask;

		m_pFSM->Bind_States(FSMDesc);
		m_pFSM->Change_State(FSMSTATE::IDLE);
	}

	m_pCallBack_Behavior->Initialize(m_pCharacter_Controller, m_pRigidBody);
	m_pCallBack_HitReport->Initialize(m_pCharacter_Controller, m_pRigidBody,&m_bCollisionPlayer);

	m_pEffectPool = m_pGameInstance->Get_Layer(NEXT_LEVEL, TEXT("Layer_EffectPool"))->Get_Object<CEffectPool>();
	SAFE_ADDREF(m_pEffectPool);

	RANROKDESC* pDesc = static_cast<RANROKDESC*>(pArg);

	_vector vPos = XMLoadFloat4(&pDesc->vPos);
	m_pTransformCom->Set_State(STATE::POSITION, vPos);
	m_pCharacter_Controller->Set_Position(vPos);
	m_pTransformCom->Rotation(XMLoadFloat4(&pDesc->vRotQ));


	m_pModelCom->Set_DisableRootMotionScale(true);
#ifdef _DEBUG
	m_pTransformCom->Compress_WorldMatrix(m_vInitialTrans, m_vInitialRotQ);
#endif // _DEBUG

	return S_OK;
}

void CRanrok::Priority_Update(_float fTimeDelta)
{
	if (m_bFireBurst) {
		m_pInfoInstance->Deregist_ActiveMonster(this);
	}
	else {
		m_pInfoInstance->Regist_ActiveMonster(this);
	}
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
	Update_CollidersPosition();
	if (m_bMotionTrail) {
		m_vCaptureTimer.x += fTimeDelta;
		if (m_vCaptureTimer.y < m_vCaptureTimer.x) {
			m_vCaptureTimer.x = 0.f;
			if (FAILED(m_pModelCom->Capture_BoneBuffer(m_pMotionTrailCom, *m_pTransformCom->Get_WorldMatrixPtr()))) {
				assert(false);
			}
		}
	}


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

	m_vEtherealTimer.x += fTimeDelta * 0.2f;
	if (m_vEtherealTimer.x > m_vEtherealTimer.y) {
		m_vEtherealTimer.x -= m_vEtherealTimer.y;
	}


	Update_Disolve(fTimeDelta,0.8f);


#pragma region TRAIL_UPDATE

	_matrix WorldMat = m_pTransformCom->Get_XMWorldMatrix();

	_matrix LeftEyeMatrix = {};
	_matrix RightEyeMatrix = {};

	LeftEyeMatrix = XMLoadFloat4x4(m_pLeftEye_BoneMat);
	RightEyeMatrix = XMLoadFloat4x4(m_pRightEye_BoneMat);


	for (int i = 0; i < 3; ++i) {
		LeftEyeMatrix.r[i] = XMVector3Normalize(LeftEyeMatrix.r[i]);
		RightEyeMatrix.r[i] = XMVector3Normalize(RightEyeMatrix.r[i]);
	}

	m_pLeftEye_Trail->Trail_Update(LeftEyeMatrix * WorldMat, fTimeDelta);
	m_pRightEye_Trail->Trail_Update(RightEyeMatrix * WorldMat, fTimeDelta);

#pragma endregion


#pragma region CREATE PROP

	if (!m_pFSM->IsEnable(FSMSTATE::IDLE | FSMSTATE::PULSE | FSMSTATE::TUCKED | FSMSTATE::DEAD))
	{
		m_vCreatePropTime.x += fTimeDelta;

		if (m_vCreatePropTime.x >= m_vCreatePropTime.y)
		{
			CEffect_Container* pEffect = nullptr;
			m_pEffectPool->Use_Skill(SKILL_TYPE::RANROK_PROP, this, nullptr, &pEffect);
			m_vCreatePropTime.x = 0.f;

			if(nullptr != pEffect)
				m_pRanrok_Props.push_back(pEffect);
		}


		_bool bAllHidden = true;

		for (auto& Props : m_pRanrok_Props)
		{
			if (Props->Get_Visible())
			{
				bAllHidden = false;
				break;
			}
		}
		if (m_pRanrok_Props.size() == 0) {
			bAllHidden = false;
		}

		if (bAllHidden)
		{
			m_bFireBurst = false;
		}
		else {
			m_bFireBurst = true;
		}
	}

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
			m_pTransformCom->LookAt_Horizontal_Lerp(XMLoadFloat4(&m_vTargetPos), fTimeDelta, 2.5f);

		}
		_vector vPos = m_pTransformCom->Get_State(STATE::POSITION);
		_vector vRight = m_pTransformCom->Get_State(STATE::RIGHT);

		_float4 SetPos = {};
		XMStoreFloat4(&SetPos,vPos + (vRight * (_float)m_iBreathRand));

		m_pModelCom->Set_TargetPos(SetPos);

	}

	m_pGameInstance->Add_RenderGroup(RENDER::NONBLEND, this);
	m_pGameInstance->Add_RenderGroup(RENDER::BLEND, this);
	m_pGameInstance->Add_RenderGroup(RENDER::NONLIGHT, this);
	Set_Shadow(m_pGameInstance->IsIn_ShadowViewFrustum(m_pTransformCom->Get_State(STATE::POSITION), m_pTransformCom->Get_Radius()));
}

HRESULT CRanrok::Render()
{
	if (FAILED(Bind_ShaderResources())) {
		return E_FAIL;
	}
	RENDER eCurrentPass = m_pGameInstance->Get_CurrentRenderPass();
	HRESULT hr = E_FAIL;
	if (RENDER::NONBLEND == eCurrentPass) {
		hr = Render_Nonblend();
	}
	else if (RENDER::BLEND == eCurrentPass) {
		//hr = Render_Blend();
		hr = S_OK;
	}
#ifdef _DEBUG
	else if (RENDER::NONLIGHT == eCurrentPass) {
		hr = Render_Collider();
	}
#endif // _DEBUG

	return hr;
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
		if (FAILED(m_pShaderCom->Begin(ENUM_CLASS(SHADER_PASS_NPC_PBR_ANIM::SHADOW)))) {
			return E_FAIL;
		}

		m_pModelCom->Bind_OutPut_SRV_VS(26, 0);
		m_pModelCom->Bind_OutPut_SRV_VS_Prev(27, 0);

		if (FAILED(m_pModelCom->Render(i))) {
			return E_FAIL;
		}
	}

	return S_OK;
}

void CRanrok::Trigger(CTimeSocket& Socket)
{
	SOCKETCONTENTS* pContents = &Socket.m_Contents;
	switch (pContents->eTypeFunc)
	{
	case TIMESOCKET_FUNC::TRANSLATION:
	{

	}break;
	case TIMESOCKET_FUNC::TRANSLATION_LERP:
	{

	}break;
	case TIMESOCKET_FUNC::SET_ANIMSTATE:
	{
	}break;
	case TIMESOCKET_FUNC::SET_FSMSTATE:
	{
		if (Socket.m_Contents.vFlags.b[0]) {
			m_pFSM->Change_State(FSMSTATE::TUCKED);
		}
		else if (Socket.m_Contents.vFlags.b[1]) {
			m_pFSM->Change_State(FSMSTATE::FIRESWEEP);
		}
	}break;
	default:
		break;
	}
}

HRESULT CRanrok::Render_MotionTrail(ID3D11ShaderResourceView* pSRV)
{
	for (_uint i = ENUM_CLASS(RANROK_MESH_ORDER::WINGS); i < ENUM_CLASS(RANROK_MESH_ORDER::END); ++i)
	{
		if (FAILED(m_pShaderCom->Bind_Matrices("g_OffsetMatrix",m_pModelCom->Get_OffsetMatrix(i).data(),
			(_int)m_pModelCom->Get_OffsetMatrix(i).size()))) {
			return E_FAIL;
		}
		if (FAILED(m_pModelCom->Bind_Material(i, m_pShaderCom))) {
			return E_FAIL;
		}
		if (FAILED(m_pModelCom->Begin(i, m_pShaderCom))) {
			return E_FAIL;
		}

		m_pContext->VSSetShaderResources(26, 1, &pSRV);

		if (FAILED(m_pModelCom->Render(i))) {
			return E_FAIL;
		}
	}
	return S_OK;
}

_vector CRanrok::Get_LockOnPos()
{
	_vector vCameraLook = m_pGameInstance->Get_CameraLook();
	_vector vCameraPos = m_pGameInstance->Get_CamXMPosition();

	_vector vNearestPos = m_pCharacter_Controller->Get_Position();
	_vector vToMonsterDir = vNearestPos - vCameraPos;
	_float fMaxDot = CMyTools::DirectionCompare(vCameraLook, vToMonsterDir);

	{
		for (_uint i = 0; i < ENUM_CLASS(RANROK_ENUM_BONEMATRICES::END); ++i) {
			_vector vPosResult = m_pTargetableDO[i]->Get_Position();
			vToMonsterDir = vPosResult - vCameraPos;
			
			_float fDotResult = CMyTools::DirectionCompare(vCameraLook, vToMonsterDir);

			if (fMaxDot < fDotResult) {
				vNearestPos = vPosResult;
				fMaxDot = fDotResult;
			}
		}
	}
	return XMVectorSetW(vNearestPos, 1.f);
}

void CRanrok::OnCollision(CGameObject* pOther, void* pDesc)
{
	if (true == m_bDead) {
		return;
	}

	if (m_bFireBurst || m_pFSM->IsEnable(FSMSTATE::LAND|FSMSTATE::TUCKED) || m_pModelCom->Get_AnimIndex() == m_Animation[STATEANIM::HIT_BWD2].first)
		return;

	ON_COLLISION_INFO* CollisionDesc = static_cast<ON_COLLISION_INFO*>(pDesc);

	XMStoreFloat4(&m_DamageInfo.vTarget_Pos, m_pCharacter_Controller->Get_HeadPosition());


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
		case ENUM_CLASS(SKILL_TYPE::AVADAKEDAVRA):
			m_eHitSpell = ENUM_CLASS(SKILL_TYPE::AVADAKEDAVRA);
			break;
		case ENUM_CLASS(SKILL_TYPE::ANCIENT_MAGIC):
			m_eHitSpell = ENUM_CLASS(SKILL_TYPE::ANCIENT_MAGIC);
			break;
		}

		m_pEffectPool->Use_Skill(SKILL_TYPE::RANROK_HIT, this, &CollisionDesc->vWorldPos);
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
#ifndef 진우
	m_pInfoInstance->Event_CallBack(TEXT("Monster_Hit"), &m_DamageInfo);
#endif
	if (0 == damagePair.second) {
		m_pFSM->Change_State(FSMSTATE::DEAD);
		return;
	}

	Update_BehaviorByHPRatio(CollisionDesc);

	if (!IsHitStateDisabled() || IsHitSpellDisabled()) {
		m_pFSM->Change_State(FSMSTATE::HIT);
	}
}

void CRanrok::OnHit(CGameObject* pOther, CGameObject* pCaller)
{
}

_vector CRanrok::Get_BonePos(RANROK_ENUM_BONEMATRICES eType)
{
	PSX::PxTransform pxPos = m_pTargetableDO[ENUM_CLASS(eType)]->Get_GlobalPosition();
	return XMVectorSet(pxPos.p.x, pxPos.p.y, pxPos.p.z, 1.f);
}

HRESULT CRanrok::Ready_Components()
{
	CTransform::TRANSFORM_DESC Desc = {};

	Desc.fSpeedPerSec = 10.f;
	Desc.fRotationPerSec = XMConvertToRadians(180.0f);
	Desc.fRadius = 10.f;

	__super::Ready_Components(&Desc);
	SAFE_RELEASE(m_pShaderCom);

	if (FAILED(__super::Add_Asset_Component(g_iStaticLevel, FX_NPC_PBR_ANIM,
		reinterpret_cast<CComponent**>(&m_pShaderCom)))) {
		return E_FAIL;
	}

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
		Desc.pCallback_HitReport = m_pCallBack_HitReport = CCallBack_Ranrok_HitReport::Create();
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
		if (FAILED(Add_Asset_Component(g_iStaticLevel, TEXT("PHYSX_DYNAMIC_RANROK"), (CComponent**)&m_pRigidBody, &Desc))) {
			return E_FAIL;
		}
		m_pGameInstance->Detach_Actor(*m_pRigidBody->Get_Actor(), NEXT_LEVEL);
	}

	if (FAILED(Add_Asset_Component(g_iStaticLevel, TEXT("STAT_RANROK"), (CComponent**)&m_pStat))) {
		return E_FAIL;
	}

	{
		CMotion_Trail::MOTIONTRAIL_RENDERFUNC funcDesc{};
		funcDesc.funcRenderCall = [this](ID3D11ShaderResourceView* pSRV) { Render_MotionTrail(pSRV); };
		if (FAILED(Add_Asset_Component(g_iStaticLevel, TEXT("Prototype_Component_Ranrok_MotionTrail"), (CComponent**)&m_pMotionTrailCom, &funcDesc))) {
			return E_FAIL;
		}
	}

	return S_OK;
}

HRESULT CRanrok::Ready_Parts()
{
#pragma region EFFECT
	/* EFFECT */

	CPartObject::PARTOBJECT_DESC PartsDesc{};

	PartsDesc.pParentTransform = m_pTransformCom;



	if (FAILED(Add_PartObject<CEffectParts>("LeftSmoke", g_iStaticLevel, &m_pLeftSmoke, &PartsDesc)))
	{
		return E_FAIL;
	}

	m_pLeftSmoke->Load("../Bin/Resources/Data/Effect/Ranrok/RanrokSmoke/Smoke", static_cast<LEVEL>(g_iStaticLevel));
	m_pLeftSmoke->FollowParents(m_pModelCom->Get_BoneMatrixPtr("wrist_left_target"));

	if (FAILED(Add_PartObject<CEffectParts>("RightSmoke", g_iStaticLevel, &m_pRightSmoke, &PartsDesc)))
	{
		return E_FAIL;
	}

	m_pRightSmoke->Load("../Bin/Resources/Data/Effect/Ranrok/RanrokSmoke/Smoke", static_cast<LEVEL>(g_iStaticLevel));
	m_pRightSmoke->FollowParents(m_pModelCom->Get_BoneMatrixPtr("wrist_right_target"));

	if (FAILED(Add_PartObject<CEffectParts>("BottomSmoke", g_iStaticLevel, &m_pBottomSmoke, &PartsDesc)))
	{
		return E_FAIL;
	}

	m_pBottomSmoke->Load("../Bin/Resources/Data/Effect/Ranrok/RanrokSmoke/Smoke_Bottom", static_cast<LEVEL>(g_iStaticLevel));
	m_pBottomSmoke->FollowParents(m_pModelCom->Get_BoneMatrixPtr("hip"));

	if (FAILED(Add_PartObject<CEffectParts>("LeftParticle", g_iStaticLevel, &m_pLeftPt, &PartsDesc)))
	{
		return E_FAIL;
	}

	m_pLeftPt->Load("../Bin/Resources/Data/Effect/Ranrok/RanrokSide/Side_PT", static_cast<LEVEL>(g_iStaticLevel));
	m_pLeftPt->FollowParents(m_pModelCom->Get_BoneMatrixPtr("indexmiddlewing_04_right"));


	if (FAILED(Add_PartObject<CEffectParts>("RightParticle", g_iStaticLevel, &m_pRightPt, &PartsDesc)))
	{
		return E_FAIL;
	}

	m_pRightPt->Load("../Bin/Resources/Data/Effect/Ranrok/RanrokSide/Side_PT", static_cast<LEVEL>(g_iStaticLevel));
	m_pRightPt->FollowParents(m_pModelCom->Get_BoneMatrixPtr("indexmiddlewing_04_left"));




	if (FAILED(Add_PartObject<CTrailObject>("Left_Trail", g_iStaticLevel, &m_pLeftEye_Trail, &PartsDesc))) {
		return E_FAIL;
	}

	m_pLeftEye_Trail->Load_Trail("../Bin/Resources/Data/Effect/Ranrok/RanrokSide/Eye_Trail", static_cast<LEVEL>(g_iStaticLevel));
	m_pLeftEye_Trail->Set_Visible(true);

	if (FAILED(Add_PartObject<CTrailObject>("Right_Trail", g_iStaticLevel, &m_pRightEye_Trail, &PartsDesc))) {
		return E_FAIL;
	}

	m_pRightEye_Trail->Load_Trail("../Bin/Resources/Data/Effect/Ranrok/RanrokSide/Eye_Trail", static_cast<LEVEL>(g_iStaticLevel));
	m_pRightEye_Trail->Set_Visible(true);

	if (FAILED(Ready_SubParts())) {
		return E_FAIL;
	}
#pragma endregion

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
HRESULT CRanrok::Ready_SubParts()
{
	m_pLeftEye_BoneMat = m_pModelCom->Get_BoneMatrixPtr("eye_left");
	m_pRightEye_BoneMat = m_pModelCom->Get_BoneMatrixPtr("eye_right");

	m_pRanrokCombinedBoneMatrices[ENUM_CLASS(RANROK_ENUM_BONEMATRICES::RIGHTWING_WRIST)]	=	m_pRightWingWrist_BoneMat	= m_pModelCom->Get_BoneMatrixPtr("wrist_right");
	m_pRanrokCombinedBoneMatrices[ENUM_CLASS(RANROK_ENUM_BONEMATRICES::RIGHTWING_TIP)]		=	m_pRightWingTip_BoneMat		= m_pModelCom->Get_BoneMatrixPtr("middle_05_right");
	m_pRanrokCombinedBoneMatrices[ENUM_CLASS(RANROK_ENUM_BONEMATRICES::RIGHTWING_PINKY)]	=	m_pRightWingPinky_BoneMat	= m_pModelCom->Get_BoneMatrixPtr("pinkyelbowwing_04_right");

	m_pRanrokCombinedBoneMatrices[ENUM_CLASS(RANROK_ENUM_BONEMATRICES::LEFTWING_WRIST)]		=	m_pLeftWingWrist_BoneMat	= m_pModelCom->Get_BoneMatrixPtr("wrist_left");
	m_pRanrokCombinedBoneMatrices[ENUM_CLASS(RANROK_ENUM_BONEMATRICES::LEFTWING_TIP)]		=	m_pLeftWingTip_BoneMat		= m_pModelCom->Get_BoneMatrixPtr("middle_05_left");
	m_pRanrokCombinedBoneMatrices[ENUM_CLASS(RANROK_ENUM_BONEMATRICES::LEFTWING_PINKY)]		=	m_pLeftWingPinky_BoneMat	= m_pModelCom->Get_BoneMatrixPtr("pinkyelbowwing_04_left");

	m_pRanrokCombinedBoneMatrices[ENUM_CLASS(RANROK_ENUM_BONEMATRICES::HEAD)]				=	m_pHead_BoneMat				= m_pModelCom->Get_BoneMatrixPtr("head");
	m_pRanrokCombinedBoneMatrices[ENUM_CLASS(RANROK_ENUM_BONEMATRICES::BODY)]				=	m_pBody_BoneMat				= m_pModelCom->Get_BoneMatrixPtr("chest_Main");
	m_pRanrokCombinedBoneMatrices[ENUM_CLASS(RANROK_ENUM_BONEMATRICES::TAIL0)]				=	m_pTail0_BoneMat			= m_pModelCom->Get_BoneMatrixPtr("tail_04");
	m_pRanrokCombinedBoneMatrices[ENUM_CLASS(RANROK_ENUM_BONEMATRICES::TAIL1)]				=	m_pTail1_BoneMat			= m_pModelCom->Get_BoneMatrixPtr("tail_08");
	m_pRanrokCombinedBoneMatrices[ENUM_CLASS(RANROK_ENUM_BONEMATRICES::TAIL2)]				=	m_pTail2_BoneMat			= m_pModelCom->Get_BoneMatrixPtr("tail_12");

	CRigidBody_Dynamic::RIGIDBODY_DYNAMIC_DESC Desc{};
	Desc.bAutoOwnerTranslation = false;
	Desc.iSubKind = ENUM_CLASS(PXOBJECT::RANROK_PROP);
	for (_uint i = 0; i < ENUM_CLASS(RANROK_ENUM_BONEMATRICES::END); ++i) {
		if (FAILED(Add_Asset_Component(g_iStaticLevel, TEXT("PHYSX_DYNAMIC_SHIELD"), (CComponent**)&m_pTargetableDO[i], &Desc))) {
			return E_FAIL;
		}
		m_pTargetableDO[i]->Set_Kinematic(true);
	}

#ifdef _DEBUG
	m_pSubShape = (GeometricPrimitive::CreateSphere(m_pContext, 1.f, 6, false, false));
	D3D11_DEPTH_STENCIL_DESC depthStencilDesc = {};
	depthStencilDesc.DepthEnable = FALSE;
	depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
	depthStencilDesc.DepthFunc = D3D11_COMPARISON_ALWAYS;
	depthStencilDesc.StencilEnable = FALSE;

	HRESULT result = m_pDevice->CreateDepthStencilState(&depthStencilDesc, &m_pDepthStencilStateNone);
	if (FAILED(result)) {
		return E_FAIL;
	}
#endif // _DEBUG
	return S_OK;
}
HRESULT CRanrok::Render_Nonblend()
{
	if (FAILED(Render_DeadDisolve())) {
		return E_FAIL;
	}

	if (FAILED(Render_Disolve())) {
		return E_FAIL;
	}

	for (_uint i = ENUM_CLASS(RANROK_MESH_ORDER::WINGS); i < ENUM_CLASS(RANROK_MESH_ORDER::END); ++i)
	{
		if (FAILED(m_pShaderCom->Bind_Matrices("g_OffsetMatrix",
			m_pModelCom->Get_OffsetMatrix(i).data(),
			(_int)m_pModelCom->Get_OffsetMatrix(i).size())))
		{
			return E_FAIL;
		}

		if (FAILED(m_pModelCom->Bind_Material(i, m_pShaderCom))) {
			return E_FAIL;
		}

		if (FAILED(m_pModelCom->Begin(i, m_pShaderCom))) {
			return E_FAIL;
		}

		m_pModelCom->Bind_OutPut_SRV_VS(26, 0);
		m_pModelCom->Bind_OutPut_SRV_VS_Prev(27, 0);

		if (true == m_bDrawOutLine) {
			m_pGameInstance->Begin_OutLine_Write(2);
		}
		if (FAILED(m_pModelCom->Render(i))) {
			return E_FAIL;
		}
		if (true == m_bDrawOutLine) {
			m_pGameInstance->End_OutLine_Write();
		}
	}

	/*if (m_bDrawOutLine) {
		Render_OutLine();
	}*/

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

#ifndef 진우
	if (m_bMotionTrail) {
		if (FAILED(m_pMotionTrailCom->Render(m_pShaderCom))) {
			return E_FAIL;
		}
	}
#endif
	return S_OK;
}

HRESULT CRanrok::Update_CollidersPosition()
{
	for (_uint i = 0; i < ENUM_CLASS(RANROK_ENUM_BONEMATRICES::END); ++i) {
		m_pTargetableDO[i]->Set_Transform(XMLoadFloat4x4(m_pRanrokCombinedBoneMatrices[i]) * m_pTransformCom->Get_XMWorldMatrix(), true);
	}
	return S_OK;
}

HRESULT CRanrok::Render_Blend()
{
	_float fDiffuseUVRatio = (m_vEtherealTimer.x / m_vEtherealTimer.y);
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fEtherealRatio", &fDiffuseUVRatio, sizeof(_float)))) {
		return E_FAIL;
	}
	for (_uint i = ENUM_CLASS(RANROK_MESH_ORDER::ETHEREAL_HOT_SPINE); i < ENUM_CLASS(RANROK_MESH_ORDER::WINGS); ++i)
	{
		if (FAILED(m_pShaderCom->Bind_Matrices("g_OffsetMatrix",
			m_pModelCom->Get_OffsetMatrix(i).data(),
			(_int)m_pModelCom->Get_OffsetMatrix(i).size())))
		{
			return E_FAIL;
		}

		if (FAILED(m_pModelCom->Bind_Material(i, m_pShaderCom))) {
			return E_FAIL;
		}
		if (FAILED(m_pModelCom->Begin(i, m_pShaderCom))) {
			return E_FAIL;
		}

		m_pModelCom->Bind_OutPut_SRV_VS(26, 0);
		m_pModelCom->Bind_OutPut_SRV_VS_Prev(27, 0);

		if (FAILED(m_pModelCom->Render(i))) {
			return E_FAIL;
		}
	}
	return S_OK;
}
HRESULT CRanrok::Render_OutLine()
{
	m_bDrawOutLine = false;
	if (FAILED(Bind_ShaderResources())) {
		return E_FAIL;
	}

	Compute_Depth();
	_float fCamFar = *m_pGameInstance->Get_CurrentCameraFar();
	_float fRatio = CMyTools::Saturate((m_fCamDepth / (fCamFar * fCamFar)));
	m_fOutLineThickness = CMyTools::Lerp_f1D(0.14f, 0.5f, fRatio);
	if (FAILED(m_pShaderCom->Bind_Matrix("g_PrevWorldMatrix", m_pTransformCom->Get_PrevWorldMatrixPtr()))) {
		return E_FAIL;
	}
	if (FAILED(m_pGameInstance->Bind_PrevMatrix(m_pShaderCom, "g_PrevViewMatrix", D3DTS::VIEW))) {
		return E_FAIL;
	}
	if (FAILED(m_pGameInstance->Bind_PrevMatrix(m_pShaderCom, "g_PrevProjMatrix", D3DTS::PROJ))) {
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Bind_RawValue("g_vOutLineColor", &m_vOutLineColor, sizeof(_float3)))) {
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fOutLineThickness", &m_fOutLineThickness, sizeof(_float)))) {
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fOutLineScale", &m_fOutLineScale, sizeof(_float)))) {
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fOutLinePower", &m_fOutLinePower, sizeof(_float)))) {
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Bind_RawValue("g_vCamPosition", m_pGameInstance->Get_CamPosition(), sizeof(_float4)))) {
		return E_FAIL;
	}
	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = ENUM_CLASS(RANROK_MESH_ORDER::WINGS); i < ENUM_CLASS(RANROK_MESH_ORDER::END); ++i)
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

		if (FAILED(m_pShaderCom->Begin(ENUM_CLASS(SHADER_PASS_NPC_PBR_ANIM::OUTLINE_READ)))) {
			return E_FAIL;
		}

		m_pModelCom->Bind_OutPut_SRV_VS(26, 0);
		m_pModelCom->Bind_OutPut_SRV_VS_Prev(27, 0);

		if (FAILED(m_pModelCom->Render(i))) {
			return E_FAIL;
		}
	}
	return S_OK;
}

void CRanrok::MoveTo(_float fTimeDelta)
{
	if (!m_pFSM->IsEnable(FSMSTATE::TUCKED))
		return;

	if (m_Points.empty())
		return;

	if (m_iCurrentFlow >= m_Points.size())
		m_iCurrentFlow = 0;

	if (m_iCurrentFlow == m_Points.size() - 1)
	{
		if (m_iCurrentPoint >= m_Points[m_iCurrentFlow].size() * 0.3f)
		{
			m_fTuckedSpeed = 55.f;
		}
	}

	_vector Target = XMLoadFloat4(&m_Points[m_iCurrentFlow][m_iCurrentPoint]);
	_vector NextTarget;

	_vector CurPos = m_pCharacter_Controller->Get_Position();
	_vector vLook = XMVector3Normalize(m_pTransformCom->Get_State(STATE::LOOK));


	_vector toTarget = Target - CurPos;
	_float fDist = XMVectorGetX(XMVector3Length(toTarget));

	if (m_iCurrentPoint + 1 < m_Points[m_iCurrentFlow].size() && fDist < 15.f)
	{
		 NextTarget = XMLoadFloat4(&m_Points[m_iCurrentFlow][m_iCurrentPoint + 1]);
	}
	else {
		NextTarget = Target;
	}

	if (m_iCurrentPoint == m_Points[m_iCurrentFlow].size() - 1)
	{
		if (fDist < 5.f)
		{
			if (m_iCurrentPoint == m_Points[m_iCurrentFlow].size() - 1)
			{
				m_bTucked = true;
				m_pFSM->Disable_State(FSMSTATE::TUCKED);
			}

			m_iCurrentPoint = (m_iCurrentPoint + 1) % m_Points[m_iCurrentFlow].size();
			return;
		}
	}
	else if (fDist < 10.f)
	{
		m_iCurrentPoint = (m_iCurrentPoint + 1) % m_Points[m_iCurrentFlow].size();
		return;
	}

	_vector LerpTarget = XMVectorLerp(Target, NextTarget, 0.5f);

	m_pTransformCom->LookAt_Lerp(LerpTarget, fTimeDelta,5.f);

	m_pCharacter_Controller->Set_Position(CurPos + vLook * m_fTuckedSpeed * fTimeDelta);
}

HRESULT CRanrok::Load_RanrokPos(const _char* pFilePath)
{
	tinyxml2::XMLDocument doc;
	if (doc.LoadFile(pFilePath) != tinyxml2::XML_SUCCESS)
		return E_FAIL;

	tinyxml2::XMLElement* pSpellLearnInfo = doc.FirstChildElement("RanrokPosInfo");
	if (!pSpellLearnInfo) return E_FAIL;

	tinyxml2::XMLElement* pSpellLearnData = pSpellLearnInfo->FirstChildElement("RanrokPosData");
	if (!pSpellLearnData) return E_FAIL;

	tinyxml2::XMLElement* pName = pSpellLearnData->FirstChildElement("Name");
	while (pName)
	{
		_int iCurrentFlow;
		pName->QueryIntAttribute("Flow", &iCurrentFlow);

		if (iCurrentFlow >= m_Points.size())
			m_Points.resize(iCurrentFlow + 1);

		tinyxml2::XMLElement* pPosition = pName->FirstChildElement("Position");
		while (pPosition)
		{
			tinyxml2::XMLElement* pPos = pPosition->FirstChildElement("Pos");
			if (pPos)
			{
				_float px{}, py{}, pz{};
				pPos->QueryFloatAttribute("x", &px);
				pPos->QueryFloatAttribute("y", &py);
				pPos->QueryFloatAttribute("z", &pz);
				m_Points[iCurrentFlow].emplace_back(px, py, pz, 1.f);
			}
			pPosition = pPosition->NextSiblingElement("Position");
		}
		pName = pName->NextSiblingElement("Name");
	}

	return S_OK;
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

	for (auto& Props : m_pRanrok_Props) {
		SAFE_RELEASE(Props);
	}
	for (_uint i = 0; i < ENUM_CLASS(RANROK_ENUM_BONEMATRICES::END); ++i) {
		SAFE_RELEASE(m_pTargetableDO[i]);
	}
	m_pRanrok_Props.clear();
	SAFE_RELEASE(m_pDepthStencilStateNone);
	SAFE_RELEASE(m_pMotionTrailCom);
	SAFE_RELEASE(m_pCharacter_Controller);
	SAFE_RELEASE(m_pRigidBody);
	SAFE_RELEASE(m_pEffectPool);
	SAFE_RELEASE(m_pRanrok_Point);
	SAFE_RELEASE(m_pRightSmoke);
	SAFE_RELEASE(m_pLeftSmoke);
	SAFE_RELEASE(m_pBottomSmoke);
	SAFE_RELEASE(m_pLeftEye_Trail);
	SAFE_RELEASE(m_pRightEye_Trail);
	SAFE_RELEASE(m_pRightPt);
	SAFE_RELEASE(m_pLeftPt);

	Safe_Delete(m_pCallBack_Behavior);
	Safe_Delete(m_pCallBack_HitReport);
}
#ifdef _DEBUG

void CRanrok::Describe_Entity()
{
	GUI::Begin("UNIT", 0, IMGUI_GLOBAL_BEGIN_FLAG);
	if (GUI::CollapsingHeader("Ranrak")) {
		__super::Describe_Entity();
		if (GUI::SmallButton("ResetPos")) {
			m_pTransformCom->Set_State(STATE::POSITION, XMVectorSetW(XMLoadFloat3(&m_vInitialTrans), 1.f));
			m_pCharacter_Controller->Set_Position(m_pTransformCom->Get_State(STATE::POSITION));
			m_pTransformCom->RotationQ(XMLoadFloat4(&m_vInitialRotQ));
			m_iCurrentFlow = 0;
			m_iCurrentPoint = 0;
		}
		string AnimList = m_pModelCom->Get_AnimList(m_pModelCom->Get_AnimIndex());
		GUI::Text(AnimList.c_str());

		GUI::Text("AnimTrack %.2f", m_pModelCom->Get_CurrentTrackPosition());
		GUI::Text("AnimRatio %.2f", m_pModelCom->Get_CurrentTrackProgressRatio());
		GUI::Text("AnimSpeed %.2f", m_pModelCom->Get_AnimSpeed());

		GUI::Text("Degree %.2f", m_fDegree);
		GUI::Text("CurrFlow %d", m_iCurrentFlow);

		_float3 Pos;
		XMStoreFloat3(&Pos, Get_WorldPostion());

		if (GUI::DragFloat3("Pos", (_float*)&Pos, 0.01f))
		{
			m_pCharacter_Controller->Set_Position(XMVectorSetW(XMLoadFloat3(&Pos), 1.f));
		}

		if (GUI::Button("MovePos"))
		{
			m_pCharacter_Controller->Set_Position(XMVectorSet(m_pGameInstance->Get_CamPosition()->x, m_pGameInstance->Get_CamPosition()->y, m_pGameInstance->Get_CamPosition()->z, 1.f));
		}

		if (GUI::Button("Create Prop"))
		{
			m_vCreatePropTime.x = m_vCreatePropTime.y;
		}

		for (_uint i = 0; i < m_Points.size(); ++i)
		{
			GUI::Separator();

			if (GUI::TreeNode(("Points " + to_string(i)).c_str()))
			{
				if (GUI::SmallButton("Copy"))
				{
					char buf[128];
					sprintf_s(buf, "<Pos x=\"%.3f\" y=\"%.3f\" z=\"%.3f\"/>", Pos.x, Pos.y, Pos.z);

					GUI::SetClipboardText(buf);

					_float4 vPos = { Pos.x, Pos.y, Pos.z, 1.f };
					m_Points[i].push_back(vPos);
				}

				GUI::Text("Points[%d] Count : %d", i, (_int)m_Points[i].size());

				for (_uint j = 0; j < m_Points[i].size(); )
				{
					_float3 p = { m_Points[i][j].x,m_Points[i][j].y,m_Points[i][j].z };

					GUI::PushID((int)(i * 10000 + j));

					GUI::Text("[%d] (%.2f, %.2f, %.2f)", j, p.x, p.y, p.z);
					GUI::SameLine();

					if (GUI::SmallButton("Move"))
					{
						m_pCharacter_Controller->Set_Position(
							XMVectorSet(p.x, p.y, p.z, 1.f));
					}

					GUI::SameLine();

					if (GUI::SmallButton("X"))
					{
						m_Points[i].erase(m_Points[i].begin() + j);
						GUI::PopID();
						continue;
					}

					GUI::PopID();
					++j;
				}

				if (GUI::SmallButton(("Clear##" + to_string(i)).c_str()))
				{
					m_Points[i].clear();
				}
				GUI::TreePop();
			}

		}
		m_pTransformCom->Describe_Entity();
		m_pModelCom->Describe_Entity();
	}
	GUI::End();
}

HRESULT CRanrok::Render_Collider()
{
	static _bool s_bRenderCollider = { true };
	GUI::Begin("CutScene");
	if (GUI::TreeNode("RenderCollider")) {
		if (true == s_bRenderCollider) {
			for (_uint i = 0; i < ENUM_CLASS(RANROK_ENUM_BONEMATRICES::END); ++i) {
				m_pTargetableDO[i]->Render([this]() {
					m_pContext->OMSetDepthStencilState(m_pDepthStencilStateNone, 0);
					}, CMyTools::ColorRGB_A_HEXtoVECTOR(0x00ff00, 1.f));
			}
		}
		GUI::TreePop();
	}
	GUI::End();
	return S_OK;
}

#endif // _DEBUG
