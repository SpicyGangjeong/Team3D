#include "pch.h"
#include "Player.h"

#include "GameInstance.h"
#include "CamPosition_Player.h"
#include "Camera_Gaze.h"
#include "CamPosition_Arm.h"
#include "Wand.h"

#pragma region STATE

#include "State_Player_Idle.h"
#include "State_Player_Dodge.h"
#include "State_Player_Jump.h"
#include "State_Player_Land.h"
#include "State_Player_Move.h"
#include "State_Player_Combat.h"
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

	Setup_InputConditions();

	m_pFSM->Change_State(FSMSTATE::ROOT);

	return S_OK;
}

void CPlayer::Priority_Update(_float fTimeDelta)
{
	__super::Priority_Update(fTimeDelta);
}

void CPlayer::Update(_float fTimeDelta)
{
	m_pModelCom->ComputeSkinning();

	Key_Input(fTimeDelta);

	m_pFSM->Update_State(fTimeDelta);

	m_pModelCom->Play_Animation(fTimeDelta, m_pTransformCom);

	__super::Update(fTimeDelta);
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
	//Desc.pSocketMatrices = m_pModelCom->Get_BoneMatrixPtr("SKT_HeadCamera");

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

void CPlayer::Setup_InputConditions()
{
	m_InputConditions =
	{
		{ FSMSTATE::IDLE_TURN,   [&]() { return m_pGameInstance->Key_Down(DIK_LEFT) || m_pGameInstance->Key_Down(DIK_RIGHT) || m_pGameInstance->Key_Down(DIK_DOWN); }},
		{ FSMSTATE::MOVE,		 [&]() { return Check(FSMSTATE::WALK) || Check(FSMSTATE::SPRINT) || Check(FSMSTATE::JOG) ||  Check(FSMSTATE::DODGE) || Check(FSMSTATE::IDLE_TURN); }},



		{ FSMSTATE::JOG,        [&]() { return m_pGameInstance->Key_Pressing(DIK_UP)
										   /*|| m_pGameInstance->Key_Pressing(DIK_DOWN)*/; }},
		{ FSMSTATE::FWD,    [&]() { return m_pGameInstance->Key_Pressing(DIK_UP); }},
		{ FSMSTATE::BWD,    [&]() { return m_pGameInstance->Key_Pressing(DIK_DOWN); }},

		{ FSMSTATE::WALK,    [&]() { return m_bWalkToggle && m_pGameInstance->Key_Pressing(DIK_UP); }},

		{ FSMSTATE::SPRINT,      [&]() { return m_bSprintToggle && m_pGameInstance->Key_Pressing(DIK_UP); }},
		{ FSMSTATE::DODGE,       [&]() { return m_pGameInstance->Key_Down(DIK_LCONTROL); }},

		{ FSMSTATE::COMBAT,		 [&]() { return Check(FSMSTATE::SKILL) || Check(FSMSTATE::LIGHT_ATTACK) || Check(FSMSTATE::CAST) || Check(FSMSTATE::SKILL2); }},
		{ FSMSTATE::SKILL,       [&]() { return m_pGameInstance->Key_Down(DIK_R); }},
		{ FSMSTATE::SKILL2,      [&]() { return m_pGameInstance->Key_Down(DIK_Q); }},
		{ FSMSTATE::LIGHT_ATTACK,[&]() { return m_pGameInstance->Mouse_Up(DIM_LBUTTON); }},
		{ FSMSTATE::CAST,        [&]() { return m_pGameInstance->Key_Down(DIK_1); }},

		{ FSMSTATE::JUMP,        [&]() { return m_pGameInstance->Key_Down(DIK_SPACE); } }
	};
}

void CPlayer::Key_Input(_float fTimeDelta)
{
	if (m_pFSM->IsEnable(FSMSTATE::WALK|FSMSTATE::SPRINT |FSMSTATE::DODGE |FSMSTATE::JOG))
	{
		if (m_pGameInstance->Key_Pressing(DIK_LEFT)){
			m_pTransformCom->Turn(-m_pTransformCom->Get_State(STATE::UP), fTimeDelta);
		}

		if (m_pGameInstance->Key_Pressing(DIK_RIGHT)){
			m_pTransformCom->Turn(m_pTransformCom->Get_State(STATE::UP), fTimeDelta);
		}
	}

	IsSprint();

	IsWalk();
}

HRESULT CPlayer::InputSystem()
{
	if (m_pGameInstance->Key_Down(DIK_1)
		|| m_pGameInstance->Key_Down(DIK_2)
		|| m_pGameInstance->Key_Down(DIK_3)
		|| m_pGameInstance->Key_Down(DIK_4)
		|| m_pGameInstance->Key_Down(DIK_5)
		|| m_pGameInstance->Key_Down(DIK_6))
	{
		return S_OK;
	}
	return E_FAIL;
}

HRESULT CPlayer::InputSkill()
{
	if (m_pGameInstance->Key_Down(DIK_SPACE)
		|| m_pGameInstance->Key_Down(DIK_Q)
		|| m_pGameInstance->Key_Down(DIK_E)
		|| m_pGameInstance->Key_Down(DIK_R)
		|| m_pGameInstance->Key_Down(DIK_F)
		|| m_pGameInstance->Key_Down(DIK_Z)
		|| m_pGameInstance->Key_Down(DIK_X)
		|| m_pGameInstance->Key_Down(DIK_C)
		|| m_pGameInstance->Key_Down(DIK_V)
		|| m_pGameInstance->Key_Down(DIK_T)
		|| m_pGameInstance->Key_Down(DIK_G)
		|| m_pGameInstance->Key_Down(DIK_B))
	{
		return S_OK;
	}
	return E_FAIL;
}

HRESULT CPlayer::InputRun()
{
	if (m_pGameInstance->Mouse_Pressing(DIK_UP)
		|| m_pGameInstance->Mouse_Pressing(DIK_LEFT)
		|| m_pGameInstance->Mouse_Pressing(DIK_DOWN)
		|| m_pGameInstance->Mouse_Pressing(DIK_RIGHT))
	{
		return S_OK;
	}
	return E_FAIL;
}

void CPlayer::Behavior_IdleEnter() {
	pair<_uint, _bool> pairAnimInfo = m_Animation[STATEANIM::IDLE];

	if (m_pFSM->IsEnable_Previous(FSMSTATE::JOG|FSMSTATE::WALK)) {
		if (m_pFSM->IsEnable_Previous(FSMSTATE::JOG)) {
			pairAnimInfo = m_Animation[STATEANIM::JOG_STOP];
		}
		else if (m_pFSM->IsEnable_Previous(FSMSTATE::WALK)) {
			pairAnimInfo = m_Animation[STATEANIM::WALK_STOP];
		}
	}

	m_pModelCom->Set_AnimationIndex(pairAnimInfo.first, pairAnimInfo.second);
}

HRESULT CPlayer::Behavior_IdleExitCheck()
{
	// S_OK -> 현 상태 유지
	// E_FAIL -> 현 상태 탈출
	if (SUCCEEDED(InputSystem())) {
		//Event
		return S_OK;
	}
	if (SUCCEEDED(InputSkill())) {
		// Action
		return E_FAIL;
	}
	if (SUCCEEDED(InputRun())) {
		m_pFSM->Change_State(FSMSTATE::MOVE);
		return E_FAIL;
	}
	return S_OK;
}

void CPlayer::Behavior_MoveEnter()
{
	pair<_uint, _bool> pairAnimInfo = {};

	if (m_pFSM->IsEnable_Previous(FSMSTATE::IDLE)) {
		// Idle -> Move
	} 
	else if (m_pFSM->IsEnable_Previous(FSMSTATE::DODGE | FSMSTATE::LAND)) {
		// OtherMove -> AnotherMove
	}
	else if (m_pFSM->IsEnable_Previous(FSMSTATE::COMBAT)) {
		// Combat -> Move
	}
	else {
		if (true == m_bSprintToggle) {
			pairAnimInfo = m_Animation[STATEANIM::SPRINT];
		}
		else if (true == m_bWalkToggle) {
			if (true == m_pGameInstance->Key_Pressing(DIK_UP)) {
				pairAnimInfo = m_Animation[STATEANIM::WALK_FWD];
			}
			else {
				pairAnimInfo = m_Animation[STATEANIM::WALK_BWD];
			}
		}
	}

	m_pModelCom->Set_AnimationIndex(pairAnimInfo.first, pairAnimInfo.second);
}

HRESULT CPlayer::Behavior_MoveExitCheck()
{
	if (SUCCEEDED(InputSystem())) {
		//Event
		return S_OK;
	}
	if (SUCCEEDED(InputSkill())) {
		// Action
		return E_FAIL;
	}
	if (SUCCEEDED(InputRun())) {
		return S_OK;
	}
	m_pFSM->Change_State(FSMSTATE::IDLE);
	return E_FAIL;
}

void CPlayer::Behavior_JumpEnter()
{
	pair<_uint, _bool> pairAnimInfo = Get_AnimInfo(STATEANIM::JUMP_SPRINT);
	if (m_pFSM->IsEnable_Previous(FSMSTATE::IDLE|FSMSTATE::JOG|FSMSTATE::WALK)) {
		if (m_pFSM->IsEnable_Previous(FSMSTATE::IDLE)) {
			// Idle -> Jump
			// pairAnimInfo = Get_AnimInfo(STATEANIM::Jump_IDLE);
		}
		else if (m_pFSM->IsEnable_Previous(FSMSTATE::JOG)) {
			pairAnimInfo = Get_AnimInfo(STATEANIM::JUMP_JOG);
		}
		else if (m_pFSM->IsEnable_Previous(FSMSTATE::WALK)) {

		}
	}
	else {
		// Drop?
	}
	m_pModelCom->Set_AnimationIndex(pairAnimInfo.first, pairAnimInfo.second);
}

HRESULT CPlayer::Behavior_JumpExitCheck()
{
	if (SUCCEEDED(InputSystem())) {
		//Event
		return S_OK;
	}
	if (IsCurrentKeyFrame("Jump")) {
		m_pFSM->Change_State(FSMSTATE::LAND);
		return E_FAIL;
	}
	return S_OK;
}

void CPlayer::Behavior_LandEnter()
{
	pair<_uint, _bool> pairAnimInfo = Get_AnimInfo(STATEANIM::LAND);
	m_pModelCom->Set_AnimationIndex(pairAnimInfo.first, pairAnimInfo.second);
}

HRESULT CPlayer::Behavior_LandExitCheck()
{
	if (SUCCEEDED(InputSystem())) {
		//Event
		//return S_OK;
	}
	if (m_pModelCom->IsFinishedAnim()){
		m_pFSM->Change_State(FSMSTATE::IDLE);
		return E_FAIL;
	} // 혹시 Land to (Jog, Sprint, Dodge) 같은 애니메이션 있으면 여기에 분기 조건 넣으면 됨
	return S_OK;
}

void CPlayer::Behavior_DodgeEnter()
{
	// 혹시 @@ to Dodge 있으면 여기에
	pair<_uint, _bool> pairAnimInfo = Get_AnimInfo(STATEANIM::DODGE);
	m_pModelCom->Set_AnimationIndex(pairAnimInfo.first, pairAnimInfo.second);
}

HRESULT CPlayer::Behavior_DodgeExitCheck()
{
	if (SUCCEEDED(InputSystem())) {
		//Event
		//return S_OK;
	}
	if (m_pModelCom->IsFinishedAnim()){
		m_pFSM->Change_State(FSMSTATE::IDLE);
		return E_FAIL;
	}
	return S_OK;
}

void CPlayer::Add_FSM()
{
#pragma region Behavior_Movement_NotFocus
	{
		CState_Idle::STATE_IDLE_DESC Desc{};
		Desc.pOwner = this;
		Desc.funcEnterEvent = [this]() { Behavior_IdleEnter(); };
		Desc.funcExitCheck = [this](_float fTimedelta) { return Behavior_IdleExitCheck(); };
		Desc.funcExitEvent = nullptr;
		Desc.funcPriorityUpdate = nullptr;
		Desc.funcLateUpdate = nullptr;
		m_States.emplace(FSMSTATE::IDLE, CState_Idle::Create(&Desc));
	}
	{
		CState_Move::STATE_MOVE_DESC Desc{};
		Desc.pOwner = this;
		Desc.funcEnterEvent = [this]() { Behavior_MoveEnter(); };
		Desc.funcExitCheck = [this](_float fTimedelta) { return Behavior_MoveExitCheck(); };
		Desc.funcExitEvent = nullptr;
		Desc.funcPriorityUpdate = nullptr;
		Desc.funcLateUpdate = nullptr;
		m_States.emplace(FSMSTATE::MOVE, CState_Move::Create(&Desc));
	}
#pragma endregion
#pragma region Behavior_Movement_Focus
	{
		CState_Jump::STATE_JUMP_DESC Desc{};
		Desc.pOwner = this;
		Desc.funcEnterEvent = [this]() { Behavior_JumpEnter(); };
		Desc.funcExitCheck = [this](_float fTimeDelta) { return Behavior_JumpExitCheck(); };
		Desc.funcExitEvent = nullptr;
		m_States.emplace(FSMSTATE::JUMP, CState_Jump::Create(&Desc));
	}
	{
		CState_Land::STATE_LAND_DESC Desc{};
		Desc.pOwner = this;
		Desc.funcEnterEvent = [this]() { Behavior_LandEnter(); };
		Desc.funcExitCheck = [this](_float fTimeDelta) { return Behavior_LandExitCheck(); };
		Desc.funcExitEvent = nullptr;
		m_States.emplace(FSMSTATE::LAND, CState_Land::Create(&Desc));
	}
	{
		CState_Dodge::STATE_DODGE_DESC Desc{};
		Desc.pOwner = this;
		Desc.funcEnterEvent = [this]() { Behavior_DodgeEnter(); };
		Desc.funcExitCheck = [this](_float fTimeDelta) { return Behavior_DodgeExitCheck(); };
		Desc.funcExitEvent = nullptr;
		Desc.funcPriorityUpdate = nullptr;
		Desc.funcLateUpdate = nullptr;
		m_States.emplace(FSMSTATE::DODGE, CState_Dodge::Create(&Desc));
	}
#pragma endregion
#pragma region Behavior_Combat_NotFocus

#pragma endregion
#pragma region Behavior_Combat_Focus

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

	SAFE_RELEASE(m_pCamPosition_TopDown_FollowPart);
	SAFE_RELEASE(m_pCamPosition_TopDown_LookPart);
}

void CPlayer::Describe_Entity()
{
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

_bool CPlayer::Check(FSMSTATE::ESTATE state)
{
	for (auto& cond : m_InputConditions)
	{
		if (cond.state == state)
			return cond.checker();
	}
	return false;
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
