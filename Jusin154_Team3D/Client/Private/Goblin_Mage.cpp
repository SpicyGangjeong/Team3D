#include "pch.h"
#include "Goblin_Mage.h"

#include "Layer.h"
#include "EffectPool.h"
#include "GameInstance.h"
#include "InfoInstance.h"
#include "Goblin_Dagger.h"
#include "Effect_Container.h"
#include "EffectParts.h"

#pragma region STATE
#include "State_Idle.h"
#include "State_Dodge.h"
#include "State_Jump.h"
#include "State_Land.h"
#include "State_Move.h"
#include "State_Combat.h"
#pragma endregion


CGoblin_Mage::CGoblin_Mage(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CMonster(pDevice, pContext)
{
}

CGoblin_Mage::CGoblin_Mage(const CGoblin_Mage& Prototype)
	: CMonster(Prototype)
{
}

HRESULT CGoblin_Mage::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CGoblin_Mage::Initialize(void* pArg)
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

	m_pEffectPool = m_pGameInstance->Get_Layer(NEXT_LEVEL, TEXT("Layer_EffectPool"))->Get_Object<CEffectPool>();
	SAFE_ADDREF(m_pEffectPool);

	m_pCharacter_Controller->Set_Position(XMVectorSet(-50.f, 5.f, -10.f, 1.f));

	return S_OK;
}

void CGoblin_Mage::Priority_Update(_float fTimeDelta)
{
	__super::Priority_Update(fTimeDelta);
}

void CGoblin_Mage::Update(_float fTimeDelta)
{
	__super::Update(fTimeDelta);

	m_pFSM->Update_State(fTimeDelta);

	m_pModelCom->Play_Animation(fTimeDelta, m_pTransformCom);

	Play_Event();
#ifdef _DEBUG

	GUI::Text("%d", m_pCharacter_Controller->IsActive());
	GUI::Text("%f %f", m_vStunTimer.x, m_vStunTimer.y);
#endif // _DEBUG

	if (true == m_pCharacter_Controller->IsActive()) {
		m_pCharacter_Controller->Move(fTimeDelta);
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

	for (_uint i = 0; i < ENUM_CLASS(GOBLIN_SKILL::END); i++){
		m_fSkillCoolTime[i] = max(0.f, m_fSkillCoolTime[i] - fTimeDelta);
	}

}

void CGoblin_Mage::Late_Update(_float fTimeDelta)
{
	__super::Late_Update(fTimeDelta);
	if (true == m_pCharacter_Controller->IsActive()) {
		m_pTransformCom->Set_State(STATE::POSITION, m_pCharacter_Controller->Get_FootPosition());
	}
	else if (true == m_pRigidBody->IsActive()) {
		m_pTransformCom->Set_WorldMatrix(m_pRigidBody->Get_FootPositionPxTransform());
	}

	if (true == m_bLookAt) {
		m_pTransformCom->LookAt_Horizontal(XMLoadFloat4(&m_vTargetPos));
	}

	m_pGameInstance->Add_RenderGroup(RENDER::NONBLEND, this);
	m_pGameInstance->Add_RenderGroup(RENDER::SHADOW, this);
}

HRESULT CGoblin_Mage::Render()
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
	for (_uint i = 0; i < iNumMeshes; i++)
	{
		if (FAILED(m_pModelCom->Bind_BoneMatrices(i, m_pShaderCom, "g_BoneMatrices"))) {
			return E_FAIL;
		}

		if (FAILED(m_pModelCom->Bind_Material(i, m_pShaderCom))) {
			return E_FAIL;
		}

		if (FAILED(m_pShaderCom->Begin(iShaderPass))) {
			return E_FAIL;
		}

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
	return S_OK;
}

HRESULT CGoblin_Mage::Render_Shadow()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix"))) {
		return E_FAIL;
	}
	if (FAILED(m_pGameInstance->Bind_Shadow_Resource(m_pShaderCom, "g_ViewMatrix", D3DTS::VIEW))) {
		return E_FAIL;
	}
	if (FAILED(m_pGameInstance->Bind_Shadow_Resource(m_pShaderCom, "g_ProjMatrix", D3DTS::PROJ))) {
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fFar", &m_pGameInstance->Get_ShadowDesc()->fFar, sizeof(_float)))) {
		return E_FAIL;
	}
	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();
	for (_uint i = 0; i < iNumMeshes; i++)
	{
		if (FAILED(m_pModelCom->Bind_BoneMatrices(i, m_pShaderCom, "g_BoneMatrices"))) {
			return E_FAIL;
		}
		if (FAILED(m_pShaderCom->Begin(ENUM_CLASS(SHADER_PASS_ANIM::SHADOW)))) {
			return E_FAIL;
		}

		if (FAILED(m_pModelCom->Render(i))) {
			return E_FAIL;
		}
	}

	return S_OK;
}

_vector CGoblin_Mage::Get_LockOnPos()
{
	if (nullptr != m_pCharacter_Controller && true == m_pCharacter_Controller->IsActive()) {
		return m_pCharacter_Controller->Get_Position();
	}
	else if (nullptr != m_pRigidBody) {
		return m_pRigidBody->Get_Position();
	}
	return Get_WorldPostion();
}

void CGoblin_Mage::OnCollision(CGameObject* pOther, void* pDesc)
{

	ON_COLLISION_INFO* CollisionDesc = static_cast<ON_COLLISION_INFO*>(pDesc);

	_uint iSkillType = dynamic_cast<CEffect_Container*>(pOther)->Get_SkillType();
	switch (iSkillType)
	{
	case ENUM_CLASS(SKILL_TYPE::DESCENDO):
		m_eHitSpell = STATEANIM::KNOCKDOWN_FWD;
		break;
	case ENUM_CLASS(SKILL_TYPE::BOMBARDA):
		m_eHitSpell = STATEANIM::KNOCKDOWN_BWD;
		break;
	case ENUM_CLASS(SKILL_TYPE::JAP):
	{
		m_eHitSpell = STATEANIM::HIT_BWD;
		//_float fSkillRatio = m_pInfoInstance->Get_Spell_Info(ENUM_CLASS(SKILL_TYPE::JAP)).fSpell_Damage;
		//_float fCoefficient = CollisionDesc->pObject->Get_Component<CStat>()->Get_Stat().fDamage;
		//if (true == Get_Damage(fSkillRatio * fCoefficient)) {
		//	m_pFSM->Change_State(FSMSTATE::DEAD);
		//	return;
		//}
	}
	break;
	case ENUM_CLASS(SKILL_TYPE::LEVIOSO):
		m_eHitSpell = STATEANIM::HIT_LEVIOSO;
		break;
	default:
		m_eHitSpell = STATEANIM::KNOCKDOWN_FWD;
		break;
	}
	if (!m_pFSM->IsEnable(FSMSTATE::BLINK)){
		m_pFSM->Change_State(FSMSTATE::HIT);
	}
}

void CGoblin_Mage::OnHit(CGameObject* pOther, CGameObject* pCaller)
{
}

HRESULT CGoblin_Mage::Ready_Components()
{
	CTransform::TRANSFORM_DESC Desc = {};

	Desc.fSpeedPerSec = 10.f;
	Desc.fRotationPerSec = XMConvertToRadians(180.0f);
	Desc.fRadius = 10.f;

	__super::Ready_Components(&Desc);

	m_strModelPrototypeTag = TEXT("Prototype_Component_Goblin_Mage_Model");

	/* Com_Model */
	if (FAILED(__super::Add_Asset_Component(g_iStaticLevel, m_strModelPrototypeTag,
		reinterpret_cast<CComponent**>(&m_pModelCom)))) {
		return E_FAIL;
	}

	{ // CCT
		CCharacter_Controller::Character_Controller_DESC Desc{};

		Desc.iSubKind = ENUM_CLASS(PXOBJECT::GOBLIN_MAGICIAN);
		Desc.pTransform = m_pTransformCom;
		Desc.eBodyType = ACTOR::CAPSULE;
		Desc.fContactOffset = 0.001f;
		Desc.fMaterial = { 1.2f, 1.0f, 0.0f };
		Desc.bAutoStepping = { false };
		Desc.fStepOffset = { 0.05f };
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
		Desc.iSubKind = ENUM_CLASS(PXOBJECT::GOBLIN_MAGICIAN);
		if (FAILED(Add_Asset_Component(g_iStaticLevel, TEXT("PHYSX_DYNAMIC_BOX"), (CComponent**)&m_pRigidBody, &Desc))) {
			return E_FAIL;
		}
		m_pGameInstance->Detach_Actor(*m_pRigidBody->Get_Actor());
	}
	if (FAILED(Add_Asset_Component(g_iStaticLevel, TEXT("STAT_GOBLIN_WIZARD"), (CComponent**)&m_pStat))) {
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CGoblin_Mage::Ready_Parts()
{
	CPartObject::PARTOBJECT_DESC PartsDesc{};

	PartsDesc.pParentTransform = m_pTransformCom;

	if (FAILED(Add_PartObject<CEffectParts>("Goblin_Orb", g_iStaticLevel, &m_pGoblin_Orb, &PartsDesc)))
	{
		return E_FAIL;
	}

	m_pGoblin_Orb->Load("../Bin/Resources/Data/Effect/GoblinMage/Orb_P", static_cast<LEVEL>(NEXT_LEVEL));

	m_pGoblin_Orb->FollowParents(m_pModelCom->Get_BoneMatrixPtr("RightHand"));

	return S_OK;
}

HRESULT CGoblin_Mage::Bind_ShaderResources()
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

	if (FAILED(m_pShaderCom->Bind_RawValue("g_fFar", m_pGameInstance->Get_CurrentCameraFar(), sizeof(_float)))) {
		return E_FAIL;
	}
	return S_OK;
}

CGoblin_Mage* CGoblin_Mage::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CGoblin_Mage* pInstance = new CGoblin_Mage(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CGoblin_Mage");
		SAFE_RELEASE(pInstance);
	}
	return pInstance;
}

CGameObject* CGoblin_Mage::Clone(void* pArg, CGameObject* pOwner)
{
	CGoblin_Mage* pInstance = new CGoblin_Mage(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CGoblin_Mage");
		SAFE_RELEASE(pInstance);
	}
	return pInstance;
}

void CGoblin_Mage::Free()
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
	SAFE_RELEASE(m_pEffectPool);
	Safe_Delete(m_pCallBack_Behavior);
	Safe_Delete(m_pCallBack_HitReport);
	SAFE_RELEASE(m_pGoblin_Orb);
}
#ifdef _DEBUG

void CGoblin_Mage::Describe_Entity()
{
	GUI::Begin("Goblin");

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


	GUI::End();
}

#endif // _DEBUG
