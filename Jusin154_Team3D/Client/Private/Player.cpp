#include "pch.h"
#include "Player.h"

#include "GameInstance.h"
#include "InfoInstance.h"
#include "CamPosition_Socket.h"
#include "Camera_Gaze.h"
#include "CamPosition_Arm.h"
#include "Wand.h"
#include "Item_Potion.h"
#include "Character_Controller.h"
#include "CallBack_Playable_Behavior.h"
#include "CamPosition_Shoulder.h"
#include "CallBack_Playable_HitReport.h"
#include "Monster.h"
#include "Broom.h"
#include "MapElement_Interactable.h"

#pragma region STATE
#include "State_Idle.h"
#include "State_Dodge.h"
#include "State_Jump.h"
#include "State_Land.h"
#include "State_Move.h"
#include "State_Combat.h"
#include "State_Hit.h"
#include "State_Broom_Ride.h"
#pragma endregion

#include "Layer.h"
#include "EffectPool.h"

CPlayer::CPlayer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUnit(pDevice, pContext)
{
}

CPlayer::CPlayer(const CPlayer& Prototype)
	: CUnit(Prototype),
	m_pInfoInstance(CInfoInstance::GetInstance())
{
}

HRESULT CPlayer::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CPlayer::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg))) {
		return E_FAIL;
	}

	if (FAILED(Ready_Components())) {
		return E_FAIL;
	}

	if (FAILED(Ready_Parts())) {
		return E_FAIL;
	}
#ifdef _DEBUG
	Load_KeyFrame();
#endif // _DEBUG

	m_pBroom = m_pGameInstance->Get_Layer(NEXT_LEVEL, LAYER_ITEM)->Get_Object<CBroom>();
	m_pBroomModel = m_pBroom->Get_Component<CModel>();
	m_pBroomTransform = m_pBroom->Get_Component<CTransform>();
	SAFE_ADDREF(m_pBroom);
	SAFE_ADDREF(m_pBroomModel);
	SAFE_ADDREF(m_pBroomTransform);

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

	m_pEffectPool = m_pGameInstance->Get_Layer(NEXT_LEVEL, TEXT("Layer_EffectPool"))->Get_Object<CEffectPool>();
	SAFE_ADDREF(m_pEffectPool);

	m_pInfoInstance->Regist_PlayerAlly(this);
	m_pInfoInstance->Set_Damage(m_pStat->Get_Stat().fDamage);

	m_pCharacter_Controller->Set_Position(XMVectorSet(-34.f, 5, -11.4f, 1.f));
	m_pTransformCom->Set_State(STATE::POSITION, XMVectorSet(-34.f, 5, -11.4f, 1.f));

#ifdef _DEBUG
	m_BasicEffect = make_unique<BasicEffect>(m_pDevice);
	m_BasicEffect->SetVertexColorEnabled(true);
	m_BasicEffect->SetView(m_pGameInstance->Get_Transform_Matrix(D3DTS::VIEW));
	m_BasicEffect->SetProjection(m_pGameInstance->Get_Transform_Matrix(D3DTS::PROJ));

	m_Batch = make_unique<PrimitiveBatch<VertexPositionColor>>(m_pContext);
#endif // _DEBUG

	// UI 연동 추가
	m_pInfoInstance->Add_Event(TEXT("UseSpell"), [this](void* p) {this->Get_Spell(*reinterpret_cast<_int*>(p)); });
	m_pInfoInstance->Add_Event(TEXT("Canvas_Change"), [this](void* p) {this->Get_UIState(*reinterpret_cast<_int*>(p)); });
	return S_OK;
}

void CPlayer::Priority_Update(_float fTimeDelta)
{
	ReLockOnTarget();
	SetGravity();
	m_pTransformCom->RewindMomentum();

	__super::Priority_Update(fTimeDelta);

}

void CPlayer::Update(_float fTimeDelta)
{
	Update_CameraCoordinateSystem(fTimeDelta);
	UpdateGrapInteractive(fTimeDelta);
	
	m_pFSM->Update_State(fTimeDelta);

	m_pModelCom->Play_Animation(fTimeDelta, m_pTransformCom);

	Play_Event();
	
__super::Update(fTimeDelta);
#ifdef _DEBUG
	Describe_Entity();
#endif // _DEBUG

	{ // 세트
		m_pCallBack_HitReport->BeginFrame();
		m_pCharacter_Controller->Move(fTimeDelta);
		m_pCallBack_HitReport->Set_CurrentSlop();
	}
	if (m_pGameInstance->Mouse_Down(DIM_RBUTTON)){
		m_pInfoInstance->Mouse_Input(ENUM_CLASS(KEYINPUT::DIM_RBUTTON_DOWN));
	}
	if (m_pGameInstance->Mouse_Up(DIM_RBUTTON))
	{
		m_pInfoInstance->Mouse_Input(ENUM_CLASS(KEYINPUT::DIM_RBUTTON_UP));
		m_bAim = false; 
	}

	if (m_pGameInstance->Key_Down(DIK_1)) {
		if (m_pModelCom->Get_SecondAnimIndex() == m_Animation[STATEANIM::LUMOS].first)
		{
			m_pModelCom->Set_Second_AnimationIndex(ENUM_CLASS(BLEND_BONE::SHOULDER_R), m_Animation[STATEANIM::LUMOS_STOP].first, m_Animation[STATEANIM::LUMOS_STOP].second);
		}
	}

}

void CPlayer::UpdateGrapInteractive(_float fTimeDelta)
{
	if (nullptr != m_pGrapInteractive) {
		m_vGrapInteratableLerp.x += fTimeDelta;
		m_pGrapInteractive->GrapToPlayer(m_pTransformCom->Get_State(STATE::POSITION) + m_pCamPosition_ShoulderPart->Get_ShoulderGlobalPos(), m_vGrapInteratableLerp.x);
		if (m_vGrapInteratableLerp.x > m_vGrapInteratableLerp.y) {
			m_vGrapInteratableLerp.x -= m_vGrapInteratableLerp.y;
		}
	}
}

void CPlayer::Update_CameraShake(_float fTimeDelta)
{
	if (true == m_bCameraShake) {
		m_vCameraShakeTimer.x += fTimeDelta;
		if (m_vCameraShakeTimer.x > m_vCameraShakeTimer.y) {
			m_vCameraShakeTimer.x = 0.f;
			m_pCamPosition_ShoulderPart->Set_CameraShake(0.f, 0.f);
			m_bCameraShake = false;
		}
		else {
			_float fIntense = { 1.f - m_vCameraShakeTimer.x / m_vCameraShakeTimer.y };
			fIntense *= fIntense;
			m_pCamPosition_ShoulderPart->Set_CameraShake(
				fIntense * m_pGameInstance->Real_Random_Float(-m_fCameraShakeIntense, m_fCameraShakeIntense),
				fIntense * m_pGameInstance->Real_Random_Float(-m_fCameraShakeIntense, m_fCameraShakeIntense)
			);
		}
	}
}

void CPlayer::Late_Update(_float fTimeDelta)
{
	m_pTransformCom->Set_State(STATE::POSITION, m_pCharacter_Controller->Get_FootPosition());

	m_pGameInstance->Add_RenderGroup(RENDER::NONBLEND, this);
	m_pGameInstance->Add_RenderGroup(RENDER::SHADOW, this);

	__super::Late_Update(fTimeDelta);

	if (nullptr != m_LockOnInfo.pUnit) {
		if (false == m_LockOnInfo.pUnit->isDead()) {
			static_cast<CMonster*>(m_LockOnInfo.pUnit)->Set_DrawOutLine();
		}
		else {
			m_LockOnInfo.pUnit = nullptr;
		}
	}
	if (nullptr != m_LockOnInfo.pInteractive) {
		static_cast<CMapElement_Interactable*>(m_LockOnInfo.pInteractive)->Set_DrawOutLine();
	}

	if (m_bLookAt && m_LockOnInfo.pUnit)
	{
		m_pTransformCom->LookAt_Lerp(m_LockOnInfo.pUnit->Get_WorldPostion(), fTimeDelta, 5.f);
	}
	////////////////////////////////////////////////////////////////////////////
	_vector look = XMVector3Normalize(m_pTransformCom->Get_State(STATE::LOOK));

	_vector worldUp = XMVectorSet(0.f, 1.f, 0.f, 0.f);

	_vector right = XMVector3Normalize(XMVector3Cross(worldUp, look));
	_vector up = XMVector3Normalize(XMVector3Cross(look, right));

	m_pTransformCom->Set_State(STATE::RIGHT, right);
	m_pTransformCom->Set_State(STATE::UP, up);
	m_pTransformCom->Set_State(STATE::LOOK, look);
	////////////////////////////////////////////////////////////////////////////
	

}

HRESULT CPlayer::Render()
{
	if (!m_bVisible){
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

#ifdef _DEBUG
	m_pCharacter_Controller->Render();
	//m_pRigidBody->Render();
	Render_CameraCoordinateSystem();
#endif

	return S_OK;
}
HRESULT CPlayer::Render_Shadow()
{
	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", m_pTransformCom->Get_WorldMatrixPtr()))) {
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
void CPlayer::OnCollision(CGameObject* pOther, void* pDesc)
{
	ON_COLLISION_INFO* CollisionDesc = static_cast<ON_COLLISION_INFO*>(pDesc);
	if (CollisionDesc) {
		Check_HitAngle(XMLoadFloat4(&CollisionDesc->vHitDir));
	}
	else {
		m_fHitDegree = -1.f;
	}

	m_pFSM->Change_State(FSMSTATE::HIT);
}
void CPlayer::OnHit(CGameObject* pOther, CGameObject* pCaller)
{
}

void CPlayer::Start_CameraShake(_float fTime, _float fIntense)
{
	m_vCameraShakeTimer.x = 0.f;
	m_vCameraShakeTimer.y = fTime;
	m_fCameraShakeIntense = fIntense;
	m_bCameraShake = true;
}
#ifdef _DEBUG

void CPlayer::Render_CameraCoordinateSystem()
{
	m_Batch->Begin();

	const _float fArrowLength = 2.0f;
	_vector xmvLook = XMVector4Normalize(XMVectorSetY(m_pTransformCom->Get_State(STATE::LOOK), 0.f));
	_float2 vLook = { XMVectorGetX(xmvLook), XMVectorGetZ(xmvLook) };
	GUI::Begin("Player_CAM_COOORD");
	GUI::Text("%d", m_LockOnInfo.pUnit);

	GUI::Text("W : %.2f, %.2f, %.2f", m_vCameraLookDir.x, 0.f, m_vCameraLookDir.z);
	GUI::Text("A : %.2f, %.2f, %.2f", -m_vCameraRightDir.x, 0.f, -m_vCameraRightDir.z);
	GUI::Text("S : %.2f, %.2f, %.2f", -m_vCameraLookDir.x, 0.f, -m_vCameraLookDir.z);
	GUI::Text("D : %.2f, %.2f, %.2f", m_vCameraRightDir.x, 0.f, m_vCameraRightDir.z);
	
	_float  fButtonSize = 45.f;
	GUI::Button("##0", { fButtonSize, fButtonSize }); GUI::SameLine();
	GUI::Button(("W : " + to_string(XMConvertToDegrees(CMyTools::Get_Direction2D(vLook, { m_vCameraLookDir.x , m_vCameraLookDir.z })))).c_str(), { fButtonSize, fButtonSize }); GUI::SameLine();
	GUI::Button("##2", { fButtonSize, fButtonSize });
	GUI::Button(("A : " + to_string(XMConvertToDegrees(CMyTools::Get_Direction2D(vLook, { -m_vCameraRightDir.x , -m_vCameraRightDir.z })))).c_str(), { fButtonSize, fButtonSize }); GUI::SameLine();
	GUI::Button("##4", { fButtonSize, fButtonSize }); GUI::SameLine();
	GUI::Button(("D : " + to_string(XMConvertToDegrees(CMyTools::Get_Direction2D(vLook, { m_vCameraRightDir.x , m_vCameraRightDir.z })))).c_str(), { fButtonSize, fButtonSize });
	GUI::Button("##6", { fButtonSize, fButtonSize }); GUI::SameLine();
	GUI::Button(("S : " + to_string(XMConvertToDegrees(CMyTools::Get_Direction2D(vLook, { -m_vCameraLookDir.x , -m_vCameraLookDir.z })))).c_str(), { fButtonSize, fButtonSize }); GUI::SameLine();
	GUI::Button("##8", { fButtonSize, fButtonSize });
	GUI::End();
	//W CMyTools::Get_Direction2D(vLook, { m_vCameraLookDir.x ,		m_vCameraLookDir.z })
	//A CMyTools::Get_Direction2D(vLook, { -m_vCameraRightDir.x , -	m_vCameraRightDir.z })
	//S CMyTools::Get_Direction2D(vLook, { m_vCameraRightDir.x ,	m_vCameraRightDir.z })
	//D CMyTools::Get_Direction2D(vLook, { -m_vCameraLookDir.x , -	m_vCameraLookDir.z })
	m_Batch->DrawLine( // W
		VertexPositionColor(fArrowLength * -XMLoadFloat3(&m_vCameraLookDir), DirectX::Colors::GhostWhite),
		VertexPositionColor(fArrowLength * XMLoadFloat3(&m_vCameraLookDir), DirectX::Colors::Blue)
	);
	m_Batch->DrawLine( // D
		VertexPositionColor(fArrowLength * -XMLoadFloat3(&m_vCameraRightDir), DirectX::Colors::GhostWhite),
		VertexPositionColor(fArrowLength * XMLoadFloat3(&m_vCameraRightDir), DirectX::Colors::Red)
	);
	m_Batch->DrawLine( // PlayerLook
		VertexPositionColor(XMVectorZero(), DirectX::Colors::GhostWhite),
		VertexPositionColor(fArrowLength * xmvLook, DirectX::Colors::HotPink)
	);

	m_Batch->End();
}
#endif // _DEBUG

HRESULT CPlayer::Ready_Components()
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

	m_pStat = m_pInfoInstance->Get_PlayerStatPtr();
	m_Components.push_back(m_pStat);
	SAFE_ADDREF(m_pStat);
	SAFE_ADDREF(m_pStat);

	{ // CCT
		CCharacter_Controller::Character_Controller_DESC Desc{};

		Desc.iSubKind = ENUM_CLASS(PXOBJECT::PLAYER);
		Desc.pTransform = m_pTransformCom;
		Desc.eBodyType = ACTOR::CAPSULE;
		Desc.fContactOffset = 0.001f;
		Desc.fMaterial = { 1.2f, 1.0f, 0.0f };
		Desc.bAutoStepping = { false };
		Desc.fStepOffset = { 0.001f };
		Desc.fRadius = 0.5f;
		Desc.fHeight = 0.6f;
		Desc.pCallback_HitReport = m_pCallBack_HitReport = CCallBack_Playable_HitReport::Create();
		Desc.pCallback_Behavior = m_pCallBack_Behavior = CCallBack_Playable_Behavior::Create();
		Desc.eClimbingMode = PSX::PxCapsuleClimbingMode::eEASY;
		Desc.fWalkableSlope = 45.f;
		if (FAILED(Add_Asset_Component(g_iStaticLevel, TEXT("PHYSX_CCT_CAPSULE"), (CComponent**)&m_pCharacter_Controller, &Desc))) {
			return E_FAIL;
		}
		m_pCharacter_Controller->SetGravity(false);
	}

	{ // DO
		CRigidBody_Dynamic::RIGIDBODY_DYNAMIC_DESC Desc{};
		Desc.iSubKind = ENUM_CLASS(PXOBJECT::PLAYER);
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

	CItem_Potion::POTION_DESC PotionDesc{};

	PotionDesc.pParentTransform = m_pTransformCom;
	PotionDesc.pSocketMatrices = m_pModelCom->Get_BoneMatrixPtr("SKT_LeftHand");

	if (FAILED(Add_PartObject<CItem_Potion>("Potion", g_iStaticLevel, nullptr, &PotionDesc)))
	{
		return E_FAIL;
	}

	Get_PartObject<CItem_Potion>()->Set_Visible(false);

	{
		CCamPosition_Shoulder::CAMERA_SHOULDER_DESC Desc;
		Desc.pParentTransform = m_pTransformCom;
		if (FAILED(Add_PartObject<CCamPosition_Shoulder>("Cam_Shoulder_Part", g_iStaticLevel, &m_pCamPosition_ShoulderPart, &Desc))) {
			return E_FAIL;
		}
	}

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
void CPlayer::ReLockOnTarget()
{
	m_pInfoInstance->Get_LockOnInfo(m_LockOnInfo);
	if (nullptr != m_LockOnInfo.pUnit) {
		if (true == m_LockOnInfo.pUnit->isDead()) {
			m_LockOnInfo.pUnit = nullptr;
		}
	}
	//m_pLockOnMonster->Get_State
}

void CPlayer::SetGravity()
{
	PSX::PxControllerCollisionFlags eCollisionFlags = m_pCharacter_Controller->Get_CollisionFlags();
	eCollisionFlags;
	if (	false == eCollisionFlags.isSet(PSX::PxControllerCollisionFlag::Enum::eCOLLISION_DOWN) 
		 &&	false == eCollisionFlags.isSet(PSX::PxControllerCollisionFlag::Enum::eCOLLISION_SIDES)) {
		if (false == m_pFSM->IsEnable(FSMSTATE::JUMP)) { // 벽에 닿지 않았는데 점프 중이 아닐 땐 중력 on
			m_pCharacter_Controller->SetGravity(true);
		}
		else { // 점프 중일 땐 off
			m_pCharacter_Controller->SetGravity(false);
		}
	}
	else { // 벽에 닿는중일 땐 항상 중력 off
		m_pCharacter_Controller->SetGravity(false);
	}
}

void CPlayer::Update_CameraCoordinateSystem(_float fTimeDelta)
{
	_vector xmvCameraLook = XMVector3Normalize(XMVectorSetY(m_pGameInstance->Get_CameraLook(), 0.f));
	_vector xmvUp = XMVectorSet(0.f, 1.f, 0.f, 0.f);
	XMStoreFloat3(&m_vCameraRightDir, XMVector3Normalize(XMVector3Cross(xmvUp, xmvCameraLook)));
	XMStoreFloat3(&m_vCameraLookDir, xmvCameraLook);
	m_pInfoInstance->Update_CameraCoordinateSystem(m_vCameraLookDir, m_vRimLightColor);
	Update_CameraShake(fTimeDelta);
}

_matrix CPlayer::Get_WandPos()
{
	CWand* pWand = Get_PartObject<CWand>();

	if (pWand == nullptr)
		return _matrix();

	return pWand->Get_WorldMatrix();
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


	SAFE_RELEASE(m_pGrapInteractive);
	if (nullptr != m_pInfoInstance) {
		CInfoInstance* pInfo = m_pInfoInstance;
		m_pInfoInstance = nullptr;
		pInfo->Deregist_PlayerAlly(this);
	}

	if (nullptr != m_pCallBack_Behavior) {
		m_pCallBack_Behavior->Finalize();
	}
	if (nullptr != m_pCallBack_HitReport) {
		m_pCallBack_HitReport->Finalize();
	}
	SAFE_RELEASE(m_pCharacter_Controller);
	SAFE_RELEASE(m_pRigidBody);
	SAFE_RELEASE(m_pLightCom);
	SAFE_RELEASE(m_pStat);
	Safe_Delete(m_pCallBack_Behavior);
	Safe_Delete(m_pCallBack_HitReport);
	SAFE_RELEASE(m_pCamPosition_TopDown_FollowPart);
	SAFE_RELEASE(m_pCamPosition_TopDown_LookPart);
	SAFE_RELEASE(m_pCamPosition_ShoulderPart);
	SAFE_RELEASE(m_pEffectPool);
	SAFE_RELEASE(m_pBroomModel);
	SAFE_RELEASE(m_pBroomTransform);
	SAFE_RELEASE(m_pBroom);
}
#ifdef _DEBUG

void CPlayer::Describe_Entity()
{
	GUI::Begin("PLAYER_DESC");
	m_pCharacter_Controller->Describe_Entity();
	_float4 vMomentum = {};
	XMStoreFloat4(&vMomentum, m_pTransformCom->Get_CurrentMomentum());
	GUI::Text("%.2f %.2f %.2f %.2f ", vMomentum.x, vMomentum.y, vMomentum.z, vMomentum.w);
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


	_float RotR, RotU, RotL;
	RotR = XMVectorGetX(m_pTransformCom->Get_State(STATE::RIGHT));
	RotU = XMVectorGetY(m_pTransformCom->Get_State(STATE::UP));
	RotL = XMVectorGetZ(m_pTransformCom->Get_State(STATE::LOOK));

	float Rot3[3] = { RotR, RotU,RotL };
	GUI::DragFloat3("Rot", Rot3);


	string AnimList = m_pModelCom->Get_AnimList(m_pModelCom->Get_AnimIndex());
	GUI::Text(AnimList.c_str());

	GUI::Text("AnimTrack %.2f", m_pModelCom->Get_CurrentTrackPosition());
	GUI::Text("AnimRatio %.2f", m_pModelCom->Get_CurrentTrackProgressRatio());
	GUI::Text("AnimSpeed %.2f", m_pModelCom->Get_AnimSpeed());

	GUI::Checkbox("Render", &m_bVisible);

	GUI::Text("%d", m_iStateMask);
	GUI::Text("HP : %f, %f", m_pStat->Get_Stat().fCurrentHp, m_pStat->Get_Stat().fMaxHp);
	GUI::SameLine(); if (GUI::Button("FULL")) { m_pStat->Set_Stat(ENUM_CLASS(STAT::CURRENTHP), m_pStat->Get_Stat().fMaxHp); }
#pragma region CAMERA_SHAKE
	if (false == m_bCameraShake && GUI::Button("Shake")) {
		m_bCameraShake = true;
	}
	if (GUI::SliderFloat("m_fCameraShakeTime", &m_fCameraShakeTime, 0.125f,  0.f, "%.3f")) {
		m_vCameraShakeTimer.y = m_fCameraShakeTime;
	}
	GUI::SliderFloat("m_fCameraShakeIntense", &m_fCameraShakeIntense, 5.f, 20.f, "%.1f");
#pragma endregion

	_vector xmvInputDir = XMVectorZero();

	_vector xmvCamLook = XMVector4Normalize(XMVectorSet(m_vCameraLookDir.x, 0.f, m_vCameraLookDir.z, 0.f));
	_vector xmvCamRight = XMVector4Normalize(XMVectorSet(m_vCameraRightDir.x, 0.f, m_vCameraRightDir.z, 0.f));

	if (m_pGameInstance->Key_Pressing(DIK_W))
		xmvInputDir += xmvCamLook;
	if (m_pGameInstance->Key_Pressing(DIK_S))
		xmvInputDir -= xmvCamLook;
	if (m_pGameInstance->Key_Pressing(DIK_A))
		xmvInputDir -= xmvCamRight;
	if (m_pGameInstance->Key_Pressing(DIK_D))
		xmvInputDir += xmvCamRight;

	xmvInputDir = XMVector3Normalize(xmvInputDir);

	_float2 vInputDir = { XMVectorGetX(xmvInputDir),XMVectorGetZ(xmvInputDir) };
	_vector xmvCurLook = XMVector4Normalize(
		XMVectorSetY(m_pTransformCom->Get_State(STATE::LOOK), 0.f));
	_float2 vCurLook = { XMVectorGetX(xmvCurLook),XMVectorGetZ(xmvCurLook) };

	_float vDir = CMyTools::Get_Direction2D(vCurLook, vInputDir);
	_float degree = XMConvertToDegrees(vDir);

	GUI::Text("Angle %.2f", degree);

	m_pLightCom->Describe_Entity();

	GUI::End();
}

#endif // _DEBUG


