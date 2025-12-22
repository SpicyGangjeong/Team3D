#include "pch.h"
#include "BroomRacerAI.h"

#include "GameInstance.h"
#include "Broom.h"
#include "RaceRing.h"
#include "CamPosition_Shoulder.h"
#include "BroomRaceManager.h"

#pragma region STATE
#include "State_Broom_Ride.h"
#include "State_Broom_Ride_Move.h"
#include "State_Hover.h"
#include "State_Fly.h"
#include "State_Broom_TurboFly.h"
#pragma endregion

#include "Layer.h"

CBroomRacerAI::CBroomRacerAI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUnit(pDevice, pContext)
{
}

CBroomRacerAI::CBroomRacerAI(const CBroomRacerAI& Prototype)
	: CUnit(Prototype)
{
}

HRESULT CBroomRacerAI::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CBroomRacerAI::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg))) {
		return E_FAIL;
	}

	if (FAILED(Ready_Components())) {
		return E_FAIL;
	}

	m_bAI = true;

	if (FAILED(Ready_Parts())) {
		return E_FAIL;
	}
#ifdef _DEBUG
	Load_KeyFrame();
#endif // _DEBUG

	m_pBroomModel = m_pBroom->Get_Component<CModel>();
	m_pBroomTransform = m_pBroom->Get_Component<CTransform>();

	SAFE_ADDREF(m_pBroomModel);
	SAFE_ADDREF(m_pBroomTransform);

	Add_FSM();

	Set_Anim();

	{
		CFSM::FSM_DESC FSMDesc{};
		FSMDesc.pStates = &m_States;
		FSMDesc.pStateMask = &m_iStateMask;

		m_pFSM->Bind_States(FSMDesc);
		m_pFSM->Change_State(FSMSTATE::BROOM_RIDE_MOVE);
	}

	_float X = m_pGameInstance->Real_Random_Float(-200.f, 200.f);
	_float Y = m_pGameInstance->Real_Random_Float(-200.f, 200.f);
	_float Z = m_pGameInstance->Real_Random_Float(-200.f, 200.f);

	m_pTransformCom->Set_State(STATE::POSITION, XMVectorSet(X, Y, Z, 1.f));

#ifdef _DEBUG
	m_BasicEffect = make_unique<BasicEffect>(m_pDevice);
	m_BasicEffect->SetVertexColorEnabled(true);
	m_BasicEffect->SetView(m_pGameInstance->Get_Transform_Matrix(D3DTS::VIEW));
	m_BasicEffect->SetProjection(m_pGameInstance->Get_Transform_Matrix(D3DTS::PROJ));

	m_Batch = make_unique<PrimitiveBatch<VertexPositionColor>>(m_pContext);
#endif // _DEBUG



	m_pBroom->Set_Ride(true);

	m_pBroomRaceManager = static_cast<CBroomRaceManager*>(pArg);

	CBroomRaceManager::RacerInfo Info;

	Info.pRacer = this;
	Info.curRing = 0;
	Info.prevPos = Get_WorldPostion();

	m_pBroomRaceManager->Push_BroomRacer(Info);

	return S_OK;
}

void CBroomRacerAI::Priority_Update(_float fTimeDelta)
{
	__super::Priority_Update(fTimeDelta);

	m_pRaceRing = m_pBroomRaceManager->GetTargetRing(this);
}

void CBroomRacerAI::Update(_float fTimeDelta)
{
	m_pFSM->Update_State(fTimeDelta);

	float ratio = m_pModelCom->Get_CurrentTrackProgressRatio();

	float ease = 1.f;
	if (ratio < 0.4f)
		ease = 1.15f;
	else if (ratio > 0.85f)
		ease = 0.85f;

	m_pModelCom->Play_Animation(fTimeDelta * ease, m_pTransformCom);

	Play_Event();

	__super::Update(fTimeDelta);
#ifdef _DEBUG
	Describe_Entity();
#endif // _DEBUG
}

void CBroomRacerAI::Late_Update(_float fTimeDelta)
{
	Set_Input(fTimeDelta);

	m_pGameInstance->Add_RenderGroup(RENDER::NONBLEND, this);
	Set_Shadow(m_pGameInstance->IsIn_ShadowViewFrustum(m_pTransformCom->Get_State(STATE::POSITION), m_pTransformCom->Get_Radius()));

	__super::Late_Update(fTimeDelta);

}

HRESULT CBroomRacerAI::Render()
{
	if (!m_bVisible) {
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
HRESULT CBroomRacerAI::Render_Shadow(SHADOW eType)
{
	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", m_pTransformCom->Get_WorldMatrixPtr()))) {
		return E_FAIL;
	}
	if (FAILED(m_pGameInstance->Bind_Shadow_Resource(m_pShaderCom, "g_ViewMatrix", D3DTS::VIEW, eType))) {
		return E_FAIL;
	}
	if (FAILED(m_pGameInstance->Bind_Shadow_Resource(m_pShaderCom, "g_ProjMatrix", D3DTS::PROJ, eType))) {
		return E_FAIL;
	}
	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; i++)
	{
		if (FAILED(m_pShaderCom->Bind_Matrices(
			"g_OffsetMatrix",
			m_pModelCom->Get_OffsetMatrix(i).data(),
			(_int)m_pModelCom->Get_OffsetMatrix(i).size()
		)))
		{
			return E_FAIL;

		}
		if (FAILED(m_pShaderCom->Begin(ENUM_CLASS(SHADER_PASS_ANIM::SHADOW)))) {
			return E_FAIL;
		}

		m_pModelCom->Bind_OutPut_SRV_VS(31, 0);

		if (FAILED(m_pModelCom->Render(i))) {
			return E_FAIL;
		}
	}

	return S_OK;
}
void CBroomRacerAI::OnCollision(CGameObject* pOther, void* pDesc)
{
}
void CBroomRacerAI::OnHit(CGameObject* pOther, CGameObject* pCaller)
{
}

HRESULT CBroomRacerAI::Ready_Components()
{
	CTransform::TRANSFORM_DESC Desc = {};

	Desc.fSpeedPerSec = 10.f;
	Desc.fRotationPerSec = XMConvertToRadians(180.0f);
	Desc.fRadius = 10.f;

	if (FAILED(__super::Ready_Components(&Desc))) {
		return E_FAIL;
	}

	m_strModelPrototypeTag = TEXT("Prototype_Component_Npc_Model");

	/* Com_Model */
	if (FAILED(__super::Add_Asset_Component(g_iStaticLevel, m_strModelPrototypeTag,
		reinterpret_cast<CComponent**>(&m_pModelCom)))) {
		return E_FAIL;
	}

	/* Com_Shader */
	if (FAILED(__super::Add_Asset_Component(g_iStaticLevel, FX_ANIMMESH,
		reinterpret_cast<CComponent**>(&m_pShaderCom)))) {
		return E_FAIL;
	}
	LightDesc.eType = LIGHT::POINT;
	LightDesc.fRange = 10.f;
	LightDesc.iLevel = NEXT_LEVEL;
	LightDesc.pPosition = m_pTransformCom->Get_StatePtr(STATE::POSITION);
	LightDesc.vAmbient = CMyTools::ColorRGB_A_HEXtoFLOAT4(0xffffff, 1.f);
	LightDesc.vDiffuse = CMyTools::ColorRGB_A_HEXtoFLOAT4(0xffffff, 1.f);
	LightDesc.vSpecular = CMyTools::ColorRGB_A_HEXtoFLOAT4(0xffffff, 1.f);
	if (FAILED(Add_Component<CLight>(g_iStaticLevel, &m_pLightCom, &LightDesc))) {
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CBroomRacerAI::Ready_Parts()
{
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer<CBroom>(g_iStaticLevel, NEXT_LEVEL, LAYER_ITEM, nullptr, this, &m_pBroom))) {
		return E_FAIL;
	}

	SAFE_ADDREF(m_pBroom);


	return S_OK;
}

HRESULT CBroomRacerAI::Bind_ShaderResources()
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


_float CBroomRacerAI::ComputeTurnToRing()
{
	_vector toRing = m_pRaceRing->Get_WorldPostion() - m_pBroom->Get_WorldPostion();
	toRing = XMVector3Normalize(XMVectorSetY(toRing, 0.f));

	_vector myLook = m_pBroom->Get_Component<CTransform>()->Get_State(STATE::LOOK);
	myLook = XMVector3Normalize(XMVectorSetY(myLook, 0.f));

	_vector cross = XMVector3Cross(myLook, toRing);
	_float turn = XMVectorGetY(cross);

	_float DEAD = 0.05f;
	if (fabsf(turn) < DEAD)
		return 0.f;

	return clamp(turn, -1.f, 1.f);
}

_float CBroomRacerAI::ComputeHeightAdjust()
{
	_float dy = (XMVectorGetY(m_pRaceRing->Get_WorldPostion()) + 15.f)
		- XMVectorGetY(m_pBroom->Get_WorldPostion());

	_float DEAD = 5.f;
	_float RANGE = 40.f;

	if (fabsf(dy) < DEAD)
		return 0.f;

	return clamp(dy / RANGE, -1.f, 1.f);
}



CBroomRacerAI* CBroomRacerAI::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CBroomRacerAI* pInstance = new CBroomRacerAI(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CBroomRacerAI");
		SAFE_RELEASE(pInstance);
	}
	return pInstance;
}

CGameObject* CBroomRacerAI::Clone(void* pArg, CGameObject* pOwner)
{
	CBroomRacerAI* pInstance = new CBroomRacerAI(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CBroomRacerAI");
		SAFE_RELEASE(pInstance);
	}
	return pInstance;
}

void CBroomRacerAI::Free()
{
	__super::Free();

	SAFE_RELEASE(m_pLightCom);
	SAFE_RELEASE(m_pBroomModel);
	SAFE_RELEASE(m_pBroomTransform);
	SAFE_RELEASE(m_pBroom);
	SAFE_RELEASE(m_pCamPosition_ShoulderPart);
}
#ifdef _DEBUG

void CBroomRacerAI::Describe_Entity()
{
}

#endif // _DEBUG

void CBroomRacerAI::Behavior_Broom_Ride_MoveEnter()
{
	m_pFSM->Enable_State(FSMSTATE::BROOM_RIDE_MOVE);
}

HRESULT CBroomRacerAI::Behavior_Broom_Ride_MoveExitCheck(_float fTimeDelta)
{
	if (m_pBroom->Get_Hover())
	{
		m_pFSM->Change_State(FSMSTATE::HOVER);
	}
	else if (m_pBroom->Get_Turbo()) {
		m_pFSM->Change_State(FSMSTATE::TURBOFLY);
	}
	else {
		m_pFSM->Change_State(FSMSTATE::FLY);
	}

	return E_FAIL;
}

void CBroomRacerAI::Behavior_Broom_Ride_MoveExit()
{
	m_pFSM->Disable_State(FSMSTATE::BROOM_RIDE_MOVE);
}

void CBroomRacerAI::Behavior_Broom_HoverEnter()
{
	pair<_uint, _bool> pairAnimInfo;
	m_pFSM->Enable_State(FSMSTATE::HOVER);
	pairAnimInfo = m_Animation[STATEANIM::BROOM_HOVER_IDLE];
	m_pModelCom->Set_AnimationIndex(pairAnimInfo.first, pairAnimInfo.second);
}

HRESULT CBroomRacerAI::Behavior_Broom_HoverExitCheck(_float fTimeDelta)
{
	_uint iCurrAnimIndex = m_pModelCom->Get_AnimIndex();
	pair<_uint, _bool> pairAnimInfo;

	if (m_pBroom->Get_Hover())
	{
		pairAnimInfo = m_Animation[STATEANIM::BROOM_HOVER_IDLE];
		m_pModelCom->Set_AnimationIndex(pairAnimInfo.first, pairAnimInfo.second);
	}

	if (!m_pBroom->Get_Hover())
	{
		m_pFSM->Change_State(FSMSTATE::BROOM_RIDE_MOVE);
	}

	return E_FAIL;
}

void CBroomRacerAI::Behavior_Broom_HoverExit()
{
	m_pFSM->Disable_State(FSMSTATE::HOVER);
}

void CBroomRacerAI::Behavior_Broom_FlyEnter()
{
	m_pFSM->Enable_State(FSMSTATE::FLY);
}

HRESULT CBroomRacerAI::Behavior_Broom_FlyExitCheck(_float fTimeDelta)
{
	_uint iCurrAnimIndex = m_pModelCom->Get_AnimIndex();
	pair<_uint, _bool> pairAnimInfo;

	_float Z = 1.f;
	_float Y = ComputeHeightAdjust();
	_float X = ComputeTurnToRing();

	if (!m_pBroom->Get_Hover())
	{

		if (Y > 0.f)
		{
			pairAnimInfo = m_Animation[STATEANIM::BROOM_UP];
		}
		else if (Y < 0.f)
		{
			pairAnimInfo = m_Animation[STATEANIM::BROOM_DOWN];
		}

		_float ax = fabsf(X);
		_float ay = fabsf(Y);
		_float az = fabsf(Z);

		if (ay > ax && ay > az)
		{
			if (Y > 0)
				pairAnimInfo = m_Animation[STATEANIM::BROOM_UP];
			else
				pairAnimInfo = m_Animation[STATEANIM::BROOM_DOWN];
		}
		else if (ax >= az)
		{
			if (ay >= ax)
			{
				if (Y > 0)
					pairAnimInfo = m_Animation[STATEANIM::BROOM_UP];
				else
					pairAnimInfo = m_Animation[STATEANIM::BROOM_DOWN];
			}
			else {
				if (X < 0)
					pairAnimInfo = m_Animation[STATEANIM::BROOM_LEFT];
				else if (X > 0)
					pairAnimInfo = m_Animation[STATEANIM::BROOM_RIGHT];
				else {
					pairAnimInfo = m_Animation[STATEANIM::BROOM_FWD];
				}
			}
		}
		else
		{
			if (Y > 0)
				pairAnimInfo = m_Animation[STATEANIM::BROOM_UP];
			else if (Y< 0)
				pairAnimInfo = m_Animation[STATEANIM::BROOM_DOWN];
			else {
				pairAnimInfo = m_Animation[STATEANIM::BROOM_FWD];
			}
		}
		

		m_pModelCom->Set_AnimationIndex(pairAnimInfo.first, pairAnimInfo.second, 1.f, true);
	}

	if (m_pBroom->Get_Hover())
	{
		m_pFSM->Change_State(FSMSTATE::BROOM_RIDE_MOVE);
	}

	return E_FAIL;
}

void CBroomRacerAI::Behavior_Broom_FlyExit()
{
	m_pFSM->Disable_State(FSMSTATE::FLY);
}

void CBroomRacerAI::Behavior_Broom_TurboFlyEnter()
{
	m_pFSM->Enable_State(FSMSTATE::TURBOFLY);
}

HRESULT CBroomRacerAI::Behavior_Broom_TurboFlyExitCheck(_float fTimeDelta)
{
	_uint iCurrAnimIndex = m_pModelCom->Get_AnimIndex();
	pair<_uint, _bool> pairAnimInfo;
	if (!m_pBroom->Get_Hover())
	{
		pairAnimInfo = Get_AnimInfo(STATEANIM::BROOM_TURBO_FWD);

		m_pModelCom->Set_AnimationIndex(pairAnimInfo.first, pairAnimInfo.second, 1.f, true);
	}

	if (m_pBroom->Get_Hover())
	{
		m_pFSM->Change_State(FSMSTATE::BROOM_RIDE_MOVE);
	}

	return E_FAIL;
}

void CBroomRacerAI::Behavior_Broom_TurboFlyExit()
{
	m_pFSM->Disable_State(FSMSTATE::TURBOFLY);
}


void CBroomRacerAI::Attach_Broom()
{
	_matrix BroomWorld = XMLoadFloat4x4(m_pBroomTransform->Get_WorldMatrixPtr());
	_matrix BoneLocal = XMLoadFloat4x4(m_pBroomModel->Get_BoneMatrixPtr("broomSocket"));

	_vector Scale, Rot, Trans;

	XMMatrixDecompose(&Scale, &Rot, &Trans, BoneLocal);

	_matrix BoneNoScale = XMMatrixRotationQuaternion(Rot) * XMMatrixTranslationFromVector(Trans);

	_matrix SocketWorld = BoneNoScale * BroomWorld;

	_matrix FixRot = XMMatrixRotationY(XMConvertToRadians(180.f));

	_matrix FinalWorld = FixRot * SocketWorld;

	m_pTransformCom->Set_WorldMatrix(FinalWorld);
}


void CBroomRacerAI::Add_FSM()
{
#pragma region Behavior_Broom_Ride
	{
		CState_Broom_Ride_Move::STATE_BROOM_RIDE_MOVE_DESC Desc{};
		Desc.pOwner = this;
		Desc.funcEnterEvent = [this]() { Behavior_Broom_Ride_MoveEnter(); };
		Desc.funcExitCheck = [this](_float fTimedelta) { return Behavior_Broom_Ride_MoveExitCheck(fTimedelta); };
		Desc.funcExitEvent = [this]() { Behavior_Broom_Ride_MoveExit(); };
		Desc.funcPriorityUpdate = [this](_float fTimeDelta) {
			_uint iCurrAnimIndex = m_pModelCom->Get_AnimIndex();
			if (!m_bOnce) {
				m_pBroomTransform->Set_WorldMatrix(m_pTransformCom->Get_XMWorldMatrix());
				m_pBroomTransform->Set_State(STATE::POSITION, m_pBroomTransform->Get_State(STATE::POSITION) + XMVectorSet(0.f, 2.f, 0.f, 0.f));
				m_bOnce = true;
			}
			Attach_Broom();
			};
		Desc.funcLateUpdate = nullptr;
		m_States.emplace(FSMSTATE::BROOM_RIDE_MOVE, CState_Broom_Ride_Move::Create(&Desc));
	}

	{
		CState_Hover::STATE_HOVER_DESC Desc{};
		Desc.pOwner = this;
		Desc.funcEnterEvent = [this]() { Behavior_Broom_HoverEnter(); };
		Desc.funcExitCheck = [this](_float fTimedelta) { return Behavior_Broom_HoverExitCheck(fTimedelta); };
		Desc.funcExitEvent = [this]() { Behavior_Broom_HoverExit(); };
		Desc.funcPriorityUpdate = [this](_float fTimeDelta) {
			_uint iCurrAnimIndex = m_pModelCom->Get_AnimIndex();
			Attach_Broom();
			};
		Desc.funcLateUpdate = nullptr;
		m_States.emplace(FSMSTATE::HOVER, CState_Hover::Create(&Desc));
	}

	{
		CState_Fly::STATE_FLY_DESC Desc{};
		Desc.pOwner = this;
		Desc.funcEnterEvent = [this]() { Behavior_Broom_FlyEnter(); };
		Desc.funcExitCheck = [this](_float fTimedelta) { return Behavior_Broom_FlyExitCheck(fTimedelta); };
		Desc.funcExitEvent = [this]() { Behavior_Broom_FlyExit(); };
		Desc.funcPriorityUpdate = [this](_float fTimeDelta) {
			_uint iCurrAnimIndex = m_pModelCom->Get_AnimIndex();
			Attach_Broom();
			};
		Desc.funcLateUpdate = nullptr;
		m_States.emplace(FSMSTATE::FLY, CState_Fly::Create(&Desc));
	}

	{
		CState_Broom_TurboFly::STATE_BROOM_TURBOFLY_DESC Desc{};
		Desc.pOwner = this;
		Desc.funcEnterEvent = [this]() { Behavior_Broom_TurboFlyEnter(); };
		Desc.funcExitCheck = [this](_float fTimedelta) { return Behavior_Broom_TurboFlyExitCheck(fTimedelta); };
		Desc.funcExitEvent = [this]() { Behavior_Broom_TurboFlyExit(); };
		Desc.funcPriorityUpdate = [this](_float fTimeDelta) {
			_uint iCurrAnimIndex = m_pModelCom->Get_AnimIndex();
			Attach_Broom();
			};
		Desc.funcLateUpdate = nullptr;
		m_States.emplace(FSMSTATE::TURBOFLY, CState_Broom_TurboFly::Create(&Desc));
	}

#pragma endregion

}

void CBroomRacerAI::Set_Anim()
{
	m_Animation[STATEANIM::BROOM_IDLE] = { 710,true };
	m_Animation[STATEANIM::BROOM_FWD] = { 711,true };
	m_Animation[STATEANIM::BROOM_LEFT] = { 713,true };
	m_Animation[STATEANIM::BROOM_RIGHT] = { 714,true };
	m_Animation[STATEANIM::BROOM_DOWN] = { 712,true };
	m_Animation[STATEANIM::BROOM_UP] = { 715,true };
	m_Animation[STATEANIM::BROOM_REVELIO] = { 678,false };

	m_Animation[STATEANIM::BROOM_MOUNT] = { 734,false };
	m_Animation[STATEANIM::BROOM_MOUNT_END] = { 737,false };
	m_Animation[STATEANIM::BROOM_HOVER_START] = { 699,false };
	m_Animation[STATEANIM::BROOM_DISMOUNT] = { 738,false };


	m_Animation[STATEANIM::BROOM_HOVER_IDLE] = { 703,true };
	m_Animation[STATEANIM::BROOM_HOVER_FWD] = { 700,true };
	m_Animation[STATEANIM::BROOM_HOVER_LEFT] = { 701,true };
	m_Animation[STATEANIM::BROOM_HOVER_RIGHT] = { 702,true };
	m_Animation[STATEANIM::BROOM_HOVER_REVELIO] = { 721,false };

	m_Animation[STATEANIM::BROOM_TURBO_DOWN] = { 716,true };
	m_Animation[STATEANIM::BROOM_TURBO_FWD] = { 717,true };
	m_Animation[STATEANIM::BROOM_TURBO_LEFT] = { 718,true };
	m_Animation[STATEANIM::BROOM_TURBO_RIGHT] = { 719,true };
	m_Animation[STATEANIM::BROOM_TURBO_UP] = { 720,true };
}

void CBroomRacerAI::Set_Input(_float fTimeDelta)
{
	_float targetX = ComputeTurnToRing();
	_float targetY = ComputeHeightAdjust();

	_float SMOOTH = 6.f;

	m_fSmoothX += (targetX - m_fSmoothX) * SMOOTH * fTimeDelta;
	m_fSmoothY += (targetY - m_fSmoothY) * SMOOTH * fTimeDelta;

	CBroom::BroomInput InputDesc;
	InputDesc.Z = 1.f;
	InputDesc.X = m_fSmoothX;
	InputDesc.Y = m_fSmoothY;
	InputDesc.bHoverToggle = false;
	InputDesc.bTurbo = false;

	m_pBroom->Set_Input(InputDesc);
}

