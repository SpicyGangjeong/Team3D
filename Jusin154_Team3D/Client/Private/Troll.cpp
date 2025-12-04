#include "pch.h"
#include "Troll.h"

#include "Troll_Weapon.h"
#include "Troll_Rock.h"
#include "GameInstance.h"

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


	for (_uint i = 0; i < ENUM_CLASS(TROLL_SKILL::END); i++)
		m_fSkillCoolTime[i] = max(0.f, m_fSkillCoolTime[i] - fTimeDelta);


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
		m_pTransformCom->LookAt_Lerp(XMLoadFloat4(&m_vTargetPos),fTimeDelta, 5.f);
	}
	

	m_pGameInstance->Add_RenderGroup(RENDER::NONBLEND, this);
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

void CTroll::OnCollision(CGameObject* pOther, void* pDesc)
{
	ON_COLLISION_INFO* CollisionDesc = static_cast<ON_COLLISION_INFO*>(pDesc);
	_vector vWorldPos = {};		// 접촉지점
	_vector vWorldNomal = {};	// 접촉노말
	_vector vHitDir = {};		// 시도한 move 방향
	_float  fLength = {};		// 작용된 힘

	//m_pCharacter_Controller->ConvertToDO(*m_pRigidBody);
	//m_pRigidBody->Add_Force(vHitDir * fLength * 100.f, PSX::PxForceMode::eIMPULSE);
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
		Desc.fContactOffset = 0.1f;
		Desc.fMaterial = { 0.5f, 0.5f, 0.6f };
		Desc.bAutoStepping = { false };
		Desc.fStepOffset = { 0.05f };
		Desc.fRadius = 1.2f;
		Desc.fHeight = 1.5f;
		Desc.pCallback_HitReport = m_pCallBack_HitReport = CCallBack_Monster_HitReport::Create();
		Desc.pCallback_Behavior = m_pCallBack_Behavior = CCallBack_Monster_Behavior::Create();
		Desc.eClimbingMode = PSX::PxCapsuleClimbingMode::eEASY;
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
}
#ifdef _DEBUG

void CTroll::Describe_Entity()
{
	GUI::Begin("Troll");

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

	if (GUI::DragFloat3("Pos",(_float*)&Pos))
	{
		m_pCharacter_Controller->Set_Position(XMLoadFloat3(&Pos));
	}

	GUI::End();

}

#endif // _DEBUG
