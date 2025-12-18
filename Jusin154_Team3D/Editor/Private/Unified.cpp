#include "pch.h"
#include "Unified.h"

#include "GameInstance.h"
#include "Layer.h"

CUnified::CUnified(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject{ pDevice, pContext }
{
}

CUnified::CUnified(const CUnified& rhs)
	: CGameObject(rhs)
{
}

void CUnified::Priority_Update(_float fTimeDelta)
{
}

void CUnified::Update(_float fTimeDelta)
{
	m_fCamDistance = XMVector3Length(XMLoadFloat4(&m_vUnifiedCenterPos) - XMLoadFloat4(m_pGameInstance->Get_CamPosition())).m128_f32[0];

	if (true == m_isLayerEnable && m_fLodSwitchDistnace <= m_fCamDistance)
	{ // LOD 전환
		m_isLayerEnable = false;
		m_pGameInstance->Get_Layer(ENUM_CLASS(LEVEL::MAP), m_srtLayerTag)->Set_LayerEnabled(false);
	}
	else if(false == m_isLayerEnable  && m_fLodSwitchDistnace > m_fCamDistance)
	{ // LOD 해제
		m_isLayerEnable = true;
		m_pGameInstance->Get_Layer(ENUM_CLASS(LEVEL::MAP), m_srtLayerTag)->Set_LayerEnabled(true);
	}
}

void CUnified::Late_Update(_float fTimeDelta)
{
	if(m_isVisible || false == m_isLayerEnable)
		m_pGameInstance->Add_RenderGroup(RENDER::NONBLEND, this);
}

HRESULT CUnified::Render()
{
	if (FAILED(Bind_ShaderResources())) {
		return E_FAIL;
	}

	for(auto& pModel : m_pModelComs)
	{
		_uint		iNumMeshes = pModel->Get_NumMeshes();

		for (_uint i = 0; i < iNumMeshes; i++)
		{
			if (FAILED(pModel->Bind_Material(i, m_pShaderCom))) {
				return E_FAIL;
			}
			if (FAILED(m_pShaderCom->Begin(ENUM_CLASS(SHADER_PASS_MESH::DEFAULT)))) {
				return E_FAIL;
			}

			if (FAILED(pModel->Render(i))) {
				return E_FAIL;
			}
		}
	}

	return S_OK;
}

HRESULT CUnified::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CUnified::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components(pArg)))
		return E_FAIL;
	
	UNIFIED_DESC* pDesc = static_cast<UNIFIED_DESC*>(pArg);

	m_srtLayerTag = pDesc->srtLayerTag;
	m_fLodSwitchDistnace = pDesc->fLodSwitchDistnace;
	m_vUnifiedCenterPos = pDesc->vUnifiedCenterPos;

	m_pTransformCom->Set_State(STATE::POSITION, XMVectorSetW(XMLoadFloat3(&pDesc->vPosition), 1.f));
	m_pTransformCom->Set_Scale(pDesc->vScale);
	m_pTransformCom->Rotation(XMConvertToRadians(pDesc->vRotation.x), XMConvertToRadians(pDesc->vRotation.y), XMConvertToRadians(pDesc->vRotation.z));
	m_fUsingSurfaceParams = 0.f;

	return S_OK;
}

HRESULT CUnified::Ready_Components(void* pArg)
{
	if (FAILED(__super::Ready_Components(pArg))) {
		return E_FAIL;
	}

	UNIFIED_DESC* pDesc = static_cast<UNIFIED_DESC*>(pArg);

	/* Com_Model */
	CModel* pModel = { nullptr };

	_uint iNumModel = (_uint)pDesc->srtModelPrototypeTags.size();
	m_pModelComs.reserve(iNumModel);

	for (_uint i = 0; i < iNumModel; ++i)
	{
		if (FAILED(__super::Add_Asset_Component(g_iStaticLevel, pDesc->srtModelPrototypeTags[i],
			reinterpret_cast<CComponent**>(&pModel))))
			return E_FAIL;

		m_pModelComs.push_back(pModel);
	}

	/* Com_Shader */
	if (FAILED(__super::Add_Asset_Component(g_iStaticLevel, FX_MESH,
		reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CUnified::Bind_ShaderResources()
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

	if (FAILED(m_pShaderCom->Bind_RawValue("g_fUsingSurfaceParams", &m_fUsingSurfaceParams, sizeof(_float)))) {
		return E_FAIL;
	}

	return S_OK;
}

CUnified* CUnified::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUnified* pInstance = new CUnified(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CUnified");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

CGameObject* CUnified::Clone(void* pArg, CGameObject* pOwner)
{
	CUnified* pInstance = new CUnified(*this);
	pInstance->m_pOwner = pOwner;
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CUnified");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}
void CUnified::Free()
{
	__super::Free();

	SAFE_RELEASE(m_pShaderCom);

	for(auto& pModelCom : m_pModelComs)
		SAFE_RELEASE(pModelCom);
}

void CUnified::Describe_Entity()
{
	if(GUI::CollapsingHeader(CMyTools::ToString(L"" + m_srtLayerTag).c_str()))
	{
		GUI::Checkbox("Visible ON / OFF", &m_isVisible);
		GUI::Text(CMyTools::ToString(m_srtLayerTag).c_str());
		GUI::Text("%f", m_fCamDistance);                                                                                                                                                                                                                                                                                                                                                      
		GUI::InputFloat("Switch Dist", &m_fLodSwitchDistnace);
		GUI::InputFloat3("Centor Pos", (_float*)&m_vUnifiedCenterPos);
		m_pTransformCom->Describe_Entity();
	}
}
