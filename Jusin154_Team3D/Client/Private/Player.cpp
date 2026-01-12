#include "pch.h"
#include "Player.h"

#include "Broom.h"
#include "BroomRaceManager.h"
#include "CallBack_Playable_Behavior.h"
#include "CallBack_Playable_HitReport.h"
#include "CamPosition_Shoulder.h"
#include "Character_Controller.h"
#include "EffectPool.h"
#include "GameInstance.h"
#include "InfoInstance.h"
#include "Item_Potion.h"
#include "Layer.h"
#include "MapElement_Interactable.h"
#include "Monster.h"
#include "TimeSocket.h"
#include "PlayerRobe.h"
#include "RaceRing.h"
#include "Wand.h"
#include "Mesh.h"
#include "Effect_Container.h"
#include "ThestralCarriage.h"
#include "MapElement_Chest.h"

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

#if 진우
	m_isDebugMode = true; // 디버그 무적 모드
#endif
#if 나
	m_isDebugMode = true; // 디버그 무적 모드
#endif

#endif // _DEBUG

	m_pBroomModel = m_pBroom->Get_Component<CModel>();
	m_pBroomTransform = m_pBroom->Get_Component<CTransform>();
	SAFE_ADDREF(m_pBroom);
	SAFE_ADDREF(m_pBroomModel);
	SAFE_ADDREF(m_pBroomTransform);

	Add_FSM();

	Load_AnimXML("../Bin/Resources/Data/AnimList/Player.xml");

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

	m_pCarriage = m_pGameInstance->Get_Layer(NEXT_LEVEL, TEXT("Layer_Item"))->Get_Object<CThestralCarriage>();
	SAFE_ADDREF(m_pCarriage);

	m_pInfoInstance->Regist_PlayerAlly(this);
	m_pInfoInstance->Set_Damage(m_pStat->Get_Stat().fDamage);
	{
		PLAYERDESC* pDesc = static_cast<PLAYERDESC*>(pArg);
		_vector vPos = XMLoadFloat4(&pDesc->vPos);
		_vector vRotQ = XMLoadFloat4(&pDesc->vRotQ);
		m_pCharacter_Controller->Set_Position(vPos);
		m_pTransformCom->Set_State(STATE::POSITION, vPos);
		m_pTransformCom->Rotation(vRotQ);

		m_pBroomRaceManager = pDesc->pBroomRaceManager;
	}

#ifdef _DEBUG

	m_BasicEffect = make_unique<BasicEffect>(m_pDevice);
	m_BasicEffect->SetVertexColorEnabled(true);
	m_BasicEffect->SetView(m_pGameInstance->Get_Transform_Matrix(D3DTS::VIEW));
	m_BasicEffect->SetProjection(m_pGameInstance->Get_Transform_Matrix(D3DTS::PROJ));

	m_Batch = make_unique<PrimitiveBatch<VertexPositionColor>>(m_pContext);
#endif // _DEBUG

	// UI 연동 추가
	m_pInfoInstance->Add_Event(TEXT("UseSpell"), [this](void* p) {this->Get_Spell(*reinterpret_cast<_int*>(p)); });
	m_pInfoInstance->Add_Event(TEXT("Player_CanvasChange"), [this](void* p) {this->Get_UIState(*reinterpret_cast<_int*>(p)); });
	m_pInfoInstance->Add_Event(TEXT("NpcInteraction"), [this](void* p) {this->Set_Interaction(*reinterpret_cast<_bool*>(p)); });

	m_bAI = false;

	XMLoadFloat4x4(m_pBroomModel->Get_BoneMatrixPtr("broomSocket"));
	m_fRayDistance = 5.f;
	m_pModelCom->Set_DisableRootMotionScale(true);

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
	Update_RaycastElements();

	m_pFSM->Update_State(fTimeDelta);

	Play_SpellHitAnim();

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

	CheckMouseInput();

	if (m_pGameInstance->Key_Down(DIK_F))
	{
		if (m_bNpcInteraction == true)
		{
			m_bCurrentInteraction = true;
			m_pInfoInstance->Event_CallBack(TEXT("NpcInteract"), &m_bNpcInteraction);
		}
	}
	if (m_bGuarding) {
		m_fParryTimer += fTimeDelta;
	}

	m_pInfoInstance->Set_PlayerPos(m_pTransformCom->Get_State(STATE::POSITION));
}

void CPlayer::Late_Update(_float fTimeDelta)
{
	m_pTransformCom->Set_State(STATE::POSITION, m_pCharacter_Controller->Get_FootPosition());

	m_pGameInstance->Add_RenderGroup(RENDER::NONBLEND, this);
#ifdef _DEBUG
	m_pGameInstance->Add_RenderGroup(RENDER::NONLIGHT, this);
#endif // _DEBUG

	Set_Shadow(m_pGameInstance->IsIn_ShadowViewFrustum(m_pTransformCom->Get_State(STATE::POSITION), m_pTransformCom->Get_Radius()));

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
		m_pTransformCom->LookAt_Horizontal_Lerp(m_LockOnInfo.pUnit->Get_WorldPostion(), fTimeDelta, 5.f);
	}

	Player_PixRot();
}


HRESULT CPlayer::Render()
{
	if (!m_bVisible) {
		return S_OK;
	}
	if (FAILED(Bind_ShaderResources())) {
		return E_FAIL;
	}
	_float fIntensity = 0.f;
	m_pShaderCom->Bind_RawValue("g_fMBIntensity", &fIntensity, sizeof(_float));
	RENDER eType = m_pGameInstance->Get_CurrentRenderPass();
	if (RENDER::NONBLEND == eType) {
		_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

		for (_uint i = 0; i < iNumMeshes; i++)
		{
			if (FAILED(m_pModelCom->Bind_Material(i, m_pShaderCom))) {
				return E_FAIL;
			}
			if (FAILED(m_pShaderCom->Bind_Matrices(
				"g_OffsetMatrix", m_pModelCom->Get_OffsetMatrix(i).data(),
				(_int)m_pModelCom->Get_OffsetMatrix(i).size())))
			{
				return E_FAIL;
			}
			if (FAILED(Bind_ShaderParameters(i))) {
				return E_FAIL;
			}
			if (FAILED(m_pModelCom->Begin(i, m_pShaderCom))) {
				return E_FAIL;
			}

			m_pModelCom->Bind_OutPut_SRV_VS(26, 0);
			m_pModelCom->Bind_OutPut_SRV_VS_Prev(27, 0);

			if (FAILED(m_pModelCom->Render(i))) {
				return E_FAIL;
			}
		}
#ifdef _DEBUG
#ifdef 기무리
		Render_CameraCoordinateSystem();
#endif // 기무리
		//m_pCharacter_Controller->Render();
#endif // _DEBUG

	}
#ifdef _DEBUG
	if (RENDER::NONLIGHT == eType) {
		//m_pRigidBody->Render();
	}
#endif

	return S_OK;
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

HRESULT CPlayer::Update_RaycastElements()
{
	CGameObject* pFoundNPC = nullptr;

	if (E_FAIL == m_pGameInstance->IsBinded_Camera(CAMERA_SHOULDER)) {
		m_iRayHitCount = 0;
		return E_FAIL;
	}
	_vector vCameraPos = m_pGameInstance->Get_CamXMPosition();
	_vector vCameraDir = m_pGameInstance->Get_CameraLook();
	vector<PSX::PxRaycastHit> m_vRayHits = {};
	m_vRayHits.resize(12);
	_bool bHit = m_pGameInstance->RayCast(vCameraPos, vCameraDir, m_fRayDistance, m_vRayHits.data(), (_uint)m_vRayHits.size(), m_iRayHitCount);
	if (true == bHit)
	{
		NPCINTERACTIONINFO Info{};

		CMyTools::SortHitsByDistance(m_vRayHits);
		for (_uint i = 0; i < m_iRayHitCount; ++i)
		{
			if (nullptr == m_vRayHits[i].actor->userData)
			{
				continue;
			}
			PHYSX_USERDATA* pData = (PHYSX_USERDATA*)m_vRayHits[i].actor->userData;

			if (pData->eKind != PHYSX_KIND::BODY_DYNAMIC)
			{
				continue;
			}

			pData->pBody->OnRayCollision(this, i, m_vRayHits[i].distance, _float3(m_vRayHits[i].position.x, m_vRayHits[i].position.y, m_vRayHits[i].position.z));

			CUnit* Npc = dynamic_cast<CUnit*>(pData->pOwner);

			if (!Npc)
			{
				continue;
			}

			if (!Npc->Get_Npc())
			{
				continue;
			}

			pFoundNPC = Npc;
			break;
		}

		if (pFoundNPC)
		{
			if (m_pCurrentNpcInteraction != pFoundNPC)
			{
				m_pCurrentNpcInteraction = pFoundNPC;

				Info.pOwner = pFoundNPC;
				Info.pNPCName = static_cast<CUnit*>(pFoundNPC)->Get_Name();
				Info.pName = static_cast<CUnit*>(pFoundNPC)->Get_NpcName();
				_float4 Pos{};
				XMStoreFloat4(&Pos, static_cast<CUnit*>(pFoundNPC)->Get_WorldPostion());
				Info.fNPCPosition = Pos;
				Info.iTextID = static_cast<CUnit*>(pFoundNPC)->Get_TextID();
				m_bNpcInteraction = true;
				m_pInfoInstance->Event_CallBack(TEXT("NPCInteractionOn"), &Info);
			}
		}

		else
		{
			if (m_pCurrentNpcInteraction && !m_bCurrentInteraction)
			{
				m_bNpcInteraction = false;
				m_pCurrentNpcInteraction = nullptr;
				m_pInfoInstance->Event_CallBack(TEXT("NPCInteractionOff"));
			}
		}
	}

	else
	{
		if (m_pCurrentNpcInteraction && !m_bCurrentInteraction)
		{
			m_bNpcInteraction = false;
			m_pCurrentNpcInteraction = nullptr;
			m_pInfoInstance->Event_CallBack(TEXT("NPCInteractionOff"));
		}
	}
	return S_OK;
}

void CPlayer::Set_Interaction(_bool bInteraction)
{
	m_bCurrentInteraction = bInteraction;
}

HRESULT CPlayer::Render_Shadow(SHADOW eType)
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
		if (FAILED(m_pShaderCom->Bind_Matrices("g_OffsetMatrix",
			m_pModelCom->Get_OffsetMatrix(i).data(),
			(_int)m_pModelCom->Get_OffsetMatrix(i).size())))
		{
			return E_FAIL;
		}
		if (FAILED(Bind_ShaderParameters(i))) {
			return E_FAIL;
		}
		if (i == ENUM_CLASS(PLAYER_MESH_ORDER::ROBE_CLOTH)) {
			if (FAILED(m_pShaderCom->Begin(ENUM_CLASS(SHADER_PASS_NPC_PBR_ANIM::SHADOW_LEGACY)))) {
				return E_FAIL;
			}

		}
		else {
			if (FAILED(m_pShaderCom->Begin(ENUM_CLASS(SHADER_PASS_NPC_PBR_ANIM::SHADOW)))) {
				return E_FAIL;
			}
		}
		m_pModelCom->Bind_OutPut_SRV_VS(26, 0);
		m_pModelCom->Bind_OutPut_SRV_VS_Prev(27, 0);

		if (FAILED(m_pModelCom->Render(i))) {
			return E_FAIL;
		}
	}
	return S_OK;
}
void CPlayer::OnCollision(CGameObject* pOther, void* pDesc)
{
	pair<_uint, _bool> pairAnimInfo;
	_int iCurrAnim = m_pModelCom->Get_AnimIndex();

	if (m_pFSM->IsEnable(FSMSTATE::DODGE | FSMSTATE::BLINK) ||
		iCurrAnim == m_Animation[STATEANIM::AVADA_KEDAVRA].first ||
		iCurrAnim == m_Animation[STATEANIM::ANCIENT_LIGHTNING].first ||
		iCurrAnim == m_Animation[STATEANIM::ANCIENT_THROW].first)
		return;

	if (m_bShield) {
		if (IsParryWindow())
		{
			m_pFSM->Change_State(FSMSTATE::PARRY);
			m_fParryTimer = 0.f;
			return;
		}
		else {
			m_pFSM->Change_State(FSMSTATE::BLOCK);
			return;
		}
	}
	

#ifdef _DEBUG
	if (m_isDebugMode == true)
		return;
#endif

	ON_COLLISION_INFO* CollisionDesc = static_cast<ON_COLLISION_INFO*>(pDesc);
	if (CollisionDesc) {
		Check_HitAngle(XMLoadFloat4(&CollisionDesc->vHitDir));
		m_eHitType = CollisionDesc->eHitType;
		m_pStat->Get_Damage(CollisionDesc->fDamage);
	}
	else {
		m_fHitDegree = -1.f;
	}

	CEffect_Container* pEffect_Container = dynamic_cast<CEffect_Container*>(pOther);
	if (pEffect_Container != nullptr)
	{
		_uint iSkillType = pEffect_Container->Get_SkillType();
		switch (iSkillType)
		{
		case ENUM_CLASS(SKILL_TYPE::DUELIST_JAP):
			m_eHitSpell = ENUM_CLASS(SKILL_TYPE::JAP);
			break;
		case ENUM_CLASS(SKILL_TYPE::DUELIST_LEVIOSO):
			m_eHitSpell = ENUM_CLASS(SKILL_TYPE::LEVIOSO);
			break;
		}
	}

	if (m_eHitType != ENUM_CLASS(HIT_TYPE::HIT_NONE))
		m_pFSM->Change_State(FSMSTATE::HIT);
}
void CPlayer::OnHit(CGameObject* pOther, CGameObject* pCaller)
{
}

void CPlayer::Trigger(CTimeSocket& Socket)
{
	SOCKETCONTENTS* pContents = &Socket.m_Contents;
	switch (pContents->eTypeFunc)
	{
	case TIMESOCKET_FUNC::TRANSLATION:
	{
		_vector vNewPos = XMVectorSetW(XMLoadFloat3((_float3*)&pContents->pxTransform.p), 1.f);
		m_pTransformCom->Set_State(STATE::POSITION, vNewPos);
		m_pCharacter_Controller->Set_Position(vNewPos);
		m_pTransformCom->RotationQ(pContents->pxTransform.q);
		m_pTransformCom->RewindMomentum();
	} break;
	case TIMESOCKET_FUNC::TRANSLATION_LERP:
	{

	} break;
	case TIMESOCKET_FUNC::SET_ANIMSTATE:
	{
	} break;
	case TIMESOCKET_FUNC::SET_FSMSTATE:
	{
		if (pContents->vFlags.b[0]) {
			m_pFSM->Change_State(FSMSTATE::CUTSCENE);
		}
		else if (pContents->vFlags.b[1]) {
			m_pFSM->Change_State(FSMSTATE::IDLE);
		}
	} break;
	case TIMESOCKET_FUNC::BIND_SOCKET_MATRIX:
	{
		
	} break;
	case TIMESOCKET_FUNC::UNBIND_SOCKET_MATRIX:
	{

	} break;
	default:
		break;
	}
}

void CPlayer::Start_CameraShake(_float fTime, _float fIntense)
{
	m_vCameraShakeTimer.x = 0.f;
	m_vCameraShakeTimer.y = fTime;
	m_fCameraShakeIntense = fIntense;
	m_bCameraShake = true;
}


void CPlayer::Set_RaceRing(CRaceRing* pRaceRing)
{
	if (m_pRaceRing != pRaceRing)
	{
		_vector TargetPos = pRaceRing->Get_WorldPostion();
		m_pInfoInstance->Event_CallBack(TEXT("BroomTargetGate"), &TargetPos);
	}
	SAFE_RELEASE(m_pRaceRing);
	m_pRaceRing = pRaceRing;
	SAFE_ADDREF(m_pRaceRing);
}

void CPlayer::Set_RaceInfo()
{
	if (m_pBroomRaceManager)
	{
		CBroomRaceManager::RacerInfo Info;

		Info.pRacer = this;
		Info.curRing = 0;
		XMStoreFloat4(&Info.prevPos, Get_WorldPostion());

		m_pBroomRaceManager->Push_BroomRacer(Info);
	}
}

_bool CPlayer::IsParryWindow()
{
	return m_fParryTimer <= 0.15f;
}


HRESULT CPlayer::Ready_Components()
{
	CTransform::TRANSFORM_DESC Desc = {};

	Desc.fSpeedPerSec = 10.f;
	Desc.fRotationPerSec = XMConvertToRadians(180.0f);
	Desc.fRadius = 10.f;

	if (FAILED(__super::Ready_Components(&Desc))) {
		return E_FAIL;
	}

	m_strModelPrototypeTag = TEXT("Prototype_Component_Playable_Model");

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
	SAFE_ADDREF(m_pStat);
	
	m_Components.push_back(m_pStat);
	SAFE_ADDREF(m_pStat);

	{ // CCT
		CCharacter_Controller::Character_Controller_DESC Desc{};

		Desc.iSubKind = ENUM_CLASS(PXOBJECT::PLAYER);
		Desc.pTransform = m_pTransformCom;
		Desc.eBodyType = ACTOR::CAPSULE;
		Desc.fContactOffset = 0.001f;
		Desc.fMaterial = { 1.2f, 1.0f, 0.0f };
		Desc.bAutoStepping = { false };
		Desc.fStepOffset = { 0.02f };
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
		m_pGameInstance->Detach_Actor(*m_pRigidBody->Get_Actor(), NEXT_LEVEL);
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

	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer<CBroom>(g_iStaticLevel, NEXT_LEVEL, LAYER_ITEM, nullptr, this, &m_pBroom))) {
		return E_FAIL;
	}

	//m_pModelCom->Play_Animation()
	//XMLoadFloat4x4(m_pModelCom->Get_BoneMatrixPtr("broomSocket"));
#ifdef 기무리

	//{
	//	CPlayerRobe::PlayerRobe_DESC Desc{};
	//	Desc.pModel = m_pModelCom;
	//	Desc.pParentTransform = m_pTransformCom;
	//	Desc.pSocketMatrix = m_pModelCom->Get_BoneMatrixPtr("Hips_Cloth");
	//	if (FAILED(Add_PartObject<CPlayerRobe>("RobePart", g_iStaticLevel, &m_pRobePart, &Desc))) {
	//		assert(false);
	//	}
	//}

#endif // 기무리


	return S_OK;
}

HRESULT CPlayer::Bind_ShaderResources()
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

HRESULT CPlayer::Bind_ShaderParameters(_uint iMeshOrder)
{
	_bool bUseColorMixer = false;

	_uint iColorParam = { UINT_MAX };
	_float fMixerFactor = { FLT_MAX };
	_uint iColorMixerMethod = { 0 };

	switch (PLAYER_MESH_ORDER(iMeshOrder))
	{
	case PLAYER_MESH_ORDER::HAIR_MAIN:
	case PLAYER_MESH_ORDER::HEAD_EYELASH:
	case PLAYER_MESH_ORDER::HAIR_SUB:
		bUseColorMixer = true;
		iColorParam = 0x2E2E2E;
		fMixerFactor = 0.9f;
		iColorMixerMethod = 1;
		break;
	case PLAYER_MESH_ORDER::LOWER:
		bUseColorMixer = true;
		iColorParam = 0x292557;
		fMixerFactor = 0.5f;
		iColorMixerMethod = 1;
		break;
	case PLAYER_MESH_ORDER::SHOES:
		bUseColorMixer = true;
		iColorParam = 0x614242;
		fMixerFactor = 0.5f;
		iColorMixerMethod = 1;
		break;
	case PLAYER_MESH_ORDER::UPPER:
		bUseColorMixer = true;
		iColorParam = 0xBFAC29;
		fMixerFactor = 0.658333f;
		iColorMixerMethod = 1;
		break;
//#ifdef 기무리
//
//	case PLAYER_MESH_ORDER::ROBE_CLOTH:
//	{
//		CMesh* pMesh = m_pModelCom->Get_Mesh(ENUM_CLASS(PLAYER_MESH_ORDER::ROBE_CLOTH));
//		_uint MeshBoneCount = pMesh->Get_NumBone();
//
//		for (_uint i = 0; i < MeshBoneCount; ++i)
//		{
//			XMStoreFloat4x4(&SkinMatrices[i], XMMatrixIdentity());
//		}
//
//		_uint temp = 0;
//		vector<_uint> globalMask = m_pModelCom->Get_BoneMask(ENUM_CLASS(BLEND_BONE::HIPS_CLOTH));
//		vector<_int> boneIndices = pMesh->Get_BoneIndices();
//
//		for (_uint i = 0; i < MeshBoneCount; ++i)
//		{
//			_uint global = boneIndices[i];
//			if (global == 38)
//				continue;
//			if (globalMask[global] == 1)
//			{
//				SkinMatrices[i] = m_pRobePart->Get_RobeJointAnchorMatrix(temp++);
//			}
//		}
//
//		GUI::DragFloat("TempWeight", &m_fTempWeight, 0.01f);
//
//		if (FAILED(m_pShaderCom->Bind_RawValue("g_TempWeight", &m_fTempWeight, sizeof(_float)))) {
//			return E_FAIL;
//		}
//
//
//		if (FAILED(m_pShaderCom->Bind_Matrices(
//			"g_BoneMatrices",
//			SkinMatrices.data(),
//			(_int)SkinMatrices.size()
//		)))
//		{
//			return E_FAIL;
//		}
//	}
//	break;
//#endif // _DEBUG

	default:
		break;
	}
	if (true == bUseColorMixer) {
		if (FAILED(m_pShaderCom->Bind_RawValue("g_iPackedBlendColor", &iColorParam, sizeof(_uint)))) {
			return E_FAIL;
		}

		if (FAILED(m_pShaderCom->Bind_RawValue("g_fMixerFactor", &fMixerFactor, sizeof(_float)))) {
			return E_FAIL;
		}

		if (FAILED(m_pShaderCom->Bind_RawValue("g_iColorMixerMethod", &iColorMixerMethod, sizeof(_uint)))) {
			return E_FAIL;
		}
	}
	return S_OK;
}

void CPlayer::CheckMouseInput()
{
	if (m_pGameInstance->Mouse_Down(DIM_RBUTTON)) {
		m_pInfoInstance->Mouse_Input(ENUM_CLASS(KEYINPUT::DIM_RBUTTON_DOWN));
	}
	if (m_pGameInstance->Mouse_Up(DIM_RBUTTON))
	{
		m_pInfoInstance->Mouse_Input(ENUM_CLASS(KEYINPUT::DIM_RBUTTON_UP));
		m_bAim = false;
	}
	if (m_pGameInstance->Key_Up(DIK_U))
	{
		m_pFSM->Change_State(FSMSTATE::CUTSCENE);
	}
}

void CPlayer::ReLockOnTarget()
{
	m_pInfoInstance->Get_LockOnInfo(m_LockOnInfo);
	if (nullptr != m_LockOnInfo.pUnit) {
		if (true == m_LockOnInfo.pUnit->isDead()) {
			m_LockOnInfo.pUnit = nullptr;
		}
	}
	if (nullptr != m_LockOnInfo.pInteractive) {
		if (true == m_LockOnInfo.pInteractive->isDead()) {
			m_LockOnInfo.pInteractive = nullptr;
		}
	}
	if (nullptr != m_LockOnInfo.pEffect) {
		if (false == m_LockOnInfo.pEffect->Get_Visible()) {
			m_LockOnInfo.pEffect = nullptr;
		}
	}

	//m_pLockOnMonster->Get_State
}

void CPlayer::SetGravity()
{
	PSX::PxControllerCollisionFlags eCollisionFlags = m_pCharacter_Controller->Get_CollisionFlags();
	eCollisionFlags;
	if (false == eCollisionFlags.isSet(PSX::PxControllerCollisionFlag::Enum::eCOLLISION_DOWN)
		&& false == eCollisionFlags.isSet(PSX::PxControllerCollisionFlag::Enum::eCOLLISION_SIDES)) {
		if (false == m_pFSM->IsEnable(FSMSTATE::JUMP|FSMSTATE::CUTSCENE) && m_eHitType != ENUM_CLASS(HIT_TYPE::HIT_HEAVY) && m_fAirTime == 0.f) { // 벽에 닿지 않았는데 점프 중이 아닐 땐 중력 on
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

void CPlayer::Player_PixRot()
{
	_int iCurrAnim = m_pModelCom->Get_AnimIndex();
	if (iCurrAnim == m_Animation[STATEANIM::AVADA_KEDAVRA].first ||
		iCurrAnim == m_Animation[STATEANIM::BROOM_DISMOUNT].first||
		iCurrAnim == m_Animation[STATEANIM::IDLE].first)
	{
		_vector vUp = XMVectorSet(0.f, 1.f, 0.f, 0.f);

		_vector vLook = m_pTransformCom->Get_State(STATE::LOOK);
		vLook = XMVectorSetY(vLook, 0.f);

		if (XMVector3LengthSq(vLook).m128_f32[0] < 1e-6f)
			vLook = XMVectorSet(0.f, 0.f, 1.f, 0.f);

		vLook = XMVector3Normalize(vLook);

		_vector vRight = XMVector3Normalize(XMVector3Cross(vUp, vLook));
		vLook = XMVector3Cross(vRight, vUp);

		m_pTransformCom->Set_State(STATE::RIGHT, vRight);
		m_pTransformCom->Set_State(STATE::UP, vUp);
		m_pTransformCom->Set_State(STATE::LOOK, vLook);
	}
}

void CPlayer::Find_HiddenObjects()
{
	CLayer* pLayer = m_pGameInstance->Get_Layer(NEXT_LEVEL, LAYER_HIDDEN);

	if (nullptr == pLayer)
		return;

	const list<class CGameObject*>* pHiddenObjects = pLayer->Get_Objects();

	for (auto& pObject : *pHiddenObjects)
	{
		CMapElement_Chest* pChest = dynamic_cast<CMapElement_Chest*>(pObject);
		if (nullptr != pChest)
			if (pChest->IsScannable(m_pTransformCom->Get_State(STATE::POSITION)))
				return;
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

	//SAFE_RELEASE(m_pRobePart);
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
	SAFE_RELEASE(m_pCamPosition_ShoulderPart);
	SAFE_RELEASE(m_pEffectPool);
	SAFE_RELEASE(m_pBroomModel);
	SAFE_RELEASE(m_pBroomTransform);
	SAFE_RELEASE(m_pBroom);
	SAFE_RELEASE(m_pRaceRing);
	SAFE_RELEASE(m_pCarriage);
}
#ifdef _DEBUG

void CPlayer::Render_CameraCoordinateSystem()
{
	m_Batch->Begin();

	const _float fArrowLength = 2.0f;
	_vector xmvLook = XMVector4Normalize(XMVectorSetY(m_pTransformCom->Get_State(STATE::LOOK), 0.f));
	_float2 vLook = { XMVectorGetX(xmvLook), XMVectorGetZ(xmvLook) };


	GUI::Begin("CAMERA", 0, IMGUI_GLOBAL_BEGIN_FLAG);
	GUI::PushItemWidth(IMGUI_GLOBAL_ITEM_WIDTH);
	if (GUI::CollapsingHeader("Player_CAM_COOORD")) {

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
		//W CMyTools::Get_Direction2D(vLook, { m_vCameraLookDir.x ,		m_vCameraLookDir.z })
		//A CMyTools::Get_Direction2D(vLook, { -m_vCameraRightDir.x , -	m_vCameraRightDir.z })
		//S CMyTools::Get_Direction2D(vLook, { m_vCameraRightDir.x ,	m_vCameraRightDir.z })
		//D CMyTools::Get_Direction2D(vLook, { -m_vCameraLookDir.x , -	m_vCameraLookDir.z })
	}
	GUI::End();
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
void CPlayer::Describe_Entity()
{
	GUI::Begin("UNIT", 0, IMGUI_GLOBAL_BEGIN_FLAG);
	GUI::PushItemWidth(IMGUI_GLOBAL_ITEM_WIDTH);
	if (GUI::CollapsingHeader("PLAYER_DESC")) {
		if (true == GUI::Button("ShaderRefresh")) {
			m_pShaderCom->Shader_Refresh();
		}
		m_pCharacter_Controller->Describe_Entity();
		GUI::Checkbox("Battle", &m_bDuel_ZOnlyMove);
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
		if (GUI::SliderFloat("m_fCameraShakeTime", &m_fCameraShakeTime, 0.125f, 0.f, "%.3f")) {
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

		_vector vRight = m_pTransformCom->Get_State(STATE::RIGHT);
		_vector vUp = m_pTransformCom->Get_State(STATE::UP);
		_vector vLook = m_pTransformCom->Get_State(STATE::LOOK);

		auto dotRU = XMVectorGetX(XMVector3Dot(vRight, vUp));
		auto dotUL = XMVectorGetX(XMVector3Dot(vUp, vLook));
		auto dotLR = XMVectorGetX(XMVector3Dot(vLook, vRight));

		GUI::Text("dot RU %.4f | UL %.4f | LR %.4f\n", dotRU, dotUL, dotLR);



		m_pLightCom->Describe_Entity();
		if (GUI::TreeNode("PLAYER_ANIMLIST")) {
			m_pModelCom->Describe_Entity();
			GUI::TreePop();
		}
	}
	GUI::End();
}

#endif // _DEBUG


