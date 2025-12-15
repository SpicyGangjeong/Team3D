#include "pch.h"
#include "MapElement_Light.h"

#include "GameInstance.h"

#include "Terrain.h"
#include "Layer.h"
#include "VIBuffer_Terrain.h"

CMapElement_Light::CMapElement_Light(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CMapElement{ pDevice, pContext }
{
}

CMapElement_Light::CMapElement_Light(const CMapElement_Light& rhs)
	: CMapElement(rhs)
{
}

HRESULT CMapElement_Light::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CMapElement_Light::Initialize(void* pArg)
{
	MAPELEMENT_LIGHT_DESC* pDesc = static_cast<MAPELEMENT_LIGHT_DESC*>(pArg);

	m_iMaxLodLevel = pDesc->iMaxLodLevel;
	m_fBloomStrength = pDesc->fBloomStregth;
	m_iGlassMeshIndex = pDesc->iGlassMeshIndex;

	for (_uint i = 0; i < m_iMaxLodLevel + 1; i++)
	{
		m_ModelPrototypeTags.push_back(pDesc->ModelPrototypeTags[i]);
		//m_ModelPathIndices.push_back((*pDesc->pModelPathIndices)[i]);
	}

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components(pArg)))
		return E_FAIL;

	m_pTransformCom->Set_State(STATE::POSITION, XMVectorSetW(XMLoadFloat3(&pDesc->vPosition), 1.f));
	m_pTransformCom->Set_Scale(pDesc->vScale);
	m_pTransformCom->Rotation(XMConvertToRadians(pDesc->vRotation.x), XMConvertToRadians(pDesc->vRotation.y), XMConvertToRadians(pDesc->vRotation.z));

	return S_OK;
}

void CMapElement_Light::Priority_Update(_float fTimeDelta)
{
}

void CMapElement_Light::Update(_float fTimeDelta)
{
}

void CMapElement_Light::Late_Update(_float fTimeDelta)
{
	if (m_pGameInstance->IsIn_WorldFrustum(Get_WorldPostion(), m_pModelComs[0]->Get_Radius()))
	{
		if (m_isLightOn)
		{
			_float fDistance = XMVectorGetX(XMVector3LengthSq(XMLoadFloat4(m_pGameInstance->Get_CamPosition()) - m_pTransformCom->Get_State(STATE::POSITION)));
			if (m_LightAdded_Distance > fDistance) // 카메라 범위 안
			{
				if (!m_isLightAdded)
				{
					m_isLightAdded = true;
					m_pGameInstance->Add_Light(CURRENT_LEVEL, m_pLightCom);
				}
			}
			else
			{
				if (m_isLightAdded)
				{
					m_isLightAdded = false;
					m_pGameInstance->Delete_Light(CURRENT_LEVEL, m_pLightCom);
				}
			}

			m_pGameInstance->Add_RenderGroup(RENDER::BLOOM, this);
		}

		m_pGameInstance->Add_RenderGroup(RENDER::NONBLEND, this);
	}
}

HRESULT CMapElement_Light::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	_uint		iNumMeshes = m_pModelComs[m_iLodIndex]->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; i++)
	{
		if (true == m_isLightOn && m_iGlassMeshIndex == i)
			continue;

		if (m_iGlassMeshIndex == i)
		{
			if (FAILED(m_pModelComs[0]->Bind_Material(m_iGlassMeshIndex, m_pShaderCom))) {
				return E_FAIL;
			}

			if (FAILED(m_pShaderCom->Bind_SRV("g_DiffuseTexture", m_pGlassTextureCom->Get_SRV(0)))) {
				return E_FAIL;
			}

			if (FAILED(m_pShaderCom->Bind_SRV("g_GlassTexture", m_pMaskTextureCom->Get_SRV(0)))) {
				return E_FAIL;
			}

			if (FAILED(m_pShaderCom->Bind_RawValue("g_fGlassRatio", &m_fGlassRatio, sizeof(_float)))) {
				return E_FAIL;
			}

			if (FAILED(m_pShaderCom->Begin(ENUM_CLASS(SHADER_PASS_MESH::GLASS)))) {
				return E_FAIL;
			}
		}
		else
		{
			if (FAILED(m_pModelComs[0]->Bind_Material(i, m_pShaderCom))) {
				return E_FAIL;
			}

			if (FAILED(m_pShaderCom->Begin(ENUM_CLASS(SHADER_PASS_MESH::DEFAULT)))) {
				return E_FAIL;
			}
		}

		if (FAILED(m_pModelComs[m_iLodIndex]->Render(i))) {
			return E_FAIL;
		}
	}

	return S_OK;
}

HRESULT CMapElement_Light::Ready_Components(void* pArg)
{
	if (FAILED(__super::Ready_Components(pArg))) {
		return E_FAIL;
	}

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

	/* Com_Texture */
	if (FAILED(__super::Add_Asset_Component(g_iStaticLevel, TEXT("LightPost_Emissive"),
		reinterpret_cast<CComponent**>(&m_pEmissiveTextureCom))))
		return E_FAIL;

	/* Com_Texture */
	if (FAILED(__super::Add_Asset_Component(g_iStaticLevel, TEXT("LightPost_Mask"),
		reinterpret_cast<CComponent**>(&m_pMaskTextureCom))))
		return E_FAIL;

	/* Com_Texture */
	if (FAILED(__super::Add_Asset_Component(g_iStaticLevel, TEXT("LightPost_Base"),
		reinterpret_cast<CComponent**>(&m_pGlassTextureCom))))
		return E_FAIL;


	MAPELEMENT_LIGHT_DESC* pDesc = static_cast<MAPELEMENT_LIGHT_DESC*>(pArg);

	LIGHT_DESC			LightDesc{};

	LightDesc.eType = LIGHT::POINT;

	memcpy(&LightDesc.vDiffuse, &pDesc->vDiffuse, sizeof(_float4));
	memcpy(&LightDesc.vAmbient, &pDesc->vAmbient, sizeof(_float4));
	memcpy(&LightDesc.vSpecular, &pDesc->vSpecular, sizeof(_float4));
	memcpy(&LightDesc.vPosOffset, &pDesc->vPosOffset, sizeof(_float4));

	LightDesc.fRange = pDesc->fRange;
	LightDesc.iLevel = NEXT_LEVEL;
	LightDesc.pPosition = m_pTransformCom->Get_StatePtr(STATE::POSITION);

	/* Com_Light*/
	if (FAILED(Add_Component<CLight>(g_iStaticLevel, &m_pLightCom, &LightDesc)))
	{
		return E_FAIL;
	}

	if (false == pDesc->isPow)
		m_pLightCom->Switch_LightAttenationMethod();

	return S_OK;
}

HRESULT CMapElement_Light::Bind_ShaderResources()
{
	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", m_pTransformCom->Get_WorldMatrixPtr()))) {
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

HRESULT CMapElement_Light::Render_Bloom()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_fBloomStrength", &m_fBloomStrength, sizeof(_float))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_SRV("g_EmissiveTexture", m_pEmissiveTextureCom->Get_SRV(0))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_SRV("g_DiffuseTexture", m_pGlassTextureCom->Get_SRV(0))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Begin(ENUM_CLASS(SHADER_PASS_MESH::BLOOM)))) {
		return E_FAIL;
	}

	if (FAILED(m_pModelComs[m_iLodIndex]->Render(m_iGlassMeshIndex))) {
		return E_FAIL;
	}

	return S_OK;
}

void CMapElement_Light::Toggle_Light()
{
	if (m_isLightOn)
		m_pGameInstance->Delete_Light(CURRENT_LEVEL, m_pLightCom);
	else
		m_pGameInstance->Add_Light(CURRENT_LEVEL, m_pLightCom);

	m_isLightAdded = m_isLightOn = !m_isLightOn;
}

CMapElement_Light* CMapElement_Light::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CMapElement_Light* pInstance = new CMapElement_Light(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CMapElement_Light");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

CGameObject* CMapElement_Light::Clone(void* pArg, CGameObject* pOwner)
{
	CMapElement_Light* pInstance = new CMapElement_Light(*this);
	pInstance->m_pOwner = pOwner;
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CMapElement_Light");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

void CMapElement_Light::Free()
{
	__super::Free();

	SAFE_RELEASE(m_pLightCom);
	SAFE_RELEASE(m_pEmissiveTextureCom);
	SAFE_RELEASE(m_pMaskTextureCom);
	SAFE_RELEASE(m_pGlassTextureCom);

	SAFE_RELEASE(m_pShaderCom);
	for (auto& pModel : m_pModelComs)
		SAFE_RELEASE(pModel);
}

#ifdef _DEBUG
void CMapElement_Light::Describe_Entity()
{

}
#endif // DEBUG


