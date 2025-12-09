#include "pch.h"
#include "Troll.h"

#include "Troll_Weapon.h"
#include "Troll_Rock.h"
#include "GameInstance.h"
#include "InfoInstance.h"
#include "Effect_Container.h"
#include "EffectParts.h"
#include "EffectPool.h"
#include "Layer.h"
#include "TrailObject.h"

#pragma region STATE
#include "State_Idle.h"
#include "State_Move.h"
#include "State_Combat.h"
#include "State_Hit.h"
#include "Troll_State_Rush.h"
#include "State_Throw.h"
#pragma endregion

CTroll::CTroll(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CMonster(pDevice, pContext)
{
}

CTroll::CTroll(const CTroll& Prototype)
	: CMonster(Prototype)
{
}

HRESULT CTroll::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CTroll::Initialize(void* pArg)
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



	m_pCharacter_Controller->Set_Position(XMVectorSet(3.f, 5.f, -20.f, 1.f));

	m_pEffectPool = m_pGameInstance->Get_Layer(NEXT_LEVEL, TEXT("Layer_EffectPool"))->Get_Object<CEffectPool>();
	SAFE_ADDREF(m_pEffectPool);

	m_pLeftHand_BoneMat = m_pModelCom->Get_BoneMatrixPtr("LeftHand");
	m_pRightHand_BoneMat = m_pModelCom->Get_BoneMatrixPtr("RightHand");
	m_pWeapon_BoneMat = Get_PartObject<CTroll_Weapon>()->Get_Component<CModel>()->Get_BoneMatrixPtr("Bone");

	return S_OK;
}

void CTroll::Priority_Update(_float fTimeDelta)
{
	__super::Priority_Update(fTimeDelta);
}

void CTroll::Update(_float fTimeDelta)
{
	__super::Update(fTimeDelta);

	m_pFSM->Update_State(fTimeDelta);

	m_pModelCom->Play_Animation(fTimeDelta, m_pTransformCom);

	Play_Event();

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


	for (_uint i = 0; i < ENUM_CLASS(TROLL_SKILL::END); i++)
		m_fSkillCoolTime[i] = max(0.f, m_fSkillCoolTime[i] - fTimeDelta);

	


#pragma region TRAIL_UPDATE

	//_matrix WorldMat = m_pTransformCom->Get_XMWorldMatrix();

	//_matrix LeftHandMatrix = {};
	//_matrix RightHandMatrix = {};
	//_matrix WeaponMatrix = {};

	//LeftHandMatrix = XMLoadFloat4x4(m_pLeftHand_BoneMat);
	//RightHandMatrix = XMLoadFloat4x4(m_pRightHand_BoneMat);
	//WeaponMatrix = XMLoadFloat4x4(m_pWeapon_BoneMat);


	//for (int i = 0; i < 3; ++i) {
	//	LeftHandMatrix.r[i] = XMVector3Normalize(LeftHandMatrix.r[i]);
	//	RightHandMatrix.r[i] = XMVector3Normalize(RightHandMatrix.r[i]);
	//	WeaponMatrix.r[i] = XMVector3Normalize(WeaponMatrix.r[i]);
	//}

	//_matrix WeaponSoket = WeaponMatrix * Get_PartObject<CTroll_Weapon>()->Get_Component<CTransform>()->Get_XMWorldMatrix();

	//m_pLeftTrail->Trail_Update(LeftHandMatrix * WorldMat, fTimeDelta);
	//m_pRightTrail->Trail_Update(RightHandMatrix * WorldMat, fTimeDelta);
	//m_pWeaponTrail->Trail_Update(WeaponSoket, fTimeDelta);

#pragma endregion

}

void CTroll::Late_Update(_float fTimeDelta)
{
	__super::Late_Update(fTimeDelta);
	if (true == m_pCharacter_Controller->IsActive()) {
		m_pTransformCom->Set_State(STATE::POSITION, m_pCharacter_Controller->Get_FootPosition());
	}
	else {
		m_pTransformCom->Set_WorldMatrix(m_pRigidBody->Get_Actor()->getGlobalPose());
	}

	if (m_bLookAt) {
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
	m_pGameInstance->Add_RenderGroup(RENDER::SHADOW, this);
}

HRESULT CTroll::Render()
{
	if (FAILED(Bind_ShaderResources())) {
		return E_FAIL;
	}

	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();
	_uint iShaderPass = ENUM_CLASS(SHADER_PASS_ANIM::DEFAULT);
	if (true == m_bDrawOutLine) {
		iShaderPass = ENUM_CLASS(SHADER_PASS_ANIM::OUTLINE_WRITE);
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

	return S_OK;
}

HRESULT CTroll::Render_Shadow()
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

_vector CTroll::Get_LockOnPos()
{
	if (nullptr != m_pCharacter_Controller && true == m_pCharacter_Controller->IsActive()) {
		return m_pCharacter_Controller->Get_Position();
	}
	else if (nullptr != m_pRigidBody) {
		return m_pRigidBody->Get_Position();
	}
	return Get_WorldPostion();
}

void CTroll::OnCollision(CGameObject* pOther, void* pDesc)
{
	ON_COLLISION_INFO* CollisionDesc = static_cast<ON_COLLISION_INFO*>(pDesc);

	_uint iSkillType = dynamic_cast<CEffect_Container*>(pOther)->Get_SkillType();
	switch (iSkillType)
	{
	case ENUM_CLASS(SKILL_TYPE::DESCENDO):
		m_eHitSpell = STATEANIM::KNOCKDOWN_FWD;
		break;
	case ENUM_CLASS(SKILL_TYPE::FLIPENDO):
		m_eHitSpell = STATEANIM::TUMBLE2;
		break;
	default:
		m_eHitSpell = STATEANIM::KNOCKDOWN_FWD;
		break;
	}
	m_pFSM->Change_State(FSMSTATE::HIT);
}

void CTroll::OnHit(CGameObject* pOther, CGameObject* pCaller)
{
}

HRESULT CTroll::Ready_Components()
{
	CTransform::TRANSFORM_DESC Desc = {};

	Desc.fSpeedPerSec = 10.f;
	Desc.fRotationPerSec = XMConvertToRadians(180.0f);
	Desc.fRadius = 10.f;

	__super::Ready_Components(&Desc);

	m_strModelPrototypeTag = TEXT("Prototype_Component_Troll_Model");

	/* Com_Model */
	if (FAILED(__super::Add_Asset_Component(g_iStaticLevel, m_strModelPrototypeTag,
		reinterpret_cast<CComponent**>(&m_pModelCom)))) {
		return E_FAIL;
	}

	{ // CCT
		CCharacter_Controller::Character_Controller_DESC Desc{};

		Desc.iSubKind = ENUM_CLASS(PXOBJECT::TROLL);
		Desc.pTransform = m_pTransformCom;
		Desc.eBodyType = ACTOR::CAPSULE;
		Desc.fContactOffset = 0.3f;
		Desc.fMaterial = { 1.2f, 1.0f, 0.0f };
		Desc.bAutoStepping = { false };
		Desc.fStepOffset = { 0.05f };
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
		Desc.iSubKind = ENUM_CLASS(PXOBJECT::TROLL);
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

HRESULT CTroll::Ready_Parts()
{
	CTroll_Weapon::TROLLWEAPON_DESC Troll_WeaponDesc{};

	Troll_WeaponDesc.pParentTransform = m_pTransformCom;
	Troll_WeaponDesc.pSocketMatrices = m_pModelCom->Get_BoneMatrixPtr("SKT_RightHand");

	if (FAILED(Add_PartObject<CTroll_Weapon>("Troll_Weapon", g_iStaticLevel, nullptr, &Troll_WeaponDesc)))
	{
		return E_FAIL;
	}

	CTroll_Rock::TROLLROCK_DESC Troll_RockDesc{};

	Troll_RockDesc.pParentTransform = m_pTransformCom;
	Troll_RockDesc.pSocketMatrices = m_pModelCom->Get_BoneMatrixPtr("SKT_LeftHand");

	if (FAILED(Add_PartObject<CTroll_Rock>("Troll_Rock", g_iStaticLevel, nullptr, &Troll_RockDesc)))
	{
		return E_FAIL;
	}

	Get_PartObject<CTroll_Rock>()->Set_Visible(false);


#pragma region EFFECT
	/* EFFECT */

	CPartObject::PARTOBJECT_DESC PartsDesc{};

	PartsDesc.pParentTransform = m_pTransformCom;



	if (FAILED(Add_PartObject<CEffectParts>("Troll_Particle", g_iStaticLevel, &m_pTroll_Particle, &PartsDesc)))
	{
		return E_FAIL;
	}

	m_pTroll_Particle->Load("../Bin/Resources/Data/Effect/Troll/TrollSide/Troll_Particle", static_cast<LEVEL>(NEXT_LEVEL));
	m_pTroll_Particle->FollowParants(m_pModelCom->Get_BoneMatrixPtr("HeadEnd"));



	if (FAILED(Add_PartObject<CEffectParts>("Troll_Particle2", g_iStaticLevel, &m_pTroll_Particle2, &PartsDesc)))
	{
		return E_FAIL;
	}

	m_pTroll_Particle2->Load("../Bin/Resources/Data/Effect/Troll/TrollSide/Troll_Particle2", static_cast<LEVEL>(NEXT_LEVEL));
	m_pTroll_Particle2->FollowParants(m_pModelCom->Get_BoneMatrixPtr("HeadEnd"));


	if (FAILED(Add_PartObject<CEffectParts>("Troll_Right_Smoke", g_iStaticLevel, &m_pRight_Smoke, &PartsDesc)))
	{
		return E_FAIL;
	}

	m_pRight_Smoke->Load("../Bin/Resources/Data/Effect/Troll/TrollSide/Troll_Smoke", static_cast<LEVEL>(NEXT_LEVEL));
	m_pRight_Smoke->FollowParants(m_pModelCom->Get_BoneMatrixPtr("RightArm"));

	if (FAILED(Add_PartObject<CEffectParts>("Troll_Left_Smoke", g_iStaticLevel, &m_pLeft_Smoke, &PartsDesc)))
	{
		return E_FAIL;
	}

	m_pLeft_Smoke->Load("../Bin/Resources/Data/Effect/Troll/TrollSide/Troll_Smoke", static_cast<LEVEL>(NEXT_LEVEL));
	m_pLeft_Smoke->FollowParants(m_pModelCom->Get_BoneMatrixPtr("LeftArm"));


	if (FAILED(Add_PartObject<CTrailObject>("Left_Trail", g_iStaticLevel, &m_pLeftTrail, &PartsDesc))) {
		return E_FAIL;
	}
	;

	m_pLeftTrail->Load_Trail("../Bin/Resources/Data/Effect/Troll/TrollSide/Troll_Trail", static_cast<LEVEL>(NEXT_LEVEL));
	m_pLeftTrail->Set_Visible(false);

	if (FAILED(Add_PartObject<CTrailObject>("Right_Trail", g_iStaticLevel, &m_pRightTrail, &PartsDesc))) {
		return E_FAIL;
	}

	m_pRightTrail->Load_Trail("../Bin/Resources/Data/Effect/Troll/TrollSide/Troll_Trail", static_cast<LEVEL>(NEXT_LEVEL));
	m_pRightTrail->Set_Visible(false);

	if (FAILED(Add_PartObject<CTrailObject>("Weapon_Trail", g_iStaticLevel, &m_pWeaponTrail, &PartsDesc))) {
		return E_FAIL;
	}

	m_pWeaponTrail->Load_Trail("../Bin/Resources/Data/Effect/Troll/TrollSide/Troll_Swing_Trail", static_cast<LEVEL>(NEXT_LEVEL));
	m_pWeaponTrail->Set_Visible(false);


#pragma endregion
	return S_OK;
}

HRESULT CTroll::Bind_ShaderResources()
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

void CTroll::Troll_Trail_Visible(_bool isTrailVisible)
{
	m_pLeftTrail->Set_Visible(isTrailVisible);
	m_pRightTrail->Set_Visible(isTrailVisible);

	m_pLeftTrail->Get_Component<CTrail>()->Reset_Trail();
	m_pRightTrail->Get_Component<CTrail>()->Reset_Trail();
}

CTroll* CTroll::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CTroll* pInstance = new CTroll(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CTroll");
		SAFE_RELEASE(pInstance);
	}
	return pInstance;
}

CGameObject* CTroll::Clone(void* pArg, CGameObject* pOwner)
{
	CTroll* pInstance = new CTroll(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CTroll");
		SAFE_RELEASE(pInstance);
	}
	return pInstance;
}

void CTroll::Free()
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

	SAFE_RELEASE(m_pTroll_Particle);
	SAFE_RELEASE(m_pTroll_Particle2);
	SAFE_RELEASE(m_pRight_Smoke);
	SAFE_RELEASE(m_pLeft_Smoke);
	SAFE_RELEASE(m_pEffectPool);
	SAFE_RELEASE(m_pLeftTrail);
	SAFE_RELEASE(m_pRightTrail);
	SAFE_RELEASE(m_pWeaponTrail);
}
#ifdef _DEBUG

void CTroll::Describe_Entity()
{
	__super::Describe_Entity();


}

#endif // _DEBUG
