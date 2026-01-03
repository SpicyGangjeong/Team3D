#include "pch.h"
#include "NPC_EleazarFig.h"

#include "GameInstance.h"
#include "InfoInstance.h"
#include "Player.h"
#include "CallBack_NonPlayable_Behavior.h"
#include "CallBack_NonPlayable_HitReport.h"

CNPC_EleazarFig::CNPC_EleazarFig(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUnit(pDevice, pContext)
{
}

CNPC_EleazarFig::CNPC_EleazarFig(const CNPC_EleazarFig& Prototype)
	: CUnit(Prototype),
	m_pInfoInstance(CInfoInstance::GetInstance())
{
}

void CNPC_EleazarFig::Priority_Update(_float fTimeDelta)
{
	m_pTransformCom->RewindMomentum();
	m_iEntered -= 1;
	if (m_iEntered < 0) {
		m_iEntered = 0;
	}
	__super::Priority_Update(fTimeDelta);
}

void CNPC_EleazarFig::Update(_float fTimeDelta)
{
	m_pFSM->Update_State(fTimeDelta);
	m_pModelCom->Play_Animation(fTimeDelta, m_pTransformCom);
	__super::Update(fTimeDelta);
#ifdef _DEBUG
	Describe_Entity();
#endif // _DEBUG
}

void CNPC_EleazarFig::Late_Update(_float fTimeDelta)
{
	m_pRigidBody->Set_Position(m_pTransformCom->Get_State(STATE::POSITION), true);

	m_pGameInstance->Add_RenderGroup(RENDER::NONBLEND, this);

	Set_Shadow(m_pGameInstance->IsIn_ShadowViewFrustum(m_pTransformCom->Get_State(STATE::POSITION), m_pTransformCom->Get_Radius()));

	__super::Late_Update(fTimeDelta);
}

HRESULT CNPC_EleazarFig::Render()
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

HRESULT CNPC_EleazarFig::Render_Shadow(SHADOW eType)
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

HRESULT CNPC_EleazarFig::Bind_ShaderResources()
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

void CNPC_EleazarFig::OnRayCollision(CGameObject* pCaster, _uint iCastedOrder, _float fDistance, _float3 vCastedWorldPos)
{
	CPlayer* pPlayer = dynamic_cast<CPlayer*>(pCaster);
	if (nullptr == pPlayer) {
		return;
	}
	if (fDistance < m_fEncounterDistance) {
		m_iEntered = 4;
	}
}
HRESULT CNPC_EleazarFig::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CNPC_EleazarFig::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg))) {
		return E_FAIL;
	}
	if (FAILED(Ready_Components(pArg))) {
		return E_FAIL;
	}
	NPCDESC* pDesc = static_cast<NPCDESC*>(pArg);
	
	_vector vPos = XMLoadFloat4(&pDesc->vPos);
	m_pTransformCom->Set_State(STATE::POSITION, vPos);
	m_pTransformCom->Rotation(XMLoadFloat4(&pDesc->vRotQ));

	m_pModelCom->Set_AnimationIndex(0, true);
	return S_OK;
}

HRESULT CNPC_EleazarFig::Ready_Components(void* pArg)
{
	CTransform::TRANSFORM_DESC Desc = {};

	Desc.fSpeedPerSec = 10.f;
	Desc.fRotationPerSec = XMConvertToRadians(180.0f);
	Desc.fRadius = 10.f;

	if (FAILED(__super::Ready_Components(&Desc))) {
		return E_FAIL;
	}

	/* Com_Shader */
	if (FAILED(__super::Add_Asset_Component(g_iStaticLevel, FX_NPC_PBR_ANIM,
		reinterpret_cast<CComponent**>(&m_pShaderCom)))) {
		return E_FAIL;
	}
	if (FAILED(__super::Add_Asset_Component(g_iStaticLevel, TEXT("Prototype_Component_Professor_EleazarFig_Model"),
		reinterpret_cast<CComponent**>(&m_pModelCom)))) {
		return E_FAIL;
	}
	{ // DO
		CRigidBody_Dynamic::RIGIDBODY_DYNAMIC_DESC Desc{};
		Desc.iSubKind = ENUM_CLASS(PXOBJECT::ELEAZARFIG);
		Desc.bAutoOwnerTranslation = false;
		if (FAILED(Add_Asset_Component(g_iStaticLevel, TEXT("PHYSX_NPC_HITBOX"), (CComponent**)&m_pRigidBody, &Desc))) {
			return E_FAIL;
		}
	}

	return S_OK;
}

CNPC_EleazarFig* CNPC_EleazarFig::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CNPC_EleazarFig* pInstance = new CNPC_EleazarFig(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CNPC_EleazarFig");
		SAFE_RELEASE(pInstance);
	}
	return pInstance;
}

CGameObject* CNPC_EleazarFig::Clone(void* pArg, CGameObject* pOwner)
{
	CNPC_EleazarFig* pInstance = new CNPC_EleazarFig(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CNPC_EleazarFig");
		SAFE_RELEASE(pInstance);
	}
	return pInstance;
}

void CNPC_EleazarFig::Free()
{
	__super::Free();

	SAFE_RELEASE(m_pRigidBody);
	if (nullptr != m_pInfoInstance) {
		CInfoInstance* pInfo = m_pInfoInstance;
		m_pInfoInstance = nullptr;
	}
}
#ifdef _DEBUG

void CNPC_EleazarFig::Describe_Entity()
{
}

#endif // _DEBUG
