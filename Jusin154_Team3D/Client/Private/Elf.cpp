#include "pch.h"
#include "Elf.h"

#include "GameInstance.h"
#include "InfoInstance.h"
#include "Player.h"
#include "NPCInteraction.h"
#include "CallBack_NonPlayable_Behavior.h"
#include "CallBack_NonPlayable_HitReport.h"
#include "Layer.h"

CElf::CElf(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUnit(pDevice, pContext)
{
}

CElf::CElf(const CElf& Prototype)
	: CUnit(Prototype),
	m_pInfoInstance(CInfoInstance::GetInstance())
{
}

HRESULT CElf::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CElf::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg))) {
		return E_FAIL;
	}

	ELFDESC* pDesc = static_cast<ELFDESC*>(pArg);

	if (FAILED(Ready_Components(pArg))) {
		return E_FAIL;
	}

	Load_AnimXML("../Bin/Resources/Data/AnimList/Elf.xml");

	Load_ElfPos("../Bin/Resources/Data/ElfPos/ElfPos.xml");

	_vector vPos = XMLoadFloat4(&pDesc->vPos);
	m_pTransformCom->Set_State(STATE::POSITION, vPos);
	m_pCharacter_Controller->Set_Position(vPos);
	m_pTransformCom->Rotation(XMLoadFloat4(&pDesc->vRotQ));

	m_pModelCom->Set_AnimationIndex(m_Anims[m_iCurrentFlow].AnimIndex, false);

	m_pCallBack_Behavior->Initialize(m_pCharacter_Controller);
	m_pCallBack_HitReport->Initialize(m_pCharacter_Controller);

	return S_OK;
}

void CElf::Priority_Update(_float fTimeDelta)
{
	m_pTransformCom->RewindMomentum();
	m_iEntered -= 1;
	if (m_iEntered < 0) {
		m_iEntered = 0;
	}
	__super::Priority_Update(fTimeDelta);
}

void CElf::Update(_float fTimeDelta)
{
	MoveTo(fTimeDelta);

	Set_Anim();

	m_pModelCom->Play_Animation(fTimeDelta, m_pTransformCom);

	__super::Update(fTimeDelta);
#ifdef _DEBUG
	Describe_Entity();
#endif // _DEBUG
	{ // 세트
		m_pCallBack_HitReport->BeginFrame();
		m_pCharacter_Controller->Move(fTimeDelta);
		m_pCallBack_HitReport->Set_CurrentSlop();
	}

	m_pRigidBody->Set_Position(m_pTransformCom->Get_State(STATE::POSITION), true);

	_vector vCurrentPos = Get_WorldPostion();
	pair<CUnit*, CTransform*> pairTarget = m_pInfoInstance->Get_NearestPlayerAlly(vCurrentPos);
	if (nullptr != pairTarget.first) {
		Set_Target(*pairTarget.first, *pairTarget.second);
	}
	if (nullptr != m_pTarget && m_pTarget->isDead()) {
		SAFE_RELEASE(m_pTarget);
		m_fTargetDistance = { FLT_MAX };
	}
}

void CElf::Late_Update(_float fTimeDelta)
{
	m_pTransformCom->Set_State(STATE::POSITION, m_pCharacter_Controller->Get_FootPosition());

	m_pRigidBody->Set_Position(m_pTransformCom->Get_State(STATE::POSITION), true);

	m_pGameInstance->Add_RenderGroup(RENDER::NONBLEND, this);

	Set_Shadow(m_pGameInstance->IsIn_ShadowViewFrustum(m_pTransformCom->Get_State(STATE::POSITION), m_pTransformCom->Get_Radius()));


	__super::Late_Update(fTimeDelta);
}

HRESULT CElf::Render()
{
	if (FAILED(Bind_ShaderResources())) {
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

	return S_OK;
}

HRESULT CElf::Render_Shadow(SHADOW eType)
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

void CElf::OnRayCollision(CGameObject* pCaster, _uint iCastedOrder, _float fDistance, _float3 vCastedWorldPos)
{
	CPlayer* pPlayer = dynamic_cast<CPlayer*>(pCaster);
	if (nullptr == pPlayer) {
		return;
	}
	if (fDistance < m_fEncounterDistance) {
		m_iEntered = 4;
	}
}

_wstring CElf::Get_Name()
{
	return m_pNpcStat->Get_Stat().pNpc_Name;
}

_wstring CElf::Get_NpcName()
{
	return m_pNpcStat->Get_Stat().pName;
}

void CElf::Set_Target(CUnit& pTarget, CTransform& pTransform)
{
	SAFE_RELEASE(m_pTarget);
	m_pTarget = &pTarget;
	SAFE_ADDREF(m_pTarget);

	_vector vTargetPos = pTransform.Get_State(STATE::POSITION);
	_vector vToTargetDir = vTargetPos - Get_WorldPostion();
	XMStoreFloat4(&m_vTargetPos, vTargetPos);
	XMStoreFloat3(&m_vToTargetDir, XMVector3Normalize(vToTargetDir));
	m_fTargetDistance = XMVectorGetX(XMVector3Length(vToTargetDir));
}


HRESULT CElf::Bind_ShaderResources()
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

HRESULT CElf::Bind_ShaderParameters(_uint iMeshOrder)
{
	_bool bUseColorMixer = false;

	_uint iColorParam = { UINT_MAX };
	_float fMixerFactor = { FLT_MAX };
	_uint iColorMixerMethod = { 0 };

	switch (m_pModelCom->Get_UsingPass(iMeshOrder, m_pShaderCom))
	{
	case 23:
	{
		bUseColorMixer = true;
		iColorParam = 0x2E2E2E;
		fMixerFactor = 0.9f;
		iColorMixerMethod = 1;
	}
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

void CElf::MoveTo(_float fTimeDelta)
{
	if (m_Points.empty())
		return;

	if (m_iCurrentFlow > m_Points.size())
	{
		return;
	}

	_vector Target = XMLoadFloat4(&m_Points[m_iCurrentFlow]);

	_vector CurPos = m_pCharacter_Controller->Get_Position();
	_vector vLook = XMVector3Normalize(m_pTransformCom->Get_State(STATE::LOOK));


	_vector toTarget = Target - CurPos;
	_float fDist = XMVectorGetX(XMVector3Length(toTarget));

	if (fDist < 3.f)
	{
		if (m_iCurrentFlow + 1 < m_Points.size())
		{
			if (m_pModelCom->IsFinishedAnim() && m_Anims[m_iCurrentFlow].Trigger == ENUM_CLASS(FLOW_TRIGGER::ARRIVE_POS)) {
				m_iCurrentFlow++;
			}
		}
	}

	m_pTransformCom->LookAt_Horizontal_Lerp(Target, fTimeDelta, 5.f);
}

void CElf::Set_Anim()
{
	if (m_Anims[m_iCurrentFlow].Trigger == ENUM_CLASS(FLOW_TRIGGER::ANIM_END))
	{
		if (m_pModelCom->IsFinishedAnim())
		{
			m_iCurrentFlow++;
		}
	}

	_bool bRootBone = true;

	m_pairAnimInfo = m_Anims[m_iCurrentFlow];
	if (m_pairAnimInfo.AnimIndex == 1)
	{
		bRootBone = false;
	}
	m_pModelCom->Set_AnimationIndex(m_pairAnimInfo.AnimIndex, m_pairAnimInfo.Loop, 1.f, false, 1.f, bRootBone);
}


void CElf::Load_AnimXML(const string& path)
{
	tinyxml2::XMLDocument doc;
	if (doc.LoadFile(path.c_str()) != tinyxml2::XML_SUCCESS)
		return;

	tinyxml2::XMLElement* pDialogue = doc.FirstChildElement("Dialogue");
	if (!pDialogue)
		return;

	for (tinyxml2::XMLElement* pName = pDialogue->FirstChildElement("Name");
		pName;
		pName = pName->NextSiblingElement("Name"))
	{
		_int flow = pName->IntAttribute("Flow");

		pName->QueryIntAttribute("Flow", &flow);

		auto* pAnim = pName->FirstChildElement("Anim");
		if (!pAnim) continue;

		const char* stateStr = pAnim->Attribute("state");
		if (!stateStr)
			continue;

		DialogueAnim Anim = {};

		STATEANIM::ESTATE State = StringToStateAnim(stateStr);

		Anim.AnimIndex = pAnim->IntAttribute("index");
		Anim.Loop = pAnim->BoolAttribute("loop");

		const char* trig = pName->Attribute("Trigger");

		if (!strcmp(trig, "DIALOGUE_END"))
			Anim.Trigger = ENUM_CLASS(FLOW_TRIGGER::DIALOGUE_END);
		else if (!strcmp(trig, "ARRIVE_POS"))
			Anim.Trigger = ENUM_CLASS(FLOW_TRIGGER::ARRIVE_POS);
		else if (!strcmp(trig, "ANIM_END"))
			Anim.Trigger = ENUM_CLASS(FLOW_TRIGGER::ANIM_END);


		m_Anims[flow] = Anim;

		m_Animation[State] = { Anim.AnimIndex, Anim.Loop };
	}
}

HRESULT CElf::Load_ElfPos(const _char* pFilePath)
{
	tinyxml2::XMLDocument doc;
	if (doc.LoadFile(pFilePath) != tinyxml2::XML_SUCCESS)
		return E_FAIL;

	auto* pElfPosInfo = doc.FirstChildElement("ElfPosInfo");
	if (!pElfPosInfo) return E_FAIL;

	auto* pElfPosData = pElfPosInfo->FirstChildElement("ElfPosData");
	if (!pElfPosData) return E_FAIL;

	for (auto* pName = pElfPosData->FirstChildElement("Name");
		pName;
		pName = pName->NextSiblingElement("Name"))
	{
		int flow = -1;
		pName->QueryIntAttribute("Flow", &flow);

		auto* pPosition = pName->FirstChildElement("Position");
		if (!pPosition) continue;

		auto* pPos = pPosition->FirstChildElement("Pos");
		if (!pPos) continue;

		_float x{}, y{}, z{};
		pPos->QueryFloatAttribute("x", &x);
		pPos->QueryFloatAttribute("y", &y);
		pPos->QueryFloatAttribute("z", &z);

		m_Points[flow] = { x, y, z, 1.f };
	}

	return S_OK;
}


HRESULT CElf::Ready_Components(void* pArg)
{
	CTransform::TRANSFORM_DESC Desc = {};

	Desc.fSpeedPerSec = 10.f;
	Desc.fRotationPerSec = XMConvertToRadians(180.0f);
	Desc.fRadius = 10.f;

	if (FAILED(Add_Component<CTransform>(g_iStaticLevel, &m_pTransformCom, pArg))) {
		return E_FAIL;
	}

	/* Com_Model */
	if (FAILED(__super::Add_Asset_Component(g_iStaticLevel, TEXT("Prototype_Component_Elf_Model"),
		reinterpret_cast<CComponent**>(&m_pModelCom)))) {
		return E_FAIL;
	}

	if (FAILED(Add_Asset_Component(g_iStaticLevel, TEXT("ELF"), (CComponent**)&m_pNpcStat))) {
		return E_FAIL;
	}

	/* Com_Shader */
	if (FAILED(__super::Add_Asset_Component(g_iStaticLevel, FX_NPC_PBR_ANIM,
		reinterpret_cast<CComponent**>(&m_pShaderCom)))) {
		return E_FAIL;
	}
	{ // CCT
		CCharacter_Controller::Character_Controller_DESC Desc{};

		Desc.iSubKind = ENUM_CLASS(PXOBJECT::OLLIVANDER);
		Desc.pTransform = m_pTransformCom;
		Desc.eBodyType = ACTOR::CAPSULE;
		Desc.fContactOffset = 0.0001f;
		Desc.fMaterial = { 1.2f, 1.0f, 0.0f };
		Desc.bAutoStepping = { false };
		Desc.fStepOffset = { 0.02f };
		Desc.fRadius = 0.2f;
		Desc.fHeight = 0.3f;
		Desc.pCallback_HitReport = m_pCallBack_HitReport = CCallBack_NonPlayable_HitReport::Create();
		Desc.pCallback_Behavior = m_pCallBack_Behavior = CCallBack_NonPlayable_Behavior::Create();
		Desc.eClimbingMode = PSX::PxCapsuleClimbingMode::eEASY;
		Desc.fWalkableSlope = 45.f;
		if (FAILED(Add_Asset_Component(g_iStaticLevel, TEXT("PHYSX_CCT_CAPSULE"), (CComponent**)&m_pCharacter_Controller, &Desc))) {
			return E_FAIL;
		}
		m_pCharacter_Controller->SetGravity(true);
	}
	{ // DO
		CRigidBody_Dynamic::RIGIDBODY_DYNAMIC_DESC Desc{};
		Desc.iSubKind = ENUM_CLASS(PXOBJECT::OLLIVANDER);
		Desc.bAutoOwnerTranslation = false;
		if (FAILED(Add_Asset_Component(g_iStaticLevel, TEXT("PHYSX_NPC_HITBOX"), (CComponent**)&m_pRigidBody, &Desc))) {
			return E_FAIL;
		}
	}

	return S_OK;
}

CElf* CElf::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CElf* pInstance = new CElf(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CElf");
		SAFE_RELEASE(pInstance);
	}
	return pInstance;
}

CGameObject* CElf::Clone(void* pArg, CGameObject* pOwner)
{
	CElf* pInstance = new CElf(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CElf");
		SAFE_RELEASE(pInstance);
	}
	return pInstance;
}

void CElf::Free()
{
	__super::Free();

	SAFE_RELEASE(m_pCharacter_Controller);
	SAFE_RELEASE(m_pNpcStat);
	SAFE_RELEASE(m_pRigidBody);
	SAFE_RELEASE(m_pTarget);
	if (nullptr != m_pInfoInstance) {
		CInfoInstance* pInfo = m_pInfoInstance;
		m_pInfoInstance = nullptr;
	}
	if (nullptr != m_pCallBack_Behavior) {
		m_pCallBack_Behavior->Finalize();
	}
	if (nullptr != m_pCallBack_HitReport) {
		m_pCallBack_HitReport->Finalize();
	}
	Safe_Delete(m_pCallBack_Behavior);
	Safe_Delete(m_pCallBack_HitReport);
}
#ifdef _DEBUG

void CElf::Describe_Entity()
{
	if (m_Anims[m_iCurrentFlow].Trigger == ENUM_CLASS(FLOW_TRIGGER::DIALOGUE_END))
	{
		GUI::InputInt("Flow", &m_iCurrentFlow);
	}

	_float3 Pos;
	XMStoreFloat3(&Pos, Get_WorldPostion());

	if (GUI::DragFloat3("Pos", (_float*)&Pos, 0.01f))
	{
		m_pCharacter_Controller->Set_Position(
			XMVectorSet(Pos.x, Pos.y, Pos.z, 1.f));
	}

	_float4 vMomentum = {};
	XMStoreFloat4(&vMomentum, m_pTransformCom->Get_CurrentMomentum());
	GUI::Text("%.2f %.2f %.2f %.2f ", vMomentum.x, vMomentum.y, vMomentum.z, vMomentum.w);

	if (GUI::Button("Move To Player"))
	{
		CPlayer* pPlayer =
			m_pGameInstance->Get_Layer(NEXT_LEVEL, LAYER_PLAYER)->Get_Object<CPlayer>();
		m_pCharacter_Controller->Set_Position(pPlayer->Get_WorldPostion());

		_vector vCurPos = m_pCharacter_Controller->Get_Position();
		_float4 vPos;
		XMStoreFloat4(&vPos, vCurPos);

		char buf[128];
		sprintf_s(buf,
			"<Pos x=\"%.3f\" y=\"%.3f\" z=\"%.3f\"/>",
			vPos.x, vPos.y, vPos.z);

		GUI::SetClipboardText(buf);
	}

	GUI::Separator();

	if (GUI::TreeNode("Points"))
	{
		GUI::Text("Total Count : %d", (_int)m_Points.size());

		for (auto it = m_Points.begin(); it != m_Points.end(); )
		{
			_int index = it->first;
			_float4& p = it->second;

			GUI::PushID(index);

			GUI::Text("[%d] (%.2f, %.2f, %.2f)", index, p.x, p.y, p.z);
			GUI::SameLine();

			if (GUI::SmallButton("Move"))
			{
				m_pCharacter_Controller->Set_Position(
					XMVectorSet(p.x, p.y, p.z, 1.f));
			}

			GUI::SameLine();

			if (GUI::SmallButton("Copy"))
			{
				_vector vCurPos = m_pCharacter_Controller->Get_Position();
				XMStoreFloat4(&p, vCurPos);

				char buf[128];
				sprintf_s(buf,
					"<Pos x=\"%.3f\" y=\"%.3f\" z=\"%.3f\"/>",
					p.x, p.y, p.z);

				GUI::SetClipboardText(buf);
			}

			GUI::SameLine();

			if (GUI::SmallButton("X"))
			{
				it = m_Points.erase(it);
				GUI::PopID();
				continue;
			}

			GUI::PopID();
			++it;
		}

		if (GUI::SmallButton("Clear All"))
		{
			m_Points.clear();
		}

		GUI::TreePop();
	}

}
#endif // _DEBUG
