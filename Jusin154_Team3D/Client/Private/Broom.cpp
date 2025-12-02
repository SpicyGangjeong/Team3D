#include "pch.h"
#include "Broom.h"

#include "GameInstance.h"
#include "InfoInstance.h"
CBroom::CBroom(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUnit(pDevice, pContext)
{
}

CBroom::CBroom(const CBroom& Prototype)
	: CUnit(Prototype),
	m_pInfoInstance(CInfoInstance::GetInstance())
{
}

HRESULT CBroom::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CBroom::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg))) {
		return E_FAIL;
	}


	if (FAILED(Ready_Components())) {
		return E_FAIL;
	}

	Add_FSM();

	Set_Anim();

	{
		CFSM::FSM_DESC FSMDesc{};
		FSMDesc.pStates = &m_States;
		FSMDesc.pStateMask = &m_iStateMask;

		m_pFSM->Bind_States(FSMDesc);
		m_pFSM->Change_State(FSMSTATE::IDLE);
	}


	m_pTransformCom->Set_State(STATE::POSITION, XMVectorSet(0.f, 10.f, 0.f, 1.f));

	return S_OK;
}

void CBroom::Priority_Update(_float fTimeDelta)
{
	
}

void CBroom::Update(_float fTimeDelta)
{
	Update_CameraCoordinateSystem();

	m_pFSM->Update_State(fTimeDelta);

	m_pModelCom->Play_Animation(fTimeDelta, m_pTransformCom);

#ifdef _DEBUG
	Describe_Entity();
#endif // _DEBUG

}

void CBroom::Late_Update(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderGroup(RENDER::NONBLEND, this);
}

HRESULT CBroom::Render()
{
	if (!m_pModelCom){
		return S_OK;
	}

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


		if (FAILED(m_pModelCom->Render(i))) {
			return E_FAIL;
		}
	}

	return S_OK;
}

HRESULT CBroom::Ready_Components()
{
	CTransform::TRANSFORM_DESC Desc = {};

	Desc.fSpeedPerSec = 10.f;
	Desc.fRotationPerSec = XMConvertToRadians(180.0f);
	Desc.fRadius = 10.f;

	__super::Ready_Components(&Desc);

	/* Com_Model */
	if (FAILED(__super::Add_Asset_Component(g_iStaticLevel, TEXT("Prototype_Component_Broom_Model"),
		reinterpret_cast<CComponent**>(&m_pModelCom)))) {
		return E_FAIL;
	}

	/* Com_Shader */
	if (FAILED(__super::Add_Asset_Component(g_iStaticLevel, FX_ANIMMESH,
		reinterpret_cast<CComponent**>(&m_pShaderCom)))) {
		return E_FAIL;
	}


	return S_OK;
}

HRESULT CBroom::Bind_ShaderResources()
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

void CBroom::Update_CameraCoordinateSystem()
{
	_vector xmvCameraLook = XMVector3Normalize(XMVectorSetY(m_pGameInstance->Get_CameraLook(), 0.f));
	_vector xmvUp = XMVectorSet(0.f, 1.f, 0.f, 0.f);
	XMStoreFloat3(&m_vCameraRightDir, XMVector3Normalize(XMVector3Cross(xmvUp, xmvCameraLook)));
	XMStoreFloat3(&m_vCameraLookDir, xmvCameraLook);
	m_pInfoInstance->Update_CameraCoordinateSystem(m_vCameraLookDir, m_vRimLightColor);
}

CBroom* CBroom::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CBroom* pInstance = new CBroom(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CBroom");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

CGameObject* CBroom::Clone(void* pArg, CGameObject* pOwner)
{
	CBroom* pInstance = new CBroom(*this);
	pInstance->m_pOwner = pOwner;
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CBroom");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

void CBroom::Free()
{
	__super::Free();
}
#ifdef _DEBUG

void CBroom::Describe_Entity()
{

	GUI::DragFloat("MaxSpeed", &m_fFlyMaxSpeed, 0.01f);
	GUI::DragFloat("Speed", &m_fSpeed, 0.01f);
	GUI::DragFloat("Accle", &m_fAccel, 0.01f);
	GUI::DragFloat("Decel", &m_fDecel, 0.01f);

	string AnimList = m_pModelCom->Get_AnimList(m_pModelCom->Get_AnimIndex());
	GUI::Text(AnimList.c_str());

}

#endif // _DEBUG
