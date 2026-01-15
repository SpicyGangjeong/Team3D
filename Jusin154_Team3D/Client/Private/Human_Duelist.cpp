#include "pch.h"
#include "Human_Duelist.h"

#include "CallBack_Playable_Behavior.h"
#include "CallBack_Playable_HitReport.h"
#include "Character_Controller.h"
#include "EffectPool.h"
#include "Effect_Container.h"
#include "GameInstance.h"
#include "InfoInstance.h"
#include "Layer.h"
#include "Wand.h"
#include "Mesh.h"
#include "Player.h"

CHuman_Duelist::CHuman_Duelist(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CMonster(pDevice, pContext)
{
}

CHuman_Duelist::CHuman_Duelist(const CHuman_Duelist& Prototype)
	: CMonster(Prototype)
{
}

HRESULT CHuman_Duelist::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CHuman_Duelist::Initialize(void* pArg)
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

	Add_FSM();

	Load_AnimXML("../Bin/Resources/Data/AnimList/Human_Duelist.xml");

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

	m_pInfoInstance->Regist_ActiveMonster(this);
	{
		DUELISTDESC* pDesc = static_cast<DUELISTDESC*>(pArg);
		_vector vPos = XMLoadFloat4(&pDesc->vPos);
		_vector vRotQ = XMLoadFloat4(&pDesc->vRotQ);
		m_pCharacter_Controller->Set_Position(vPos);
		m_pTransformCom->Set_State(STATE::POSITION, vPos);
		m_OriginPos = pDesc->vPos;
		m_pTransformCom->Rotation(vRotQ);
	}

	m_bAI = true;

	m_pModelCom->Set_DisableRootMotionScale(true);

	return S_OK;
}

void CHuman_Duelist::Priority_Update(_float fTimeDelta)
{
	m_pTransformCom->RewindMomentum();

	__super::Priority_Update(fTimeDelta);

}

void CHuman_Duelist::Update(_float fTimeDelta)
{
	m_pFSM->Update_State(fTimeDelta);

	m_pModelCom->Play_Animation(fTimeDelta *m_fEasing, m_pTransformCom);

	Check_BattleLose();

	if (m_bBattleEnd)
	{
		m_fBattleEndTimer += fTimeDelta;
		if (m_fBattleEndTimer >= 3.f)
		{
			m_fBattleEndTimer = 0.f;
			CPlayer* pPlayer = dynamic_cast<CPlayer*>(m_pInfoInstance->Get_NearestPlayerAlly(Get_WorldPostion()).first);

			if (pPlayer == nullptr)
				return;

			pPlayer->ExitBattle();

			m_bBattleEnd = false;
		}
	}
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

	for (_uint i = 0; i < ENUM_CLASS(SKILL::END); i++)
		m_fSkillCoolTime[i] = max(0.f, m_fSkillCoolTime[i] - fTimeDelta);
}

void CHuman_Duelist::Late_Update(_float fTimeDelta)
{
	m_pTransformCom->Set_State(STATE::POSITION, m_pCharacter_Controller->Get_FootPosition());

	m_pGameInstance->Add_RenderGroup(RENDER::NONBLEND, this);
#ifdef _DEBUG
	m_pGameInstance->Add_RenderGroup(RENDER::NONLIGHT, this);
#endif // _DEBUG

	Set_Shadow(m_pGameInstance->IsIn_ShadowViewFrustum(m_pTransformCom->Get_State(STATE::POSITION), m_pTransformCom->Get_Radius()));

	__super::Late_Update(fTimeDelta);

	if (true == m_bLookAt) {
		m_pTransformCom->LookAt_Horizontal_Lerp(XMLoadFloat4(&m_vTargetPos), fTimeDelta, 3.f);
	}
	//m_pInfoInstance->Set_PlayerPos(m_pTransformCom->Get_State(STATE::POSITION));
}


HRESULT CHuman_Duelist::Render()
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

			if (true == m_bDrawOutLine) {
				m_pGameInstance->Begin_OutLine_Write(2);
			}
			if (FAILED(m_pModelCom->Render(i))) {
				return E_FAIL;
			}
			if (true == m_bDrawOutLine) {
				m_pGameInstance->End_OutLine_Write();
			}

			if (true == m_bDrawOutLine) {
				Render_OutLine();
			}
		}
#ifdef _DEBUG
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

HRESULT CHuman_Duelist::Render_Shadow(SHADOW eType)
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

		if (FAILED(m_pShaderCom->Begin(ENUM_CLASS(SHADER_PASS_NPC_PBR_ANIM::SHADOW)))) {
			return E_FAIL;
		}
		m_pModelCom->Bind_OutPut_SRV_VS(26, 0);
		m_pModelCom->Bind_OutPut_SRV_VS_Prev(27, 0);

		if (FAILED(m_pModelCom->Render(i))) {
			return E_FAIL;
		}
	}
	return S_OK;
}

HRESULT CHuman_Duelist::Render_OutLine()
{
	m_bDrawOutLine = false;
	if (FAILED(Bind_ShaderResources())) {
		return E_FAIL;
	}

	Compute_Depth();
	_float fCamFar = *m_pGameInstance->Get_CurrentCameraFar();
	_float fRatio = CMyTools::Saturate((m_fCamDepth / (fCamFar * fCamFar)));
	m_fOutLineThickness = CMyTools::Lerp_f1D(0.03f, 0.08f, fRatio);
	if (FAILED(m_pShaderCom->Bind_Matrix("g_PrevWorldMatrix", m_pTransformCom->Get_PrevWorldMatrixPtr()))) {
		return E_FAIL;
	}
	if (FAILED(m_pGameInstance->Bind_PrevMatrix(m_pShaderCom, "g_PrevViewMatrix", D3DTS::VIEW))) {
		return E_FAIL;
	}
	if (FAILED(m_pGameInstance->Bind_PrevMatrix(m_pShaderCom, "g_PrevProjMatrix", D3DTS::PROJ))) {
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Bind_RawValue("g_vOutLineColor", &m_vOutLineColor, sizeof(_float3)))) {
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fOutLineThickness", &m_fOutLineThickness, sizeof(_float)))) {
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fOutLineScale", &m_fOutLineScale, sizeof(_float)))) {
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fOutLinePower", &m_fOutLinePower, sizeof(_float)))) {
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Bind_RawValue("g_vCamPosition", m_pGameInstance->Get_CamPosition(), sizeof(_float4)))) {
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

		if (FAILED(m_pModelCom->Bind_Material(i, m_pShaderCom))) {
			return E_FAIL;
		}

		if (FAILED(m_pShaderCom->Begin(ENUM_CLASS(SHADER_PASS_NPC_PBR_ANIM::OUTLINE_READ)))) {
			return E_FAIL;
		}

		m_pModelCom->Bind_OutPut_SRV_VS(26, 0);
		m_pModelCom->Bind_OutPut_SRV_VS_Prev(27, 0);

		if (FAILED(m_pModelCom->Render(i))) {
			return E_FAIL;
		}
	}
	return S_OK;
}

_vector CHuman_Duelist::Get_LockOnPos()
{
	return m_pCharacter_Controller->Get_Position();
}


void CHuman_Duelist::OnCollision(CGameObject* pOther, void* pDesc)
{
	ON_COLLISION_INFO* CollisionDesc = static_cast<ON_COLLISION_INFO*>(pDesc);

	if (CollisionDesc) {
		Check_HitAngle(XMLoadFloat4(&CollisionDesc->vHitDir));
	}
	else {
		m_fHitDegree = -1.f;
	}

	XMStoreFloat4(&m_DamageInfo.vTarget_Pos, m_pCharacter_Controller->Get_HeadPosition());

	pair<_float, _float> damagePair = {};

	CEffect_Container* pEffect_Container = dynamic_cast<CEffect_Container*>(pOther);
	if (pEffect_Container != nullptr)
	{
		_uint iSkillType = pEffect_Container->Get_SkillType();
		damagePair = Get_Damage(m_pInfoInstance->Get_Spell_Damage(iSkillType));

		switch (iSkillType)
		{
		case ENUM_CLASS(SKILL_TYPE::JAP):
			m_eHitSpell = ENUM_CLASS(SKILL_TYPE::JAP);
			break;
		case ENUM_CLASS(SKILL_TYPE::LEVIOSO):
			m_eHitSpell = ENUM_CLASS(SKILL_TYPE::LEVIOSO);
			m_eHitState = ENUM_CLASS(HIT_STATE::AIR_LEVIOSO);
			break;
		case ENUM_CLASS(SKILL_TYPE::STUPEFY):
			m_eHitSpell = ENUM_CLASS(SKILL_TYPE::STUPEFY);
			break;
		}
	}

	m_DamageInfo.fDamage = damagePair.first;
	m_pInfoInstance->Event_CallBack(TEXT("Monster_Hit"), &m_DamageInfo);

	m_pFSM->Change_State(FSMSTATE::HIT);
}

void CHuman_Duelist::OnHit(CGameObject* pOther, CGameObject* pCaller)
{
}


HRESULT CHuman_Duelist::Ready_Components()
{
	CTransform::TRANSFORM_DESC Desc = {};

	Desc.fSpeedPerSec = 10.f;
	Desc.fRotationPerSec = XMConvertToRadians(180.0f);
	Desc.fRadius = 10.f;

	if (FAILED(Add_Component<CTransform>(g_iStaticLevel, &m_pTransformCom, &Desc))) {
		return E_FAIL;
	}

	if (FAILED(Add_Component<CFSM>(g_iStaticLevel, &m_pFSM))) {
		return E_FAIL;
	}

	m_strModelPrototypeTag = TEXT("Prototype_Component_M_Student_Model");

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


	if (FAILED(Add_Asset_Component(g_iStaticLevel, TEXT("STAT_M_STUDENT"), (CComponent**)&m_pStat))) {
		return E_FAIL;
	}

	{ // CCT
		CCharacter_Controller::Character_Controller_DESC Desc{};

		Desc.iSubKind = ENUM_CLASS(PXOBJECT::AI);
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
	}

	{ // DO
		CRigidBody_Dynamic::RIGIDBODY_DYNAMIC_DESC Desc{};
		Desc.iSubKind = ENUM_CLASS(PXOBJECT::AI);
		if (FAILED(Add_Asset_Component(g_iStaticLevel, TEXT("PHYSX_DYNAMIC_BOX"), (CComponent**)&m_pRigidBody, &Desc))) {
			return E_FAIL;
		}
		m_pGameInstance->Detach_Actor(*m_pRigidBody->Get_Actor(), NEXT_LEVEL);
	}

	return S_OK;
}

HRESULT CHuman_Duelist::Ready_Parts()
{
	CWand::WAND_DESC WandDesc{};
	WandDesc.pParentTransform = m_pTransformCom;
	WandDesc.pSocketMatrices = m_pModelCom->Get_BoneMatrixPtr("SKT_RightHand");

	if (FAILED(Add_PartObject<CWand>("Wand", g_iStaticLevel, nullptr, &WandDesc)))
	{
		return E_FAIL;
	}


	return S_OK;
}

HRESULT CHuman_Duelist::Bind_ShaderResources()
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

HRESULT CHuman_Duelist::Bind_ShaderParameters(_uint iMeshOrder)
{
	_bool bUseColorMixer = false;

	_uint iColorParam = { UINT_MAX };
	_float fMixerFactor = { FLT_MAX };
	_uint iColorMixerMethod = { 0 };

	switch (CPlayer::PLAYER_MESH_ORDER(iMeshOrder))
	{
	case CPlayer::PLAYER_MESH_ORDER::HAIR_MAIN:
	case CPlayer::PLAYER_MESH_ORDER::EYELASH:
		bUseColorMixer = true;
		iColorParam = 0x2E2E2E;
		fMixerFactor = 0.9f;
		iColorMixerMethod = 1;
		break;
	case CPlayer::PLAYER_MESH_ORDER::CLOTH:
		bUseColorMixer = true;
		iColorParam = 0x292557;
		fMixerFactor = 0.5f;
		iColorMixerMethod = 1;
		break;
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

void CHuman_Duelist::Check_BattleLose()
{
	if (!m_bBattle)
		return;

	_float vPosY = XMVectorGetY(m_pCharacter_Controller->Get_Position());

	if (m_OriginPos.y > vPosY)
	{
		if (true == m_pCharacter_Controller->IsOnGround())
		{
			_int iRand = m_pGameInstance->Real_Random_Int(0, 2);
			switch (iRand)
			{
			case 0:
				m_pGameInstance->Sound_Play(SOUND::SD_KIND::TROLL_14, SD_CHANNEL_GROUP::EFFECT, false, 0.7f);
				break;
			case 1:
				m_pGameInstance->Sound_Play(SOUND::SD_KIND::TROLL_15, SD_CHANNEL_GROUP::EFFECT, false, 0.7f);
				break;
			case 2:
				m_pGameInstance->Sound_Play(SOUND::SD_KIND::TROLL_20, SD_CHANNEL_GROUP::EFFECT, false, 0.7f);
				break;
			}
			m_pEffectPool->Use_Skill(SKILL_TYPE::BOX_SPLESH, this);
			m_bBattle = false;
			m_bBattleEnd = true;
		}
	}
}


_matrix CHuman_Duelist::Get_WandPos()
{
	CWand* pWand = Get_PartObject<CWand>();

	if (pWand == nullptr)
		return _matrix();

	return pWand->Get_WorldMatrix();
}

CHuman_Duelist* CHuman_Duelist::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CHuman_Duelist* pInstance = new CHuman_Duelist(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CHuman_Duelist");
		SAFE_RELEASE(pInstance);
	}
	return pInstance;
}

CGameObject* CHuman_Duelist::Clone(void* pArg, CGameObject* pOwner)
{
	CHuman_Duelist* pInstance = new CHuman_Duelist(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CHuman_Duelist");
		SAFE_RELEASE(pInstance);
	}
	return pInstance;
}

void CHuman_Duelist::Free()
{
	__super::Free();


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
	SAFE_RELEASE(m_pEffectPool);
}
#ifdef _DEBUG

void CHuman_Duelist::Describe_Entity()
{


	GUI::Begin("UNIT", 0, IMGUI_GLOBAL_BEGIN_FLAG);
	GUI::PushItemWidth(IMGUI_GLOBAL_ITEM_WIDTH);
	if (GUI::CollapsingHeader("HUMAN_DUELIST_DESC")) {
		if (true == GUI::Button("ShaderRefresh")) {
			m_pShaderCom->Shader_Refresh();
		}
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


		if(GUI::Button("Reset Pos"))
		{
			m_pCharacter_Controller->Set_Position(XMVectorSet(1007.f, 6.f, 1016.f, 1.f));
		}

		m_pLightCom->Describe_Entity();
	}
	GUI::End();
}

#endif // _DEBUG
