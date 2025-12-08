#include "pch.h"
#include "MapObject_Render.h"

#include "GameInstance.h"
#include "Camera_Debug.h"
#include "Layer.h"
#include "VIBuffer_Terrain.h"
#include "Character_Controller.h"
#include "Bounding_Sphere.h"
#include "Collider.h"

CMapObject_Render::CMapObject_Render(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CMapPartObject(pDevice, pContext)
{
}

CMapObject_Render::CMapObject_Render(const CMapObject_Render& Prototype)
	: CMapPartObject(Prototype), m_iLodIndex{ 0 }
{
}

HRESULT CMapObject_Render::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CMapObject_Render::Initialize(void* pArg)
{
	MAPOBJECT_DESC* pDesc = static_cast<MAPOBJECT_DESC*>(pArg);

	//m_Type = static_cast<MAPOBJECT_RENDER_TYPE>(pDesc->iRenderType);
	m_iMaxLodLevel = pDesc->iMaxLodLevel;

	for (_uint i = 0; i < m_iMaxLodLevel + 1; i++)
	{
		m_ModelPrototypeTags.push_back(pDesc->ModelPrototypeTags[i]);
		//m_ModelPathIndices.push_back((*pDesc->pModelPathIndices)[i]);
	}

	/*if (_wstring::npos != m_ModelPrototypeTags.front().find(L"Glass"))
		m_iShaderPass = 12;
	else*/

	m_iShaderPass = ENUM_CLASS(SHADER_PASS_MESH::DEFAULT);

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_fRadius = m_pModelComs[0]->Get_Radius();

	m_pTransformCom->Set_State(STATE::POSITION, XMVectorSetW(XMLoadFloat3(&pDesc->vPosition), 1.f));
	m_pTransformCom->Set_Scale(pDesc->vScale);
	m_pTransformCom->Rotation(XMConvertToRadians(pDesc->vRotation.x), XMConvertToRadians(pDesc->vRotation.y), XMConvertToRadians(pDesc->vRotation.z));

	XMStoreFloat4x4(&m_CombinedWorldMatrix, m_pTransformCom->Get_XMWorldMatrix() * m_pParentTransformCom->Get_XMWorldMatrix());

	_float3 vOffset = m_pModelComs[0]->Get_RadiusOffset();
	_matrix ColliderMatrix = XMMatrixTranslation(vOffset.x, vOffset.y, vOffset.z) * XMLoadFloat4x4(&m_CombinedWorldMatrix);
	XMStoreFloat4(&m_vExtentPosition, ColliderMatrix.r[3]);

	return S_OK;
}

void CMapObject_Render::Priority_Update(_float fTimeDelta)
{
}

void CMapObject_Render::Update(_float fTimeDelta)
{

}

void CMapObject_Render::Late_Update(_float fTimeDelta)
{
	//m_pGameInstance->Add_RenderGroup(RENDER::SHADOW, this);
	if (m_pGameInstance->isIn_WorldFrustum(XMLoadFloat4(&m_vExtentPosition), m_fRadius)) {

		m_fCamDepth = XMVectorGetX(XMVector3LengthSq(XMLoadFloat4(m_pGameInstance->Get_CamPosition()) - XMLoadFloat4(&m_vExtentPosition)));

		m_iLodIndex = min(m_iMaxLodLevel, (_uint)(m_fCamDepth / (m_fRadius * m_fRadius + 3600.f)));

		m_iNumMeshe = m_pModelComs[m_iLodIndex]->Get_NumMeshes();

		switch (m_Type)
		{
		case MAPOBJECT_RENDER_TYPE::NORMAL:
			m_pGameInstance->Add_RenderGroup(RENDER::NONBLEND, this);
			break;

		case MAPOBJECT_RENDER_TYPE::GLASS:
			break;

		case MAPOBJECT_RENDER_TYPE::DECAL:
			m_pGameInstance->Add_RenderGroup(RENDER::DECAL, this);
			break;

		default:
			break;
		}
	}
}

HRESULT CMapObject_Render::Render()
{
	if (FAILED(Bind_ShaderResources())) {
		return E_FAIL;
	}

	for (_uint i = 0; i < m_iNumMeshe; i++)
	{
		if (FAILED(m_pModelComs[0]->Bind_Material(i, m_pShaderCom))) {
			return E_FAIL;
		}

		if (FAILED(m_pShaderCom->Begin(m_iShaderPass))) {
			return E_FAIL;
		}

		if (FAILED(m_pModelComs[m_iLodIndex]->Render(i))) {
			return E_FAIL;
		}
	}

	return S_OK;
}

HRESULT CMapObject_Render::Render_Shadow()
{
	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_CombinedWorldMatrix))) {
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

	for (_uint i = 0; i < m_iNumMeshe; i++)
	{
		if (FAILED(m_pModelComs[0]->Bind_Material(i, m_pShaderCom))) {
			return E_FAIL;
		}

		if (FAILED(m_pShaderCom->Begin(ENUM_CLASS(SHADER_PASS_MESH::SHADOW)))) {
			return E_FAIL;
		}

		if (FAILED(m_pModelComs[m_iLodIndex]->Render(i))) {
			return E_FAIL;
		}
	}

	return S_OK;
}

_wstring CMapObject_Render::Get_PrototypeTag(_uint iLodIndex)
{
	if (m_iMaxLodLevel < iLodIndex)
		return m_ModelPrototypeTags[0];

	return m_ModelPrototypeTags[iLodIndex];
}

HRESULT CMapObject_Render::Add_LodModel(const _tchar* pModelPrototypeTag)
{
	CModel* pModel = { nullptr };

	if (FAILED(__super::Add_Asset_Component(g_iStaticLevel, pModelPrototypeTag,
		reinterpret_cast<CComponent**>(&pModel))))
		return E_FAIL;

	m_ModelPrototypeTags.push_back(pModelPrototypeTag);

	++m_iMaxLodLevel;
	m_pModelComs.push_back(pModel);

	return S_OK;
}

HRESULT CMapObject_Render::Ready_Components()
{
	__super::Ready_Components();

	for (_uint i = 0; i < m_iMaxLodLevel + 1; ++i)
	{
		CModel* pModel = { nullptr };

		/* Com_Model */
		if (FAILED(__super::Add_Asset_Component(g_iStaticLevel, m_ModelPrototypeTags[i],
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

HRESULT CMapObject_Render::Bind_ShaderResources()
{
	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_CombinedWorldMatrix))) {
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

CMapObject_Render* CMapObject_Render::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CMapObject_Render* pInstance = new CMapObject_Render(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CMapObject_Render");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

CGameObject* CMapObject_Render::Clone(void* pArg, CGameObject* pOwner)
{
	CMapObject_Render* pInstance = new CMapObject_Render(*this);
	pInstance->m_pOwner = pOwner;
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CMapObject_Render");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

void CMapObject_Render::Free()
{
	__super::Free();

	SAFE_RELEASE(m_pShaderCom);

	for (auto& pModel : m_pModelComs)
		SAFE_RELEASE(pModel);
	m_pModelComs.clear();

	m_ModelPrototypeTags.clear();
	m_ModelPathIndices.clear();
}
#ifdef _DEBUG
void CMapObject_Render::Describe_Entity()
{
}
#endif // _DEBUG


