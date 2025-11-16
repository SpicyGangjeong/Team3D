#include "pch.h"
#include "Player.h"

#include "GameInstance.h"
#include "DebugCamera.h"
#include "State_Idle.h"
#include "State_Walk.h"
#include "State_Dodge.h"
#include "State_Sprint.h"
#include "State_Jump.h"

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

	Add_FSM();

	Set_Anim();

	m_pFSM->Change_State(FSMSTATE::IDLE);

	return S_OK;
}

void CPlayer::Priority_Update(_float fTimeDelta)
{

}

void CPlayer::Update(_float fTimeDelta)
{
	Check_State();

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

_bool CPlayer::IsWalking()
{
	if (m_pGameInstance->Key_Pressing(DIK_UP) || m_pGameInstance->Key_Pressing(DIK_DOWN))
	{
		return true;
	}
	return false;
}

_bool CPlayer::IsDodge()
{
	if (m_pGameInstance->Key_Down(DIK_LCONTROL))
	{
		return true;
	}
	return false;
}

_bool CPlayer::IsSprint()
{
	if (m_pGameInstance->Key_Pressing(DIK_LSHIFT) && m_pGameInstance->Key_Pressing(DIK_UP))
	{
		return true;
	}
	return false;
}

_bool CPlayer::IsJump()
{
	if (m_pGameInstance->Key_Down(DIK_SPACE))
	{
		return true;
	}
	return false;
}

HRESULT CPlayer::Ready_Components()
{
	CTransform::TRANSFORM_DESC Desc = {};

	Desc.fSpeedPerSec = 10.f;
	Desc.fRotationPerSec = 10.f;
	Desc.fRadius = 10.f;

	__super::Ready_Components(&Desc);

	/* Com_Model */
	if (FAILED(__super::Add_Asset_Component(g_iStaticLevel, TEXT("Prototype_Component_Npc_Model"),
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

void CPlayer::Add_FSM()
{
	m_pFSM->Add_State(FSMSTATE::IDLE, new CState_Idle());
	m_pFSM->Add_State(FSMSTATE::WALK, new CState_Walk());
	m_pFSM->Add_State(FSMSTATE::DODGE, new CState_Dodge());
	m_pFSM->Add_State(FSMSTATE::SPRINT, new CState_Sprint());
	m_pFSM->Add_State(FSMSTATE::JUMP, new CState_Jump());
}

void CPlayer::Set_Anim()
{
	m_Animation[FSMSTATE::IDLE] = { 41,true };
	m_Animation[FSMSTATE::DODGE] = { 802,false };
	m_Animation[FSMSTATE::WALK_FWD] = { 167,true };
	m_Animation[FSMSTATE::WALK_BWD] = { 166,true };
	m_Animation[FSMSTATE::SPRINT] = { 599,true };
	m_Animation[FSMSTATE::JUMP] = { 205,false };
}

void CPlayer::Check_State()
{
	IsWalking();

	IsDodge();

	IsSprint();
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
}
