#include "pch.h"
#include "Broom.h"

#include "GameInstance.h"
#include "InfoInstance.h"
#include "Unit.h"
#include "PartObject.h"
#include "EffectParts.h"

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

	m_pParentUnit = dynamic_cast<CUnit*>(m_pOwner);

	if (m_pParentUnit->IsAI() == false)
	{
		if (FAILED(Ready_Child())) {
			return E_FAIL;
		}

		m_pWindEffect->Get_Effect_Info()->isBillboard = false;
	}




	return S_OK;
}

void CBroom::Priority_Update(_float fTimeDelta)
{
	__super::Priority_Update(fTimeDelta);
}

void CBroom::Update(_float fTimeDelta)
{
	__super::Update(fTimeDelta);

	Update_CameraCoordinateSystem();

	if(!m_pParentUnit->IsAI())
		PlayerInput();
	else {
		m_bHoverToggle = m_Input.bHoverToggle;
		m_bTurbo = m_Input.bTurbo;
	}

	if (!m_bRide)
	{
		m_bHoverToggle = true;
		m_fSpeed = 0.f;

		if(m_pWindEffect != nullptr)
		{
			if(m_pWindEffect->Get_Visible() == true) // 전 프레임에 트루였다면
			{
				m_pWindEffect->Set_Visible(false);
				m_pWindEffect->Get_Effect_Info()->fSoftMask = 1.f;
			}
		}

		m_pFSM->Change_State(FSMSTATE::IDLE);
	}

	m_pFSM->Update_State(fTimeDelta);

	m_pModelCom->Play_Animation(fTimeDelta, m_pTransformCom);

#ifdef _DEBUG
	Describe_Entity();
#endif // _DEBUG

	/* 윈드 이펙트 */

	if (m_bRide == true && m_pWindEffect != nullptr)
	{
		if (m_fSpeed <= 5.f)
		{
			//m_pWindEffect->Set_Visible(false);
			return;
		}

		m_pWindEffect->Set_Visible(true);
		m_pWindEffect->Get_Component<CInstance_Model>()->Set_TimeMult(0.5f + m_fSpeed / 20.f);
		m_pWindEffect->Get_Effect_Info()->fBlurIntensity = m_fSpeed / 30.f;

	}



}

void CBroom::Late_Update(_float fTimeDelta)
{
	__super::Late_Update(fTimeDelta);

	if (m_bRide == true && m_pWindEffect != nullptr)
	{
		CTransform* WindTransform = m_pWindEffect->Get_Component<CTransform>();

		_matrix WorldMat = m_pTransformCom->Get_XMWorldMatrix()/* * XMMatrixRotationAxis(XMVectorSet(0.f , 1.f, 0.f ,0.f), XMConvertToRadians(180.f))*/;
		WindTransform->Set_WorldMatrix(WorldMat);

		_vector vCameraLook = XMVector3Normalize(m_pGameInstance->Get_CameraLook());
		WindTransform->Set_State(STATE::POSITION, m_pGameInstance->Get_CamXMPosition() + vCameraLook * m_fCameraOffset);

#if _DEBUG
#if 진우
		GUI::Begin("Wind");

		m_pWindEffect->Get_Component<CInstance_Model>()->Describe_Entity();
		GUI::Checkbox("BillBoard", &m_pWindEffect->Get_Effect_Info()->isBillboard);
		GUI::DragFloat("Offset", &m_fCameraOffset);
		GUI::End();
#endif
#endif
	}

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
		if (FAILED(m_pModelCom->Bind_Material(i, m_pShaderCom))) {
			return E_FAIL;
		}

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

HRESULT CBroom::Ready_Child()
{

	CPartObject::PARTOBJECT_DESC PartsDesc{};

	PartsDesc.pParentTransform = m_pTransformCom;

	if (FAILED(Add_PartObject<CEffectParts>("Wind_Screen", g_iStaticLevel, &m_pWindEffect, &PartsDesc)))
	{
		return E_FAIL;
	}

	m_pWindEffect->Load("../Bin/Resources/Data/Effect/ScreenFX/Wind", static_cast<LEVEL>(NEXT_LEVEL));

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
	if (FAILED(__super::Add_Asset_Component(g_iStaticLevel, FX_NPC_PBR_ANIM,
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


void CBroom::PlayerInput()
{
	m_Input = {};

	m_Input.Z = m_pGameInstance->Key_Pressing(DIK_W) ? 1.f : 0.f;

	m_Input.X = (m_pGameInstance->Key_Pressing(DIK_D) ? 1.f : 0.f)
		- (m_pGameInstance->Key_Pressing(DIK_A) ? 1.f : 0.f);

	m_Input.Y = (m_pGameInstance->Key_Pressing(DIK_SPACE) ? 1.f : 0.f)
		- (m_pGameInstance->Key_Pressing(DIK_LCONTROL) ? 1.f : 0.f + m_pGameInstance->Key_Pressing(DIK_N) ? 1.f : 0.f);

	if (m_pGameInstance->Key_Up(DIK_LSHIFT))
	{
		m_Input.bHoverToggle = !m_Input.bHoverToggle;
		m_bHoverToggle = m_Input.bHoverToggle;
	}


	m_Input.bTurbo = m_pGameInstance->Mouse_Pressing(DIM_LBUTTON);
	m_bTurbo = m_Input.bTurbo;
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

	SAFE_RELEASE(m_pWindEffect);
}
#ifdef _DEBUG

void CBroom::Describe_Entity()
{
	GUI::Begin("UNIT", 0, IMGUI_GLOBAL_BEGIN_FLAG);
	if (GUI::CollapsingHeader("Broom")) {
		GUI::DragFloat("MaxSpeed", &m_fFlyMaxSpeed, 0.01f);
		GUI::DragFloat("Speed", &m_fSpeed, 0.01f);
		GUI::DragFloat("Accle", &m_fAccel, 0.01f);
		GUI::DragFloat("Decel", &m_fDecel, 0.01f);

		string AnimList = m_pModelCom->Get_AnimList(m_pModelCom->Get_AnimIndex());
		GUI::Text(AnimList.c_str());
		GUI::Text("AnimIndex %d", m_pModelCom->Get_AnimIndex());

		GUI::Text("AnimTrack %.2f", m_pModelCom->Get_CurrentTrackPosition());
		GUI::Text("AnimRatio %.2f", m_pModelCom->Get_CurrentTrackProgressRatio());
		_float3 Pos;
		XMStoreFloat3(&Pos, Get_WorldPostion());

		float Pos3[3] = { Pos.x, Pos.y, Pos.z };
		GUI::DragFloat3("Pos", Pos3);


		_float RotR, RotU, RotL;
		RotR = XMVectorGetX(m_pTransformCom->Get_State(STATE::RIGHT));
		RotU = XMVectorGetY(m_pTransformCom->Get_State(STATE::UP));
		RotL = XMVectorGetZ(m_pTransformCom->Get_State(STATE::LOOK));

		float Rot3[3] = { RotR, RotU,RotL };
		GUI::DragFloat3("Rot", Rot3);
	}
	GUI::End();
}

#endif // _DEBUG
