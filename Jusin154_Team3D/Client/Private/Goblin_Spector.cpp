#include "pch.h"
#include "Goblin_Spector.h"

#include "GameInstance.h"
#include "Effect_Container.h"

#pragma region STATE
#include "State_Idle.h"
#include "State_Dodge.h"
#include "State_Jump.h"
#include "State_Land.h"
#include "State_Move.h"
#include "State_Combat.h"
#pragma endregion

#include "EffectParts.h"


CGoblin_Spector::CGoblin_Spector(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CMonster(pDevice, pContext)
{
}

CGoblin_Spector::CGoblin_Spector(const CGoblin_Spector& Prototype)
	: CMonster(Prototype)
{
}

HRESULT CGoblin_Spector::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CGoblin_Spector::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	if (FAILED(Ready_Parts()))
		return E_FAIL;


	return S_OK;
}

void CGoblin_Spector::Priority_Update(_float fTimeDelta)
{
	__super::Priority_Update(fTimeDelta);
}

void CGoblin_Spector::Update(_float fTimeDelta)
{
	__super::Update(fTimeDelta);

	m_pModelCom->Play_Animation(fTimeDelta, m_pTransformCom);

	Play_Event();

	GUI::Text("%d", m_pCharacter_Controller->IsActive());
	GUI::Text("%f %f", m_vStunTimer.x, m_vStunTimer.y);
	if (true == m_pCharacter_Controller->IsActive()) {
		m_pCharacter_Controller->Move(fTimeDelta);
		m_vStunTimer.x = 0.f;
	}
	else {
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

}

void CGoblin_Spector::Late_Update(_float fTimeDelta)
{
	__super::Late_Update(fTimeDelta);
	if (true == m_pCharacter_Controller->IsActive()) {
		m_pTransformCom->Set_State(STATE::POSITION, m_pCharacter_Controller->Get_FootPosition());
	}
	else {
		m_pTransformCom->Set_WorldMatrix(m_pRigidBody->Get_FootPositionPxTransform());
	}


	m_pTransformCom->LookAt_Horizontal(XMLoadFloat4(&m_vTargetPos));

	m_pGameInstance->Add_RenderGroup(RENDER::NONBLEND, this);
}

HRESULT CGoblin_Spector::Render()
{
	if (!m_bVisible)
		return S_OK;

	if (FAILED(Bind_ShaderResources())) {
		return E_FAIL;
	}

	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();
	_uint iShaderPass = ENUM_CLASS(SHADER_PASS_ANIM::SPECTOR);

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


#ifdef _DEBUG
	if (true == m_pCharacter_Controller->IsActive()) {
		if (FAILED(m_pCharacter_Controller->Render())) {
			return E_FAIL;
		}
	}
	else {
		if (FAILED(m_pRigidBody->Render())) {
			return E_FAIL;
		}
	}
#endif
	return S_OK;
}

_vector CGoblin_Spector::Get_LockOnPos()
{
	if (nullptr != m_pCharacter_Controller && true == m_pCharacter_Controller->IsActive()) {
		return m_pCharacter_Controller->Get_Position();
	}
	else if (nullptr != m_pRigidBody) {
		return m_pRigidBody->Get_Position();
	}
	return Get_WorldPostion();
}

void CGoblin_Spector::OnCollision(CGameObject* pOther, void* pDesc)
{
	/*if (true == m_bDead) {
		return;
	}
	ON_COLLISION_INFO* CollisionDesc = static_cast<ON_COLLISION_INFO*>(pDesc);

	_uint iSkillType = dynamic_cast<CEffect_Container*>(pOther)->Get_SkillType();
	m_fHitRadius = CMyTools::Get_Direction2D(m_pTransformCom->Get_State(STATE::LOOK), XMLoadFloat4(&CollisionDesc->vHitDir));
	switch (iSkillType)
	{
	case ENUM_CLASS(SKILL_TYPE::DESCENDO):
		m_eHitSpell = STATEANIM::KNOCKDOWN_FWD;
		break;
	case ENUM_CLASS(SKILL_TYPE::FLIPENDO):
		m_eHitSpell = STATEANIM::TUMBLE2;
		break;
	case ENUM_CLASS(SKILL_TYPE::JAP):
		m_eHitSpell = STATEANIM::HIT_LEVIOSO;
		if (true == Get_Damage(60.f)) {
			m_pFSM->Change_State(FSMSTATE::DEAD);
			return;
		}
		break;
	case ENUM_CLASS(SKILL_TYPE::LEVIOSO):
		m_eHitSpell = STATEANIM::HIT_LEVIOSO;
		break;
	default:
		m_eHitSpell = STATEANIM::KNOCKDOWN_FWD;
		break;
	}
	if (!m_pFSM->IsEnable(FSMSTATE::BLINK)) {
		m_pFSM->Change_State(FSMSTATE::HIT);
	}
*/
}

void CGoblin_Spector::OnHit(CGameObject* pOther, CGameObject* pCaller)
{
}

HRESULT CGoblin_Spector::Ready_Components()
{
	CTransform::TRANSFORM_DESC Desc = {};

	Desc.fSpeedPerSec = 10.f;
	Desc.fRotationPerSec = XMConvertToRadians(180.0f);
	Desc.fRadius = 10.f;

	__super::Ready_Components(&Desc);

	m_strModelPrototypeTag = TEXT("Prototype_Component_Goblin_Spector_Model");

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
		Desc.iSubKind = ENUM_CLASS(PXOBJECT::GOBLIN_WARRIOR);
		if (FAILED(Add_Asset_Component(g_iStaticLevel, TEXT("PHYSX_DYNAMIC_BOX"), (CComponent**)&m_pRigidBody, &Desc))) {
			return E_FAIL;
		}
		m_pGameInstance->Detach_Actor(*m_pRigidBody->Get_Actor());
	}

	if (FAILED(Add_Asset_Component(g_iStaticLevel, TEXT("STAT_GOBLIN"), (CComponent**)&m_pStat))) {
		return E_FAIL;
	}
	return S_OK;
}

HRESULT CGoblin_Spector::Ready_Parts()
{
#pragma region EFFECT
	/* EFFECT */

	CPartObject::PARTOBJECT_DESC PartsDesc{};

	PartsDesc.pParentTransform = m_pTransformCom;



	if (FAILED(Add_PartObject<CEffectParts>("Goblin_Particle", g_iStaticLevel, &m_pGoblin_Particle, &PartsDesc)))
	{
		return E_FAIL;
	}

	m_pGoblin_Particle->Load("../Bin/Resources/Data/Effect/Goblin/GoblinSide/Goblin_Particle", static_cast<LEVEL>(NEXT_LEVEL));
	m_pGoblin_Particle->FollowParants(m_pModelCom->Get_BoneMatrixPtr("RightShoulder"));



	if (FAILED(Add_PartObject<CEffectParts>("Goblin_Particle2", g_iStaticLevel, &m_pGoblin_Particle2, &PartsDesc)))
	{
		return E_FAIL;
	}

	m_pGoblin_Particle2->Load("../Bin/Resources/Data/Effect/Goblin/GoblinSide/Goblin_Particle2", static_cast<LEVEL>(NEXT_LEVEL));
	m_pGoblin_Particle2->FollowParants(m_pModelCom->Get_BoneMatrixPtr("RightShoulder"));


	if (FAILED(Add_PartObject<CEffectParts>("Goblin_Smoke", g_iStaticLevel, &m_pSmoke, &PartsDesc)))
	{
		return E_FAIL;
	}

	m_pSmoke->Load("../Bin/Resources/Data/Effect/Goblin/GoblinSide/Goblin_Smoke", static_cast<LEVEL>(NEXT_LEVEL));
	m_pSmoke->FollowParants(m_pModelCom->Get_BoneMatrixPtr("Spine2"));


#pragma endregion
	return S_OK;

	return S_OK;
}

HRESULT CGoblin_Spector::Bind_ShaderResources()
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

CGoblin_Spector* CGoblin_Spector::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CGoblin_Spector* pInstance = new CGoblin_Spector(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CGoblin_Spector");
		SAFE_RELEASE(pInstance);
	}
	return pInstance;
}

CGameObject* CGoblin_Spector::Clone(void* pArg, CGameObject* pOwner)
{
	CGoblin_Spector* pInstance = new CGoblin_Spector(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CGoblin_Spector");
		SAFE_RELEASE(pInstance);
	}
	return pInstance;
}

void CGoblin_Spector::Free()
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
	Safe_Delete(m_pCallBack_Behavior);
	Safe_Delete(m_pCallBack_HitReport);
}
#ifdef _DEBUG

void CGoblin_Spector::Describe_Entity()
{
}

#endif // _DEBUG
