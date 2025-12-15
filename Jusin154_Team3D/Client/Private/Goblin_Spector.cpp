#include "pch.h"
#include "Goblin_Spector.h"

#include "GameInstance.h"
#include "InfoInstance.h"
#include "Goblin.h"
#include "Effect_Container.h"
#include "Goblin_BattleAxe.h"

#include "EffectParts.h"
#include "TrailObject.h"


CGoblin_Spector::CGoblin_Spector(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CMonster(pDevice, pContext)
{
}

CGoblin_Spector::CGoblin_Spector(const CGoblin_Spector& Prototype)
	: CMonster(Prototype)
{
}

HRESULT CGoblin_Spector::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CGoblin_Spector::Initialize(void* pArg)
{
	GOBLIN_SPECTOR_DESC* pDesc = static_cast<GOBLIN_SPECTOR_DESC*> (pArg);
	m_pParentTransformCom = pDesc->pParentTransform;
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	if (FAILED(Ready_Parts()))
		return E_FAIL;

	m_pInfoInstance->Deregist_ActiveMonster(this);

	m_pGoblin = dynamic_cast<CGoblin*>(m_pOwner);
	SAFE_ADDREF(m_pGoblin);

	m_vOriginScale = m_pTransformCom->Get_Scale();

	m_pLeftHand_BoneMat = m_pModelCom->Get_BoneMatrixPtr("SKT_LeftHand");
	m_pRightHand_BoneMat = m_pModelCom->Get_BoneMatrixPtr("SKT_RightHand");

	m_pModelCom->Set_AnimationIndex(5);
	m_pModelCom->Play_Animation(0, m_pTransformCom);


	m_bVisible = false;

	return S_OK;
}

void CGoblin_Spector::Priority_Update(_float fTimeDelta)
{
	__super::Priority_Update(fTimeDelta);
}

void CGoblin_Spector::Update(_float fTimeDelta)
{
	__super::Update(fTimeDelta);

	m_Offset = _float3(0.f, 0.3f, 0.f);

	_matrix Offset = XMMatrixTranslation(m_Offset.x,
		m_Offset.y, m_Offset.z);

	m_pTransformCom->Set_WorldMatrix(Offset * XMLoadFloat4x4(m_pParentTransformCom->Get_WorldMatrixPtr()));

	if (m_bVisible)
	{
		if (m_vScale.x <= 2.f)
		{
			m_vScale.x += fTimeDelta;
			m_vScale.y += fTimeDelta;
			m_vScale.z += fTimeDelta;
		}
		m_pTransformCom->Set_Scale(m_vScale);
		m_pModelCom->Set_AnimationIndex(m_pGoblin->Get_Component<CModel>()->Get_AnimIndex());
		m_pModelCom->Set_CurrentTrackPosition(m_pGoblin->Get_Component<CModel>()->Get_CurrentTrackPosition());
		Get_PartObject<CGoblin_BattleAxe>()->Set_Visible(true);
		m_pModelCom->Play_Animation(fTimeDelta, m_pTransformCom);
	}
	else {
		Get_PartObject<CGoblin_BattleAxe>()->Set_Visible(false);
		m_vScale = { 1.f,1.f,1.f };
		m_pTransformCom->Set_Scale(m_vOriginScale);
		m_pModelCom->Set_AnimationIndex(5);
	}

	if (m_bDisolve) {
		Get_PartObject<CGoblin_BattleAxe>()->Set_Disolve(true);
		m_fDisolveTime += fTimeDelta;
		if (m_fDisolveTime >= 1.f)
		{
			m_fDisolveTime = 0.f;
			m_bDisolve = false;
			m_bVisible = false;
		}
	}

#ifdef _DEBUG
	Describe_Entity();
#endif // _DEBUG

#pragma region TRAIL_UPDATE

	_matrix WorldMat = m_pTransformCom->Get_XMWorldMatrix();

	_matrix LeftHandMatrix = {};
	_matrix RightHandMatrix = {};
	_matrix WeaponMatrix = {};

	LeftHandMatrix = XMLoadFloat4x4(m_pLeftHand_BoneMat);
	RightHandMatrix = XMLoadFloat4x4(m_pRightHand_BoneMat);


	for (int i = 0; i < 3; ++i) {
		LeftHandMatrix.r[i] = XMVector3Normalize(LeftHandMatrix.r[i]);
		RightHandMatrix.r[i] = XMVector3Normalize(RightHandMatrix.r[i]);
		WeaponMatrix.r[i] = XMVector3Normalize(WeaponMatrix.r[i]);
	}

	_matrix WeaponSoket = WeaponMatrix * Get_Component<CTransform>()->Get_XMWorldMatrix();

	m_pLeft_Trail->Trail_Update(LeftHandMatrix * WorldMat, fTimeDelta);
	m_pRight_Trail->Trail_Update(RightHandMatrix * WorldMat, fTimeDelta);

#pragma endregion

}

void CGoblin_Spector::Late_Update(_float fTimeDelta)
{
	__super::Late_Update(fTimeDelta);

	m_pGameInstance->Add_RenderGroup(RENDER::NONBLEND, this);
}

HRESULT CGoblin_Spector::Render()
{
	if (!m_bVisible)
		return S_OK;
	if (FAILED(Bind_ShaderResources())) {
		return E_FAIL;
	}

	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();
	_uint iShaderPass = ENUM_CLASS(SHADER_PASS_ANIM::SPECTOR);

	if (FAILED(Render_Disolve())) {
		return E_FAIL;
	}
	
	for (_uint i = 0; i < iNumMeshes; i++)
	{
		if (FAILED(m_pModelCom->Bind_BoneMatrices(i, m_pShaderCom, "g_BoneMatrices"))) {
			return E_FAIL;
		}

		if (FAILED(m_pModelCom->Bind_Material(i, m_pShaderCom))) {
			return E_FAIL;
		}

		if (FAILED(m_pShaderCom->Begin(iShaderPass))) {
			return E_FAIL;
		}

		if (FAILED(m_pModelCom->Render(i))) {
			return E_FAIL;
		}
	}

	{
		_bool bDisolve = false;
		_float zero = 0.f;
		m_pShaderCom->Bind_RawValue("g_bDisolve", &bDisolve, sizeof(_bool));
		m_pShaderCom->Bind_RawValue("g_fDisolveRatio", &zero, sizeof(_float));
	}

	return S_OK;
}

void CGoblin_Spector::OnCollision(CGameObject* pOther, void* pDesc)
{

}

void CGoblin_Spector::OnHit(CGameObject* pOther, CGameObject* pCaller)
{
}

HRESULT CGoblin_Spector::Ready_Components()
{
	CTransform::TRANSFORM_DESC Desc = {};

	Desc.fSpeedPerSec = 10.f;
	Desc.fRotationPerSec = XMConvertToRadians(180.0f);
	Desc.fRadius = 10.f;

	__super::Ready_Components(&Desc);

	m_strModelPrototypeTag = TEXT("Prototype_Component_Goblin_Spector_Model");

	/* Com_Model */
	if (FAILED(__super::Add_Asset_Component(g_iStaticLevel, m_strModelPrototypeTag,
		reinterpret_cast<CComponent**>(&m_pModelCom)))) {
		return E_FAIL;
	}

	CPartObject::PARTOBJECT_DESC PartsDesc{};

	PartsDesc.pParentTransform = m_pTransformCom;

	if (FAILED(Add_PartObject<CTrailObject>("Left_Trail", g_iStaticLevel, &m_pLeft_Trail, &PartsDesc))) {
		return E_FAIL;
	}

	m_pLeft_Trail->Load_Trail("../Bin/Resources/Data/Effect/Goblin/GoblinSide/Goblin_Trail_R", static_cast<LEVEL>(NEXT_LEVEL));
	m_pLeft_Trail->Set_Visible(false);

	if (FAILED(Add_PartObject<CTrailObject>("Right_Trail", g_iStaticLevel, &m_pRight_Trail, &PartsDesc))) {
		return E_FAIL;
	}

	m_pRight_Trail->Load_Trail("../Bin/Resources/Data/Effect/Goblin/GoblinSide/Goblin_Trail_R", static_cast<LEVEL>(NEXT_LEVEL));
	m_pRight_Trail->Set_Visible(false);


	return S_OK;
}

void CGoblin_Spector::Spector_Trail_Visible(_bool isTrailVisible)
{
	m_pLeft_Trail->Set_Visible(isTrailVisible);
	m_pRight_Trail->Set_Visible(isTrailVisible);

	m_pLeft_Trail->Get_Component<CTrail>()->Reset_Trail();
	m_pRight_Trail->Get_Component<CTrail>()->Reset_Trail();
}

HRESULT CGoblin_Spector::Ready_Parts()
{

	CGoblin_BattleAxe::GOBLIN_BATTLEAXE_DESC Goblin_BattleAxe_Desc{};

	Goblin_BattleAxe_Desc.pParentTransform = m_pTransformCom;
	Goblin_BattleAxe_Desc.pSocketMatrices = m_pModelCom->Get_BoneMatrixPtr("SKT_RightHand");

	if (FAILED(Add_PartObject<CGoblin_BattleAxe>("Goblin_BattleAxe", g_iStaticLevel, nullptr, &Goblin_BattleAxe_Desc)))
	{
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CGoblin_Spector::Bind_ShaderResources()
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

CGoblin_Spector* CGoblin_Spector::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CGoblin_Spector* pInstance = new CGoblin_Spector(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CGoblin_Spector");
		SAFE_RELEASE(pInstance);
	}
	return pInstance;
}

CGameObject* CGoblin_Spector::Clone(void* pArg, CGameObject* pOwner)
{
	CGoblin_Spector* pInstance = new CGoblin_Spector(*this);
	pInstance->m_pOwner = pOwner;
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CGoblin_Spector");
		SAFE_RELEASE(pInstance);
	}
	return pInstance;
}

void CGoblin_Spector::Free()
{
	__super::Free();


	SAFE_RELEASE(m_pGoblin);
	SAFE_RELEASE(m_pLeft_Trail);
	SAFE_RELEASE(m_pRight_Trail);
}
#ifdef _DEBUG

void CGoblin_Spector::Describe_Entity()
{
	GUI::Begin("UNIT", 0, IMGUI_GLOBAL_BEGIN_FLAG);
	GUI::PushItemWidth(80);
	if (GUI::CollapsingHeader("GoblinSpector")) {
		_float3 vScale = m_pTransformCom->Get_Scale();
		GUI::DragFloat3("Scale", (_float*)&vScale);
	}
	GUI::End();
}

#endif // _DEBUG
