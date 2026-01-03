#include "pch.h"
#include "Goblin_Assassin.h"

#include "GameInstance.h"
#include "InfoInstance.h"
#include "Layer.h"
#include "Player.h"
#include "Goblin_Sword.h"
#include "Goblin_Assassin_Spector.h"
#include "Effect_Container.h"
#include "EffectPool.h"
#include "MapElement_Interactable.h"

#include "EffectParts.h"


CGoblin_Assassin::CGoblin_Assassin(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CMonster(pDevice, pContext)
{
}

CGoblin_Assassin::CGoblin_Assassin(const CGoblin_Assassin& Prototype)
	: CMonster(Prototype)
{
}

HRESULT CGoblin_Assassin::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CGoblin_Assassin::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	if (FAILED(Ready_Parts()))
		return E_FAIL;

	Add_FSM();

	Load_AnimXML("../Bin/Resources/Data/AnimList/Goblin_Assassin.xml");

	{
		CFSM::FSM_DESC FSMDesc{};
		FSMDesc.pStates = &m_States;
		FSMDesc.pStateMask = &m_iStateMask;

		m_pFSM->Bind_States(FSMDesc);
		m_pFSM->Change_State(FSMSTATE::IDLE);
	}

	m_pCallBack_Behavior->Initialize(m_pCharacter_Controller, m_pRigidBody);
	m_pCallBack_HitReport->Initialize(m_pCharacter_Controller, m_pRigidBody);


	m_pCharacter_Controller->Set_Position(XMVectorSet(-52.f, 0.f, -14.f, 1.f));
	m_pTransformCom->Set_State(STATE::POSITION, XMVectorSet(-52.f, 0.f, -14.f, 1.f));


	m_pEffectPool = m_pGameInstance->Get_Layer(NEXT_LEVEL, TEXT("Layer_EffectPool"))->Get_Object<CEffectPool>();
	SAFE_ADDREF(m_pEffectPool);

	m_pModelCom->Set_DisableRootMotionScale(true);

	return S_OK;
}

void CGoblin_Assassin::Priority_Update(_float fTimeDelta)
{
	__super::Priority_Update(fTimeDelta);
}

void CGoblin_Assassin::Update(_float fTimeDelta)
{
	if (m_bVisible == false)
		return;

	m_pFSM->Update_State(fTimeDelta);

	m_pModelCom->Play_Animation(fTimeDelta * m_fEasing, m_pTransformCom);

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

	for (_uint i = 0; i < ENUM_CLASS(GOBLIN_ASSASSIN_SKILL::END); i++)
		m_fSkillCoolTime[i] = max(0.f, m_fSkillCoolTime[i] - fTimeDelta);


	//m_pDetection->Set_Active(m_bDetection);


	if (m_bDisolve) {
		m_fDisolveTime += fTimeDelta*0.8f;
		if (m_fDisolveTime >= 1.f)
		{
			m_fDisolveTime = 0.f;
			m_bDisolve = false;
		}
	}
}

void CGoblin_Assassin::Late_Update(_float fTimeDelta)
{
	if (m_bVisible == false)
		return;

	__super::Late_Update(fTimeDelta);
	if (true == m_pCharacter_Controller->IsActive()) {
		m_pTransformCom->Set_State(STATE::POSITION, m_pCharacter_Controller->Get_FootPosition());
	}
	else if (true == m_pRigidBody->IsActive()) {
		m_pTransformCom->Set_WorldMatrix(m_pRigidBody->Get_FootPositionPxTransform());
	}
	if (true == m_bLookAt) {
		m_pTransformCom->LookAt_Horizontal_Lerp(XMLoadFloat4(&m_vTargetPos), fTimeDelta, 3.f);
	}

	m_pGameInstance->Add_RenderGroup(RENDER::NONBLEND, this);

	Set_Shadow(m_pGameInstance->IsIn_ShadowViewFrustum(m_pTransformCom->Get_State(STATE::POSITION), m_pTransformCom->Get_Radius()));
}

HRESULT CGoblin_Assassin::Render()
{
	if (FAILED(Bind_ShaderResources())) {
		return E_FAIL;
	}

	if (FAILED(Render_DeadDisolve())) {
		return E_FAIL;
	}

	if (FAILED(Render_Disolve())) {
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

	if (true == m_bDrawOutLine) {
		Render_OutLine();
	}

#ifdef _DEBUG
	//if (true == m_pCharacter_Controller->IsActive()) {
	//	if (FAILED(m_pCharacter_Controller->Render())) {
	//		return E_FAIL;
	//	}
	//}
	//else {
	//	if (FAILED(m_pRigidBody->Render())) {
	//		return E_FAIL;
	//	}
	//}
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

HRESULT CGoblin_Assassin::Render_OutLine()
{
	m_bDrawOutLine = false;
	if (FAILED(Bind_ShaderResources())) {
		return E_FAIL;
	}

	Compute_Depth();
	_float fCamFar = *m_pGameInstance->Get_CurrentCameraFar();
	_float fRatio = CMyTools::Saturate((m_fCamDepth / (fCamFar * fCamFar)));
	m_fOutLineThickness = CMyTools::Lerp_f1D(0.024f, 0.5f, fRatio);
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

HRESULT CGoblin_Assassin::Render_Shadow(SHADOW eType)
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

		if (FAILED(m_pModelCom->Begin(i, m_pShaderCom))) {
			return E_FAIL;
		}

		m_pModelCom->Bind_OutPut_SRV_VS(26, 0);

		if (FAILED(m_pModelCom->Render(i))) {
			return E_FAIL;
		}
	}

	return S_OK;
}

_vector CGoblin_Assassin::Get_LockOnPos()
{
	if (nullptr != m_pCharacter_Controller && true == m_pCharacter_Controller->IsActive()) {
		return m_pCharacter_Controller->Get_Position();
	}
	else if (nullptr != m_pRigidBody) {
		return m_pRigidBody->Get_Position();
	}
	return Get_WorldPostion();
}

void CGoblin_Assassin::OnCollision(CGameObject* pOther, void* pDesc)
{
	if (true == m_bDead) {
		return;
	}
	if (m_pFSM->IsEnable(FSMSTATE::BLINK)) {
		return;
	}
	XMStoreFloat4(&m_DamageInfo.vTarget_Pos, m_pCharacter_Controller->Get_HeadPosition());

	m_pGoblinSpector->Set_Visible(false);
	ON_COLLISION_INFO* CollisionDesc = static_cast<ON_COLLISION_INFO*>(pDesc);

	Check_HitAngle(XMLoadFloat4(&CollisionDesc->vHitDir));

	m_fHitRadius = CMyTools::Get_Direction2D(m_pTransformCom->Get_State(STATE::LOOK), XMLoadFloat4(&CollisionDesc->vHitDir));
	
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
			m_fTumbleTimer = 0.f;
			break;
		case ENUM_CLASS(SKILL_TYPE::LEVIOSO):
			m_eHitSpell = ENUM_CLASS(SKILL_TYPE::LEVIOSO);
			m_eHitState = ENUM_CLASS(HIT_STATE::AIR_LEVIOSO);
			break;
		case ENUM_CLASS(SKILL_TYPE::ACCIO):
			m_eHitSpell = ENUM_CLASS(SKILL_TYPE::ACCIO);
			m_eHitState = ENUM_CLASS(HIT_STATE::AIR_LEVIOSO);
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
		_int ID = m_pStat->Get_Stat().iObjectID;
		m_pInfoInstance->Event_CallBack(TEXT("MonsterDead"), &ID);
		return;
	}


	m_pFSM->Change_State(FSMSTATE::HIT);
	

}

void CGoblin_Assassin::OnHit(CGameObject* pOther, CGameObject* pCaller)
{
}

void CGoblin_Assassin::Set_Detection(_bool bDetection)
{
	m_bDetection = bDetection;
}

HRESULT CGoblin_Assassin::Ready_Components()
{
	CTransform::TRANSFORM_DESC Desc = {};

	Desc.fSpeedPerSec = 10.f;
	Desc.fRotationPerSec = XMConvertToRadians(180.0f);
	Desc.fRadius = 10.f;

	if (FAILED(Add_Component<CTransform>(g_iStaticLevel, &m_pTransformCom, &Desc))) {
		return E_FAIL;
	}
	if (FAILED(Add_Component<CFSM>(g_iStaticLevel, &m_pFSM))) {
		return E_FAIL;
	}
	/* Com_Shader */
	if (FAILED(__super::Add_Asset_Component(g_iStaticLevel, FX_NPC_PBR_ANIM,
		reinterpret_cast<CComponent**>(&m_pShaderCom)))) {
		return E_FAIL;
	}

	m_strModelPrototypeTag = TEXT("Prototype_Component_Goblin_Assassin_Model");

	/* Com_Model */
	if (FAILED(__super::Add_Asset_Component(g_iStaticLevel, m_strModelPrototypeTag,
		reinterpret_cast<CComponent**>(&m_pModelCom)))) {
		return E_FAIL;
	}

	{ // CCT
		CCharacter_Controller::Character_Controller_DESC Desc{};

		Desc.iSubKind = ENUM_CLASS(PXOBJECT::GOBLIN_WARRIOR);
		Desc.pTransform = m_pTransformCom;
		Desc.eBodyType = ACTOR::CAPSULE;
		Desc.fContactOffset = 0.001f;
		Desc.fMaterial = { 1.2f, 1.0f, 0.0f };
		Desc.bAutoStepping = { false };
		Desc.fStepOffset = { 0.02f };
		Desc.fRadius = 0.6f;
		Desc.fHeight = 0.7f;
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
		Desc.iSubKind = ENUM_CLASS(PXOBJECT::GOBLIN_WARRIOR);
		if (FAILED(Add_Asset_Component(g_iStaticLevel, TEXT("PHYSX_DYNAMIC_BOX"), (CComponent**)&m_pRigidBody, &Desc))) {
			return E_FAIL;
		}
		m_pGameInstance->Detach_Actor(*m_pRigidBody->Get_Actor(), NEXT_LEVEL);
	}

	if (FAILED(Add_Asset_Component(g_iStaticLevel, TEXT("STAT_GOBLIN_ASSASSIN"), (CComponent**)&m_pStat))) {
		return E_FAIL;
	}
	return S_OK;
}

HRESULT CGoblin_Assassin::Ready_Parts()
{
	{
		CGoblin_Sword::GOBLIN_SWORD_DESC Goblin_SwordDesc{};

		Goblin_SwordDesc.pParentTransform = m_pTransformCom;
		Goblin_SwordDesc.pSocketMatrices = m_pModelCom->Get_BoneMatrixPtr("SKT_LeftHand");
		Goblin_SwordDesc.iIndex = 0;

		if (FAILED(Add_PartObject<CGoblin_Sword>("Goblin_Sword_L", g_iStaticLevel, nullptr, &Goblin_SwordDesc)))
		{
			return E_FAIL;
		}
	}

	{
		CGoblin_Sword::GOBLIN_SWORD_DESC Goblin_SwordDesc{};

		Goblin_SwordDesc.pParentTransform = m_pTransformCom;
		Goblin_SwordDesc.pSocketMatrices = m_pModelCom->Get_BoneMatrixPtr("SKT_RightHand");
		Goblin_SwordDesc.iIndex = 1;

		if (FAILED(Add_PartObject<CGoblin_Sword>("Goblin_Sword_R", g_iStaticLevel, nullptr, &Goblin_SwordDesc)))
		{
			return E_FAIL;
		}
	}


	CGoblin_Assassin_Spector::GOBLIN_SPECTOR_DESC Goblin_SpectorDesc{};

	Goblin_SpectorDesc.pParentTransform = m_pTransformCom;

	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer<CGoblin_Assassin_Spector>(g_iStaticLevel, NEXT_LEVEL, LAYER_MONSTER, &Goblin_SpectorDesc, this, &m_pGoblinSpector))) {
		return E_FAIL;
	}

#pragma region EFFECT
	/* EFFECT */

	CPartObject::PARTOBJECT_DESC PartsDesc{};

	PartsDesc.pParentTransform = m_pTransformCom;

	if (FAILED(Add_PartObject<CEffectParts>("Goblin_Particle", g_iStaticLevel, &m_pGoblin_Particle, &PartsDesc)))
	{
		return E_FAIL;
	}

	m_pGoblin_Particle->Load("../Bin/Resources/Data/Effect/Goblin/GoblinSide/Goblin_Particle", static_cast<LEVEL>(g_iStaticLevel));
	m_pGoblin_Particle->FollowParents(m_pModelCom->Get_BoneMatrixPtr("RightShoulder"));



	if (FAILED(Add_PartObject<CEffectParts>("Goblin_Particle2", g_iStaticLevel, &m_pGoblin_Particle2, &PartsDesc)))
	{
		return E_FAIL;
	}

	m_pGoblin_Particle2->Load("../Bin/Resources/Data/Effect/Goblin/GoblinSide/Goblin_Particle2", static_cast<LEVEL>(g_iStaticLevel));
	m_pGoblin_Particle2->FollowParents(m_pModelCom->Get_BoneMatrixPtr("RightShoulder"));


	if (FAILED(Add_PartObject<CEffectParts>("Goblin_Smoke", g_iStaticLevel, &m_pSmoke, &PartsDesc)))
	{
		return E_FAIL;
	}

	m_pSmoke->Load("../Bin/Resources/Data/Effect/Goblin/GoblinSide/Goblin_Smoke", static_cast<LEVEL>(g_iStaticLevel));
	m_pSmoke->FollowParents(m_pModelCom->Get_BoneMatrixPtr("Spine2"));


#pragma endregion


	//if (FAILED(m_pGameInstance->Add_GameObject_ToLayer<CEnemy_Detection>(g_iStaticLevel, NEXT_LEVEL, LAYER_MONSTER, &PartsDesc, this, &m_pDetection))) {
	//	return E_FAIL;
	//}

	return S_OK;
}

HRESULT CGoblin_Assassin::Bind_ShaderResources()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix"))) {
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
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::VIEW)))) {
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::PROJ)))) {
		return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Bind_RawValue("g_fFar", m_pGameInstance->Get_CurrentCameraFar(), sizeof(_float)))) {
		return E_FAIL;
	}
	return S_OK;
}

CGoblin_Assassin* CGoblin_Assassin::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CGoblin_Assassin* pInstance = new CGoblin_Assassin(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CGoblin_Assassin");
		SAFE_RELEASE(pInstance);
	}
	return pInstance;
}

CGameObject* CGoblin_Assassin::Clone(void* pArg, CGameObject* pOwner)
{
	CGoblin_Assassin* pInstance = new CGoblin_Assassin(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CGoblin_Assassin");
		SAFE_RELEASE(pInstance);
	}
	return pInstance;
}

void CGoblin_Assassin::Free()
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
	SAFE_RELEASE(m_pSmoke);
	SAFE_RELEASE(m_pGoblin_Particle);
	SAFE_RELEASE(m_pGoblin_Particle2);
	SAFE_RELEASE(m_pEffectPool);
	Safe_Delete(m_pCallBack_Behavior);
	Safe_Delete(m_pCallBack_HitReport);
}
#ifdef _DEBUG

void CGoblin_Assassin::Describe_Entity()
{
	GUI::Begin("UNIT", 0, IMGUI_GLOBAL_BEGIN_FLAG);
	GUI::PushItemWidth(IMGUI_GLOBAL_ITEM_WIDTH);
	if (GUI::CollapsingHeader("Goblin")) {
		GUI::Checkbox("LookAt", &m_bLookAt);

		string AnimList = m_pModelCom->Get_AnimList(m_pModelCom->Get_AnimIndex());
		GUI::Text(AnimList.c_str());

		GUI::Text("AnimTrack %.2f", m_pModelCom->Get_CurrentTrackPosition());
		GUI::Text("AnimRatio %.2f", m_pModelCom->Get_CurrentTrackProgressRatio());

		_float4 vMomentum = {};
		XMStoreFloat4(&vMomentum, m_pTransformCom->Get_CurrentMomentum());
		GUI::Text("%.2f %.2f %.2f %.2f ", vMomentum.x, vMomentum.y, vMomentum.z, vMomentum.w);

		_float3 Pos;
		XMStoreFloat3(&Pos, Get_WorldPostion());

		if (GUI::DragFloat3("Pos", (_float*)&Pos))
		{
			m_pCharacter_Controller->Set_Position(XMLoadFloat3(&Pos));
		}

		XMFLOAT3 f3;
		XMStoreFloat3(&f3, m_vOriginPos);

		GUI::Text("Origin: %.2f, %.2f, %.2f", f3.x, f3.y, f3.z);

		m_fLength = XMVectorGetX(XMVector2Length(m_pTransformCom->Get_State(STATE::POSITION) - m_vOriginPos));

		GUI::Text("Length %.2f", m_fLength);

		GUI::TextColored(ImVec4(1.f, 0.f, 0.f, 1.f), "Distance %.2f", m_fTargetDistance);
	}
	GUI::End();
}

#endif // _DEBUG
