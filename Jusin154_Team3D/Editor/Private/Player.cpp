#include "pch.h"
#include "Player.h"

#include "GameInstance.h"
#include "DebugCamera.h"

#pragma region STATE
#include "State_Idle.h"
#include "State_Walk.h"
#include "State_Jog.h"
#include "State_Sprint.h"
#include "State_Dodge.h"
#include "State_Jump.h"
#include "State_Skill.h"
#include "State_Skill2.h"
#include "State_LightAttack.h"
#include "State_Land.h"
#include "State_Cast.h"
#include "State_Move.h"
#include "State_Combat.h"
#include "State_Idle_Turn.h"
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

	Load_KeyFrame();

	Add_FSM();

	Set_FSM();

	Set_Anim();

	Setup_InputConditions();

	m_pFSM->Change_State(FSMSTATE::IDLE);

	return S_OK;
}

void CPlayer::Priority_Update(_float fTimeDelta)
{

}

void CPlayer::Update(_float fTimeDelta)
{
	Key_Input(fTimeDelta);

	m_pFSM->Update(fTimeDelta);

	m_pModelCom->Play_Animation(fTimeDelta, m_pTransformCom);
}

void CPlayer::Late_Update(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderGroup(RENDER::BLEND, this);
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

		if (FAILED(m_pModelCom->Bind_Material(i, m_pShaderCom, "g_DiffuseTexture", aiTextureType_DIFFUSE, 0))) {
			return E_FAIL;
		}
		if (FAILED(m_pModelCom->Bind_Material(i, m_pShaderCom, "g_NormalTexture", aiTextureType_NORMALS, 0))) {
			return E_FAIL;
		}

		if (FAILED(m_pShaderCom->Begin(ENUM_CLASS(SHADER_PASS_ANIM::DEFAULT)))) {
			return E_FAIL;
		}

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
		{ FSMSTATE::MOVE,		 [&]() { return Check(FSMSTATE::WALK) || Check(FSMSTATE::SPRINT) || Check(FSMSTATE::JOG) || Check(FSMSTATE::DODGE); }},
		{ FSMSTATE::WALK,        [&]() { return m_pGameInstance->Key_Pressing(DIK_UP)
										   /*|| m_pGameInstance->Key_Pressing(DIK_DOWN)*/; }},
		{ FSMSTATE::WALK_FWD,    [&]() { return m_pGameInstance->Key_Pressing(DIK_UP); }},
		{ FSMSTATE::WALK_BWD,    [&]() { return m_pGameInstance->Key_Pressing(DIK_DOWN); }},
		{ FSMSTATE::SPRINT,      [&]() { return m_bSprintToggle
										   && m_pGameInstance->Key_Pressing(DIK_UP); }},
		{ FSMSTATE::DODGE,       [&]() { return m_pGameInstance->Key_Down(DIK_LCONTROL); }},

		{ FSMSTATE::COMBAT,		 [&]() { return Check(FSMSTATE::SKILL) || Check(FSMSTATE::LIGHT_ATTACK) || Check(FSMSTATE::CAST) || Check(FSMSTATE::SKILL2); }},
		{ FSMSTATE::SKILL,       [&]() { return m_pGameInstance->Key_Down(DIK_R); }},
		{ FSMSTATE::SKILL2,      [&]() { return m_pGameInstance->Key_Down(DIK_Q); }},
		{ FSMSTATE::LIGHT_ATTACK,[&]() { return m_pGameInstance->Mouse_Up(ENUM_CLASS(MOUSEKEYSTATE::LBUTTON)); }},
		{ FSMSTATE::CAST,        [&]() { return m_pGameInstance->Key_Down(DIK_1); }},

		{ FSMSTATE::JUMP,        [&]() { return m_pGameInstance->Key_Down(DIK_SPACE); } }
	};
}

void CPlayer::Key_Input(_float fTimeDelta)
{
	if (m_pFSM->Get_CurrState() == FSMSTATE::WALK ||
		m_pFSM->Get_CurrState() == FSMSTATE::SPRINT||
		m_pFSM->Get_CurrState() == FSMSTATE::DODGE)
	{
		if (m_pGameInstance->Key_Pressing(DIK_LEFT))
			m_pTransformCom->Turn(-m_pTransformCom->Get_State(STATE::UP), fTimeDelta);

		if (m_pGameInstance->Key_Pressing(DIK_RIGHT))
			m_pTransformCom->Turn(m_pTransformCom->Get_State(STATE::UP), fTimeDelta);
	}

	IsSprint();
}

void CPlayer::Add_FSM()
{
#pragma region MOVE
	m_pFSM->Add_State(FSMSTATE::MOVE, new CState_Move());
	m_pFSM->Add_State(FSMSTATE::WALK, new CState_Walk());
	m_pFSM->Add_State(FSMSTATE::JOG, new CState_Jog());
	m_pFSM->Add_State(FSMSTATE::SPRINT, new CState_Sprint());
	m_pFSM->Add_State(FSMSTATE::DODGE, new CState_Dodge());
#pragma endregion

#pragma region COMBAT
	m_pFSM->Add_State(FSMSTATE::COMBAT, new CState_Combat());
	m_pFSM->Add_State(FSMSTATE::SKILL, new CState_Skill());
	m_pFSM->Add_State(FSMSTATE::SKILL2, new CState_Skill2());
	m_pFSM->Add_State(FSMSTATE::LIGHT_ATTACK, new CState_LightAttack());
	m_pFSM->Add_State(FSMSTATE::CAST, new CState_Cast());
#pragma endregion
	m_pFSM->Add_State(FSMSTATE::IDLE, new CState_Idle());
	m_pFSM->Add_State(FSMSTATE::IDLE_TURN, new CState_Idle_Turn());
	m_pFSM->Add_State(FSMSTATE::JUMP, new CState_Jump());
	m_pFSM->Add_State(FSMSTATE::LAND, new CState_Land());
	
}

void CPlayer::Set_FSM()
{
#pragma region MOVE
	m_pFSM->Set_Parent(FSMSTATE::WALK, FSMSTATE::MOVE);
	m_pFSM->Set_Parent(FSMSTATE::WALK_BWD, FSMSTATE::MOVE);
	m_pFSM->Set_Parent(FSMSTATE::WALK_FWD, FSMSTATE::MOVE);
	m_pFSM->Set_Parent(FSMSTATE::JOG, FSMSTATE::MOVE);
	m_pFSM->Set_Parent(FSMSTATE::SPRINT, FSMSTATE::MOVE);
	m_pFSM->Set_Parent(FSMSTATE::DODGE, FSMSTATE::MOVE);
#pragma endregion

#pragma region COMBAT
	m_pFSM->Set_Parent(FSMSTATE::SKILL, FSMSTATE::COMBAT);
	m_pFSM->Set_Parent(FSMSTATE::SKILL2, FSMSTATE::COMBAT);
	m_pFSM->Set_Parent(FSMSTATE::LIGHT_ATTACK, FSMSTATE::COMBAT);
	m_pFSM->Set_Parent(FSMSTATE::CAST, FSMSTATE::COMBAT);
#pragma endregion
}

void CPlayer::Set_Anim()
{

	m_Animation[STATEANIM::DODGE] = { 802,false };

	m_Animation[STATEANIM::JOG] = { 166,true };

	m_Animation[STATEANIM::SKILL] = { 593,false };
	m_Animation[STATEANIM::SKILL2] = { 915,false };
	m_Animation[STATEANIM::LIGHT_ATTACK] = { 413,false };
	m_Animation[STATEANIM::LAND] = { 259,false };
	m_Animation[STATEANIM::CAST] = { 696,false };

	m_Animation[STATEANIM::IDLE] = { 266,true };
	m_Animation[STATEANIM::IDLE_TURN_L] = { 270,false };
	m_Animation[STATEANIM::IDLE_TURN_R] = { 430,false };
	m_Animation[STATEANIM::IDLE_TURN_BWD] = { 268,false };

	m_Animation[STATEANIM::WALK_FWD] = { 167,true };
	m_Animation[STATEANIM::WALK_BWD] = { 166,true };
	m_Animation[STATEANIM::WALK_STOP] = { 289,false };

	m_Animation[STATEANIM::JUMP] = { 205,false };
	m_Animation[STATEANIM::JUMP_WALK] = { 203,false };
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
