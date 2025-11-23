#include "pch.h"
#include "Player.h"

#include "GameInstance.h"
#include "CamPosition_Player.h"
#include "Camera_Gaze.h"
#include "CamPosition_Arm.h"
#include "Wand.h"
#include "Character_Controller.h"
#include "CallBack_Playable_Behavior.h"
#include "CallBack_Playable_HitReport.h"
#include "Character_Controller.h"

#pragma region STATE

#pragma endregion

CPlayer::CPlayer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUnit(pDevice, pContext)
{
}

CPlayer::CPlayer(const CPlayer& Prototype)
	: CUnit(Prototype)
{
}

HRESULT CPlayer::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CPlayer::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;


	if (FAILED(Ready_Parts()))
		return E_FAIL;

	Load_KeyFrame();

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
	return S_OK;
}

void CPlayer::Priority_Update(_float fTimeDelta)
{
	m_pTransformCom->RewindMomentum();
	m_pTransformCom->Set_State(STATE::POSITION, m_pCharacter_Controller->Get_Position());
	__super::Priority_Update(fTimeDelta);
}

void CPlayer::Update(_float fTimeDelta)
{
	m_pModelCom->ComputeSkinning();

	IsSprint();
	IsWalk();

	m_pFSM->Update_State(fTimeDelta);


	m_pModelCom->Play_Animation(fTimeDelta, m_pTransformCom);

	__super::Update(fTimeDelta);
	Describe_Entity();
	m_pCharacter_Controller->Move(fTimeDelta);
}

void CPlayer::Late_Update(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderGroup(RENDER::BLEND, this);

	__super::Late_Update(fTimeDelta);
}

HRESULT CPlayer::Render()
{
	if (FAILED(Bind_ShaderResources())) {
		return E_FAIL;
	}

	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; i++)
	{
		if (FAILED(m_pModelCom->Bind_BoneMatrices(i, m_pShaderCom, "g_BoneMatrices"))) {
			return E_FAIL;
		}

		if (FAILED(m_pModelCom->Bind_Material(i, m_pShaderCom))) {
			return E_FAIL;
		}
		if (FAILED(m_pShaderCom->Begin(ENUM_CLASS(SHADER_PASS_ANIM::DEFAULT)))) {
			return E_FAIL;
		}

		if (FAILED(m_pModelCom->Bind_CS_Output(5, i)))
			return E_FAIL;

		if (FAILED(m_pModelCom->Render(i))) {
			return E_FAIL;
		}
	}

#ifdef _DEBUG
	m_pCharacter_Controller->Render();
	//m_pRigidBody->Render();
#endif

	return S_OK;
}

HRESULT CPlayer::Ready_Components()
{
	CTransform::TRANSFORM_DESC Desc = {};

	Desc.fSpeedPerSec = 10.f;
	Desc.fRotationPerSec = XMConvertToRadians(180.0f);
	Desc.fRadius = 10.f;

	__super::Ready_Components(&Desc);

	m_strModelPrototypeTag = TEXT("Prototype_Component_Npc_Model");

	/* Com_Model */
	if (FAILED(__super::Add_Asset_Component(g_iStaticLevel, m_strModelPrototypeTag,
		reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	/* Com_Shader */
	if (FAILED(__super::Add_Asset_Component(g_iStaticLevel, FX_ANIMMESH,
		reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	{ // CCT
		CCharacter_Controller::Character_Controller_DESC Desc{};

		Desc.iSubKind = ENUM_CLASS(COLLIDABLEOBJECT::PLAYER);
		Desc.pTransform = m_pTransformCom;
		Desc.eBodyType = ACTOR::CAPSULE;
		Desc.fContactOffset = 0.1f;
		Desc.fMaterial = { 0.5f, 0.5f, 0.6f };
		Desc.bAutoStepping = { false };
		Desc.fStepOffset = { 0.05f };
		Desc.fRadius = 0.5f;
		Desc.fHeight = 1.0f;
		Desc.pCallback_HitReport = m_pCallBack_HitReport = CCallBack_Playable_HitReport::Create();
		Desc.pCallback_Behavior = m_pCallBack_Behavior = CCallBack_Playable_Behavior::Create();
		Desc.eClimbingMode = PSX::PxCapsuleClimbingMode::eEASY;
		if (FAILED(Add_Asset_Component(g_iStaticLevel, TEXT("PHYSX_CCT_CAPSULE"), (CComponent**)&m_pCharacter_Controller, &Desc))) {
			return E_FAIL;
		}
	}

	{ // DO
		CRigidBody_Dynamic::RIGIDBODY_DYNAMIC_DESC Desc{};
		Desc.iSubKind = ENUM_CLASS(COLLIDABLEOBJECT::PLAYER);
		if (FAILED(Add_Asset_Component(g_iStaticLevel, TEXT("PHYSX_DYNAMIC_BOX"), (CComponent**)&m_pRigidBody, &Desc))) {
			return E_FAIL;
		}
		m_pGameInstance->Detach_Actor(*m_pRigidBody->Get_Actor());
	}

	return S_OK;
}

HRESULT CPlayer::Ready_Parts()
{
	CWand::WAND_DESC WandDesc{};

	WandDesc.pParentTransform = m_pTransformCom;
	WandDesc.pSocketMatrices = m_pModelCom->Get_BoneMatrixPtr("SKT_RightHand");

	if (FAILED(Add_PartObject<CWand>("Wand", g_iStaticLevel, nullptr, &WandDesc)))
	{
		return E_FAIL;
	}

	CCamPosition_Player::CAMERAPOSITION_PLAYER_DESC Desc{};

	Desc.pParentTransform = m_pTransformCom;

	if (FAILED(Add_PartObject<CCamPosition_Player>("Cam_TopDown_Look", g_iStaticLevel, &m_pCamPosition_TopDown_LookPart, &Desc)))
	{
		return E_FAIL;
	}

	CCamPosition_Arm::CameraArm_DESC CameraArmDesc{};
	XMStoreFloat3(&CameraArmDesc.fAt, m_pTransformCom->Get_State(STATE::POSITION));
	CameraArmDesc.pParentTransform = m_pTransformCom;
	CameraArmDesc.fEye = { 0.f, 10.f, 10.f };
	CameraArmDesc.fMouseSensor = 0.4f;
	CameraArmDesc.fDistance = 4.f;
	CameraArmDesc.pParentTransform = m_pTransformCom;

	if (FAILED(Add_PartObject<CCamPosition_Arm>("Cam_TopDown_Follow", g_iStaticLevel, &m_pCamPosition_TopDown_FollowPart, &CameraArmDesc))) {
		return E_FAIL;
	}

	CCamera_Gaze::CAMERA_GAZE_DESC CameraDesc{};
	CameraDesc.fFovy = XMConvertToRadians(60.0f);
	CameraDesc.fNear = 0.1f;
	CameraDesc.fFar = 300.f;
	CameraDesc.fSpeedPerSec = 5.f;
	CameraDesc.fRotationPerSec = XMConvertToRadians(90.f);
	CameraDesc.pFollowTarget = m_pCamPosition_TopDown_FollowPart;
	CameraDesc.pLookTarget = m_pCamPosition_TopDown_LookPart;
	CameraDesc.iPriority = 51;

	CCamera_Gaze* pCamera = { nullptr };

	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer<CCamera_Gaze>(g_iStaticLevel, NEXT_LEVEL, LAYER_CAMERA, &CameraDesc, nullptr, &pCamera)))
	{
		return E_FAIL;
	}

	m_pGameInstance->Add_Camera(NEXT_LEVEL, pCamera, TEXT("CAM_TOPVIEW"));

	return S_OK;
}

HRESULT CPlayer::Bind_ShaderResources()
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

void CPlayer::Add_FSM()
{
#pragma region MOVE
#pragma endregion

#pragma region COMBAT
#pragma endregion
	
}

void CPlayer::Set_Anim()
{

	m_Animation[STATEANIM::DODGE] = { 802,false };

	m_Animation[STATEANIM::SKILL] = { 593,false };
	m_Animation[STATEANIM::SKILL2] = { 915,false };
	m_Animation[STATEANIM::LIGHT_ATTACK] = { 413,false };
	m_Animation[STATEANIM::LAND] = { 259,false };
	m_Animation[STATEANIM::CAST] = { 696,false };

	m_Animation[STATEANIM::IDLE] = { 266,true };
	m_Animation[STATEANIM::IDLE_TURN_L] = { 270,false };
	m_Animation[STATEANIM::IDLE_TURN_R] = { 430,false };
	m_Animation[STATEANIM::IDLE_TURN_BWD] = { 268,false };

	m_Animation[STATEANIM::WALK_FWD] = { 625,true };
	m_Animation[STATEANIM::WALK_BWD] = { 166,true };
	m_Animation[STATEANIM::WALK_STOP] = { 344,false };

	m_Animation[STATEANIM::JOG_FWD] = { 167,true };
	m_Animation[STATEANIM::JOG_STOP] = { 289,false };

	m_Animation[STATEANIM::JUMP] = { 205,false };
	m_Animation[STATEANIM::JUMP_JOG] = { 203,false };
	m_Animation[STATEANIM::JUMP_SPRINT] = { 207,false };

	m_Animation[STATEANIM::SPRINT] = { 599,true };

}

CPlayer* CPlayer::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CPlayer* pInstance = new CPlayer(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CPlayer");
		SAFE_RELEASE(pInstance);
	}
	return pInstance;
}

CGameObject* CPlayer::Clone(void* pArg, CGameObject* pOwner)
{
	CPlayer* pInstance = new CPlayer(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CPlayer");
		SAFE_RELEASE(pInstance);
	}
	return pInstance;
}

void CPlayer::Free()
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
	SAFE_RELEASE(m_pCamPosition_TopDown_FollowPart);
	SAFE_RELEASE(m_pCamPosition_TopDown_LookPart);
}

void CPlayer::Describe_Entity()
{
	m_pCharacter_Controller->Describe_Entity();
	_float4 vMomentum = {};
	XMStoreFloat4(&vMomentum, m_pTransformCom->Get_CurrentMomentum());
	GUI::Text("%.1f %.1f %.1f %.1f ", vMomentum.x, vMomentum.y, vMomentum.z, vMomentum.w);
	_char label[256];
	for (auto& iter : m_KeyFrames)
	{
		sprintf_s(label, "%s   %.2f", iter.first.c_str(), iter.second);

		GUI::Text(label);
	}

	_float3 Pos;
	XMStoreFloat3(&Pos, Get_WorldPostion());

	float Pos3[3] = { Pos.x, Pos.y, Pos.z };
	GUI::DragFloat3("Pos", Pos3);


	_float RotR,RotU,RotL;
	RotR = XMVectorGetX(m_pTransformCom->Get_State(STATE::RIGHT));
	RotU = XMVectorGetY(m_pTransformCom->Get_State(STATE::UP));
	RotL = XMVectorGetZ(m_pTransformCom->Get_State(STATE::LOOK));

	float Rot3[3] = { RotR, RotU,RotL };
	GUI::DragFloat3("Rot", Rot3);
	
}

_bool CPlayer::IsSprint()
{
	if (m_pGameInstance->Key_Down(DIK_LSHIFT))
	{
		return m_bSprintToggle = !m_bSprintToggle;
	}
	return false;
}

_bool CPlayer::IsWalk()
{
	if (m_pGameInstance->Key_Down(DIK_C))
	{
		return m_bWalkToggle = !m_bWalkToggle;
	}
	return false;
}
