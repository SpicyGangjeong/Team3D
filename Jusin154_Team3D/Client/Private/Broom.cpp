#include "pch.h"
#include "Broom.h"

#include "GameInstance.h"
#include "InfoInstance.h"
#include "Unit.h"
#include "PartObject.h"
#include "EffectParts.h"
#include "TrailObject.h"

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

	if (pArg) {
		m_iIndex = *static_cast<_int*>(pArg);
	}


	if (FAILED(Ready_Components())) {
		return E_FAIL;
	}


	Add_FSM();


	Load_AnimXML("../Bin/Resources/Data/AnimList/Broom.xml");

	{
		CFSM::FSM_DESC FSMDesc{};
		FSMDesc.pStates = &m_States;
		FSMDesc.pStateMask = &m_iStateMask;

		m_pFSM->Bind_States(FSMDesc);
		m_pFSM->Change_State(FSMSTATE::IDLE);
	}

	m_pTransformCom->Set_State(STATE::POSITION, XMVectorSet(0.f, -100.f, 0.f, 1.f));

	m_pParentUnit = dynamic_cast<CUnit*>(m_pOwner);

	if (m_pParentUnit->IsAI() == false)
	{
		if (FAILED(Ready_Child())) {
			return E_FAIL;
		}

		m_pWindEffect->Get_Effect_Info()->isBillboard = false;
	}
	else {
		m_fAISeed = m_pGameInstance->Real_Random_Float(0.f, XM_2PI);
		m_fAICondition = 1.f;
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

	if (!m_pParentUnit->IsAI()) {
		PlayerInput(fTimeDelta);
		m_pInfoInstance->Set_Broom_Booster_Timer(m_fTurboBoost);
	}
	else {
		m_fAITime += fTimeDelta;

		_float target = 1.f + sinf(m_fAITime * 0.3f + m_fAISeed) * 0.15f;

		m_fAICondition += (target - m_fAICondition) * fTimeDelta * 0.3f;

		m_fAICondition = clamp(m_fAICondition, 0.85f, 1.15f);
	}
	if (!m_bRide)
	{
		m_bHoverToggle = true;
		m_fSpeed = 0.f;

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
		_matrix BroomTail_Mat = XMLoadFloat4x4(m_pBroom_TailMat);

		for (int i = 0; i < 3; ++i) {
			BroomTail_Mat.r[i] = XMVector3Normalize(BroomTail_Mat.r[i]);
		}

		m_pBroomTrail->Oneside_Rope_Trail_Update(BroomTail_Mat * m_pTransformCom->Get_XMWorldMatrix(), fTimeDelta);

		if (m_fSpeed <= 5.f)
		{
			m_pWindEffect->Get_Component<CInstance_Model>()->Set_Loop(false);
			m_pWindEffect->Get_Effect_Info()->isDissolve = true;
			m_pWindEffect->Get_Effect_Info()->fSoftMask = 1.f;

			m_pBroomTrail->SetDissolve(true);

			return;
		}



		m_pBroomTrail->SetDissolve(false);
		m_pBroomTrail->Set_Visible(true);

		/* 윈드 이펙트 루프 돌리도록 */
		m_pWindEffect->Get_Component<CInstance_Model>()->Set_Loop(true);
		m_pWindEffect->Get_Effect_Info()->isDissolve = false;
		m_pWindEffect->Set_Visible(true);

		/* 윈드 이펙트 속도에 따라 강해지도록 */
		m_pWindEffect->Get_Component<CInstance_Model>()->Set_TimeMult(0.5f + m_fSpeed / 20.f);
		m_pWindEffect->Get_Effect_Info()->fBlurIntensity = m_fSpeed / 30.f;


	}
}

void CBroom::Late_Update(_float fTimeDelta)
{
	if (!m_pGameInstance->IsIn_WorldFrustum(m_pTransformCom->Get_State(STATE::POSITION), m_pTransformCom->Get_Radius())) {
		return;
	}
	__super::Late_Update(fTimeDelta);

	if (m_bRide == true && m_pWindEffect != nullptr)
	{
		CTransform* WindTransform = m_pWindEffect->Get_Component<CTransform>();

		_matrix WorldMat = m_pTransformCom->Get_XMWorldMatrix()/* * XMMatrixRotationAxis(XMVectorSet(0.f , 1.f, 0.f ,0.f), XMConvertToRadians(180.f))*/;
		WindTransform->Set_WorldMatrix(WorldMat);

		_vector vCameraLook = XMVector3Normalize(m_pGameInstance->Get_CameraLook());
		WindTransform->Set_State(STATE::POSITION, m_pGameInstance->Get_CamXMPosition() + vCameraLook * m_fCameraOffset);

	}

	m_pGameInstance->Add_RenderGroup(RENDER::NONBLEND, this);

}

HRESULT CBroom::Render()
{
	if (!m_pModelCom) {
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

	m_pWindEffect->Load("../Bin/Resources/Data/Effect/ScreenFX/Wind", static_cast<LEVEL>(g_iStaticLevel));

	///

	if (FAILED(Add_PartObject<CEffectParts>("Boost_Screen", g_iStaticLevel, &m_pBoostScreenFX, &PartsDesc)))
	{
		return E_FAIL;
	}
	m_pBoostScreenFX->Load("../Bin/Resources/Data/Effect/BroomEffect/Booster_ScreenFX", static_cast<LEVEL>(g_iStaticLevel));


	///

	if (FAILED(Add_PartObject<CTrailObject>("Broom_Trail", g_iStaticLevel, &m_pBroomTrail, &PartsDesc))) {
		return E_FAIL;
	}

	m_pBroomTrail->Load_Trail("../Bin/Resources/Data/Effect/BroomEffect/Broom_Trail", static_cast<LEVEL>(g_iStaticLevel));
	m_pBroomTrail->Set_Visible(false);


	m_pBroom_TailMat = m_pModelCom->Get_BoneMatrixPtr("thistleSocket");

	return S_OK;
}

HRESULT CBroom::Ready_Components()
{
	CTransform::TRANSFORM_DESC Desc = {};

	Desc.fSpeedPerSec = 10.f;
	Desc.fRotationPerSec = XMConvertToRadians(180.0f);
	Desc.fRadius = 10.f;

	__super::Ready_Components(&Desc);

	switch (m_iIndex)
	{
	case 0:
		m_strModelPrototypeTag = TEXT("Prototype_Component_Broom_Model");
		break;
	case 1:
		m_strModelPrototypeTag = TEXT("Prototype_Component_SK_MoonTrimmerBroom_Model");
		break;
	case 2:
		m_strModelPrototypeTag = TEXT("Prototype_Component_DarkWizardBroom_Model");
		break;
	case 3:
		m_strModelPrototypeTag = TEXT("Prototype_Component_WildFireBroom_Model");
		break;
	default:
		break;
	}

	/* Com_Model */
	if (FAILED(__super::Add_Asset_Component(g_iStaticLevel, m_strModelPrototypeTag,
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
	if (FAILED(m_pShaderCom->Bind_Matrix("g_PrevWorldMatrix", m_pTransformCom->Get_PrevWorldMatrixPtr()))) {
		return E_FAIL;
	}
	if (FAILED(m_pGameInstance->Bind_PrevMatrix(m_pShaderCom, "g_PrevViewMatrix", D3DTS::VIEW))) {
		return E_FAIL;
	}
	if (FAILED(m_pGameInstance->Bind_PrevMatrix(m_pShaderCom, "g_PrevProjMatrix", D3DTS::PROJ))) {
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

void CBroom::Boost_Effect_Visible(_bool isVisible)
{
	m_pBoostScreenFX->Get_Component<CInstance_Model>()->Set_Loop(isVisible);

	if (isVisible == true)
	{

		m_pBoostScreenFX->Set_Visible(true);
	}
}


void CBroom::PlayerInput(_float fTimeDelta)
{
	if (!m_bRide)
		return;
	m_Input = {};

	m_Input.Z = m_pGameInstance->Key_Pressing(DIK_W) ? 1.f : 0.f;

	m_Input.X = (m_pGameInstance->Key_Pressing(DIK_D) ? 1.f : 0.f)
		- (m_pGameInstance->Key_Pressing(DIK_A) ? 1.f : 0.f);

	m_Input.Y = (m_pGameInstance->Key_Pressing(DIK_SPACE) ? 1.f : 0.f)
		- (m_pGameInstance->Key_Pressing(DIK_LCONTROL) ? 1.f : 0.f + m_pGameInstance->Key_Pressing(DIK_N) ? 1.f : 0.f);

	if (m_pGameInstance->Key_Up(DIK_LSHIFT))
	{
		m_bHoverToggle = !m_bHoverToggle;
	}

	if (m_pGameInstance->Mouse_Pressing(DIM_LBUTTON))
	{
		if (m_fTurboBoost != 0.f) {
			m_fTurboBoost -= fTimeDelta;
			if (m_fTurboBoost <= 0.f) {
				m_fTurboBoost = 0.f;
			}
			m_Input.bTurbo = true;
			m_pInfoInstance->Event_CallBack(TEXT("BroomBooster"), &m_Input.bTurbo);
		}
		else {
			m_Input.bTurbo = false;
		}
		m_fTurboBoostChargeDelay = 0.f;
	}
	else {
		m_fTurboBoostChargeDelay += fTimeDelta;
		if (m_fTurboBoostChargeDelay >= 1.f)
		{
			m_fTurboBoost += fTimeDelta;
			if (m_fTurboBoost >= 5.f)
			{
				m_fTurboBoost = 5.f;
			}
		}
		m_Input.bTurbo = false;
		m_pInfoInstance->Event_CallBack(TEXT("BroomBooster"), &m_Input.bTurbo);
	}

	m_bTurbo = m_Input.bTurbo;
}

void CBroom::Set_AISpeed(_float speedMul, _float accelMul)
{
	m_fAISpeedMul = speedMul;
	m_fAIAccelMul = accelMul;
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
	SAFE_RELEASE(m_pBroomTrail);
	SAFE_RELEASE(m_pBoostScreenFX);

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
