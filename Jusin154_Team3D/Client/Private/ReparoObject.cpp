#include "pch.h"
#include "ReparoObject.h"

#include "GameInstance.h"
#include "InfoInstance.h"
#include "Unit.h"
#include "PartObject.h"
#include "EffectParts.h"

CReparoObject::CReparoObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CMonster(pDevice, pContext)
{
}

CReparoObject::CReparoObject(const CReparoObject& Prototype)
	: CMonster(Prototype)
{
}

HRESULT CReparoObject::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CReparoObject::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg))) {
		return E_FAIL;
	}

	if (FAILED(Ready_Components())) {
		return E_FAIL;
	}

	REPARO_OBJECT_DESC* pDesc = static_cast<REPARO_OBJECT_DESC*>(pArg);
	m_iModelID = pDesc->iModelID;
	m_fRadius = pDesc->fRadius;
	m_pTransformCom->Set_State(STATE::POSITION, XMVectorSetW(XMLoadFloat3(&pDesc->vPosition), 1.f));
	m_pTransformCom->Rotation(XMConvertToRadians(pDesc->vRotation.x), XMConvertToRadians(pDesc->vRotation.y), XMConvertToRadians(pDesc->vRotation.z));

	// 되돌리기 전 Before
	m_ePreState = m_eCurState = REPARO_OBJECT_STATE::SLEEP;
	m_pModelCom->Set_AnimationIndex(2);
	//m_pInfoInstance->Deregist_PlayerAlly(this);
	
	m_pInfoInstance->Deregist_ActiveMonster(this);
	return S_OK;
}

void CReparoObject::Priority_Update(_float fTimeDelta)
{
	__super::Priority_Update(fTimeDelta);
}

void CReparoObject::Update(_float fTimeDelta)
{
	__super::Update(fTimeDelta);

	Change_State();

	m_pModelCom->Play_Animation(fTimeDelta, m_pTransformCom);

	if (m_pModelCom->Get_AnimIndex() == 0)
	{
		if (true == m_pModelCom->IsFinishedAnim())
			m_eCurState = REPARO_OBJECT_STATE::AFTER;
	}

#ifdef _DEBUG
	Describe_Entity();
#endif // _DEBUG

}

void CReparoObject::Late_Update(_float fTimeDelta)
{
	__super::Late_Update(fTimeDelta);

	if(m_pGameInstance->IsIn_WorldFrustum(m_pTransformCom->Get_State(STATE::POSITION), m_fRadius))
		m_pGameInstance->Add_RenderGroup(RENDER::NONBLEND, this);

}

HRESULT CReparoObject::Render()
{
	if (!m_pModelCom) {
		return S_OK;
	}

	if (FAILED(Bind_ShaderResources())) {
		return E_FAIL;
	}

	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();
	_uint iShaderPass = ENUM_CLASS(SHADER_PASS_ANIM::DEFAULT);
	if (true == m_bDrawOutLine) {
		iShaderPass = ENUM_CLASS(SHADER_PASS_ANIM::OUTLINE_WRITE);
	}

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

		if (FAILED(m_pShaderCom->Begin(ENUM_CLASS(SHADER_PASS_ANIM::DEFAULT)))) {
			return E_FAIL;
		}

		m_pModelCom->Bind_OutPut_SRV_VS(31, 0);
		m_pModelCom->Bind_OutPut_SRV_VS_Prev(32, 0);

		if (FAILED(m_pModelCom->Render(i))) {
			return E_FAIL;
		}
	}

	if (m_bDrawOutLine) {
		Render_OutLine();
	}

	return S_OK;
}

_bool CReparoObject::IsRepairable(_bool isAwake)
{
	if (isAwake && REPARO_OBJECT_STATE::SLEEP == m_eCurState)
	{
		m_eCurState = REPARO_OBJECT_STATE::BEFORE;
		return false;
	}
	LOCKON_INFO info = {};
	m_pInfoInstance->Get_LockOnInfo(info);
	
	if (info.pUnit == this && REPARO_OBJECT_STATE::BEFORE == m_eCurState)
	{
		m_eCurState = REPARO_OBJECT_STATE::PLAYANIM;
		return true;
	}

	return false;
}

HRESULT CReparoObject::Render_OutLine()
{
	m_bDrawOutLine = false;
	if (FAILED(Bind_ShaderResources())) {
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

	Compute_Depth();
	_float fCamFar = *m_pGameInstance->Get_CurrentCameraFar();
	_float fRatio = CMyTools::Saturate((m_fCamDepth / (fCamFar * fCamFar)));
	m_fOutLineThickness = CMyTools::Lerp_f1D(0.024f, 0.5f, fRatio);
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

		if (FAILED(m_pShaderCom->Begin(ENUM_CLASS(SHADER_PASS_ANIM::OUTLINE_READ)))) {
			return E_FAIL;
		}

		m_pModelCom->Bind_OutPut_SRV_VS(31, 0);
		m_pModelCom->Bind_OutPut_SRV_VS_Prev(32, 0);

		if (FAILED(m_pModelCom->Render(i))) {
			return E_FAIL;
		}
	}
	return S_OK;
}
void CReparoObject::Change_State()
{
	if (m_eCurState != m_ePreState)
	{
		switch (m_eCurState)
		{
		case Client::CReparoObject::REPARO_OBJECT_STATE::SLEEP:
			break;

		case Client::CReparoObject::REPARO_OBJECT_STATE::BEFORE:
			m_pInfoInstance->Regist_ActiveMonster(this);
			break;

		case Client::CReparoObject::REPARO_OBJECT_STATE::PLAYANIM:
			m_pModelCom->Set_AnimationIndex(0, false);
			break;

		case Client::CReparoObject::REPARO_OBJECT_STATE::AFTER:
			m_pModelCom->Set_AnimationIndex(1);
			m_pInfoInstance->Deregist_ActiveMonster(this);
			break;

		case Client::CReparoObject::REPARO_OBJECT_STATE::END:
		default:
			break;
		}
		m_ePreState = m_eCurState;
	}
}

HRESULT CReparoObject::Ready_Components()
{
	CTransform::TRANSFORM_DESC Desc = {};

	Desc.fSpeedPerSec = 10.f;
	Desc.fRotationPerSec = XMConvertToRadians(180.0f);
	Desc.fRadius = 10.f;

	if (FAILED(Add_Component<CTransform>(g_iStaticLevel, &m_pTransformCom, &Desc))) {
		return E_FAIL;
	}

	if(0 == m_iModelID)
	{
		/* Com_Model */
		if (FAILED(__super::Add_Asset_Component(g_iStaticLevel, TEXT("Prototype_Component_VFX_SK_OLI_TrollFight_BlockerA_Model"),
			reinterpret_cast<CComponent**>(&m_pModelCom)))) {
			return E_FAIL;
		}
	}
	else
	{
		/* Com_Model */
		if (FAILED(__super::Add_Asset_Component(g_iStaticLevel, TEXT("Prototype_Component_VFX_SK_OLI_TrollFight_BlockerB_Model"),
			reinterpret_cast<CComponent**>(&m_pModelCom)))) {
			return E_FAIL;
		}
	}

	/* Com_Shader */
	if (FAILED(__super::Add_Asset_Component(g_iStaticLevel, FX_ANIMMESH,
		reinterpret_cast<CComponent**>(&m_pShaderCom)))) {
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CReparoObject::Bind_ShaderResources()
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

CReparoObject* CReparoObject::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CReparoObject* pInstance = new CReparoObject(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CReparoObject");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

CGameObject* CReparoObject::Clone(void* pArg, CGameObject* pOwner)
{
	CReparoObject* pInstance = new CReparoObject(*this);
	pInstance->m_pOwner = pOwner;
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CReparoObject");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

void CReparoObject::Free()
{
	__super::Free();

}
#ifdef _DEBUG

void CReparoObject::Describe_Entity()
{
	GUI::Begin("UNIT", 0, IMGUI_GLOBAL_BEGIN_FLAG);
	
	GUI::End();
}

#endif // _DEBUG
