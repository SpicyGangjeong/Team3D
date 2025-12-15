#include "pch.h"
#include "MapElement_Lake.h"

#include "GameInstance.h"

#include "Terrain.h"
#include "Layer.h"

CMapElement_Lake::CMapElement_Lake(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CMapElement{ pDevice, pContext }
{
}

CMapElement_Lake::CMapElement_Lake(const CMapElement_Lake& rhs)
	: CMapElement(rhs)
{
}

HRESULT CMapElement_Lake::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CMapElement_Lake::Initialize(void* pArg)
{
	MAPOBJECT_LAKE_DESC* pDesc = static_cast<MAPOBJECT_LAKE_DESC*>(pArg);

	m_iMaxLodLevel = pDesc->iMaxLodLevel;

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
	
	m_fRadius = pDesc->fRadius;
	m_fTimeSpeed = pDesc->fTimeSpeed;

	m_fRefractionStrength = pDesc->fRefractionStrength;
	m_fRefractionPow = pDesc->fRefractionPow;

	m_fUVValue1 = pDesc->fUVValue1;
	m_fUVValue2 = pDesc->fUVValue2;
	m_fUVValue3 = pDesc->fUVValue3;

	m_vUVSpeed = pDesc->vUVSpeed;
	m_vLargeUVSpeed = pDesc->vLargeUVSpeed;
	m_vSubUVSpeed3 = pDesc->vSubUVSpeed3;

	memcpy(&m_vRefractionColor, &pDesc->vRefractionColor, sizeof(_float4));
	memcpy(&m_vSurfaceColor, &pDesc->vSurfaceColor, sizeof(_float4));

	return S_OK;
}

void CMapElement_Lake::Priority_Update(_float fTimeDelta)
{
}

void CMapElement_Lake::Update(_float fTimeDelta)
{
	m_fTimeAcc += fTimeDelta * m_fTimeSpeed;

	if (3600.f <= m_fTimeAcc)
		m_fTimeAcc = 0.f;
	//Describe_Entity();
}

void CMapElement_Lake::Late_Update(_float fTimeDelta)
{
	if(m_pGameInstance->IsIn_WorldFrustum(m_pTransformCom->Get_State(STATE::POSITION), m_fRadius))
		m_pGameInstance->Add_RenderGroup(RENDER::NONBLEND, this);
}

HRESULT CMapElement_Lake::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Begin(ENUM_CLASS(SHADER_PASS_MESH::WATER)))) {
		return E_FAIL;
	}

	if (FAILED(m_pModelComs[0]->Render(0))) {
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CMapElement_Lake::Ready_Components(void* pArg)
{
	if (FAILED(__super::Ready_Components(pArg))) {
		return E_FAIL;
	}

	MAPOBJECT_LAKE_DESC* pDesc = static_cast<MAPOBJECT_LAKE_DESC*>(pArg);

	for (_uint i = 0; i < m_iMaxLodLevel + 1; ++i)
	{
		CModel* pModel = { nullptr };

		/* Com_Model */
		if (FAILED(__super::Add_Asset_Component(g_iStaticLevel, m_ModelPrototypeTags[i],
			reinterpret_cast<CComponent**>(&pModel))))
			return E_FAIL;

		m_pModelComs.push_back(pModel);

		/* Com_Model */
		if (FAILED(__super::Add_Asset_Component(g_iStaticLevel, pDesc->ShallowModelPrototypeTags[i],
			reinterpret_cast<CComponent**>(&pModel))))
			return E_FAIL;

		m_pShallowModels.push_back(pModel);
	}

	/* Com_Shader */
	if (FAILED(__super::Add_Asset_Component(g_iStaticLevel, FX_MESH,
		reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* Com_Texture */
	if (FAILED(__super::Add_Asset_Component(g_iStaticLevel, TEXT("T_Noises_D"),
		reinterpret_cast<CComponent**>(&m_pNoiseTextureCom))))
		return E_FAIL;
	/* Com_Texture */
	if (FAILED(__super::Add_Asset_Component(g_iStaticLevel, TEXT("T_Water_Normal_Large_N"),
		reinterpret_cast<CComponent**>(&m_pNormalLargeTextureCom))))
		return E_FAIL;
	/* Com_Texture */
	if (FAILED(__super::Add_Asset_Component(g_iStaticLevel, TEXT("T_Water_Normal_Subtle_N"),
		reinterpret_cast<CComponent**>(&m_pNormalSubTextureCom))))
		return E_FAIL;
	/* Com_Texture */
	if (FAILED(__super::Add_Asset_Component(g_iStaticLevel, TEXT("T_Caustics_D"),
		reinterpret_cast<CComponent**>(&m_pCausticsTextureCom))))
		return E_FAIL;
	/* Com_Texture */
	if (FAILED(__super::Add_Asset_Component(g_iStaticLevel, TEXT("T_Water_N"),
		reinterpret_cast<CComponent**>(&m_pNormalTextureCom))))
		return E_FAIL;
	/* Com_Texture */
	if (FAILED(__super::Add_Asset_Component(g_iStaticLevel, TEXT("Lake_Cube_D"),
		reinterpret_cast<CComponent**>(&m_pCubeMapTextureCom))))
		return E_FAIL;
	/* Com_Texture */
	if (FAILED(__super::Add_Asset_Component(g_iStaticLevel, TEXT("Lake_Refraction"),
		reinterpret_cast<CComponent**>(&m_pRefractionTextureCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CMapElement_Lake::Bind_ShaderResources()
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

	if (FAILED(m_pShaderCom->Bind_RawValue("g_fTime", &m_fTimeAcc, sizeof(_float)))) {
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fNormalValue1", &m_fUVValue1, sizeof(_float)))) {
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fNormalValue2", &m_fUVValue2, sizeof(_float)))) {
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fNormalValue3", &m_fUVValue3, sizeof(_float)))) {
		return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Bind_RawValue("g_vUVSpeed1", &m_vUVSpeed, sizeof(_float2)))) {
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Bind_RawValue("g_vUVSpeed2", &m_vLargeUVSpeed, sizeof(_float2)))) {
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Bind_RawValue("g_vUVSpeed3", &m_vSubUVSpeed3, sizeof(_float2)))) {
		return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Bind_RawValue("g_fRefractionStrength", &m_fRefractionStrength, sizeof(_float)))) {
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fRefractionPow", &m_fRefractionPow, sizeof(_float)))) {
		return E_FAIL;
	}


	if (FAILED(m_pShaderCom->Bind_RawValue("g_vCamPosition", m_pGameInstance->Get_CamPosition(), sizeof(_float3)))) {
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Bind_RawValue("g_vDiffuseColor", &m_vRefractionColor, sizeof(_float4)))) {
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Bind_RawValue("g_vSurfaceColor", &m_vSurfaceColor, sizeof(_float4)))) {
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Bind_SRV("g_DiffuseTexture", m_pRefractionTextureCom->Get_SRV(0)))) {
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Bind_SRV("g_NormalTexture", m_pNormalTextureCom->Get_SRV(0)))) {
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Bind_SRV("g_NormalLargeTexture", m_pNormalLargeTextureCom->Get_SRV(0)))) {
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Bind_SRV("g_CausticsTexture", m_pCausticsTextureCom->Get_SRV(0)))) {
		return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Bind_SRV("g_NoiseTexture", m_pNoiseTextureCom->Get_SRV(0)))) {
		return E_FAIL;
	}
	if (FAILED(m_pCubeMapTextureCom->Bind_ShaderResource(m_pShaderCom, "g_CubeTexture", 0))) {
		return E_FAIL;
	}



	return S_OK;
}

CMapElement_Lake* CMapElement_Lake::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CMapElement_Lake* pInstance = new CMapElement_Lake(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CMapElement_Lake");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

CGameObject* CMapElement_Lake::Clone(void* pArg, CGameObject* pOwner)
{
	CMapElement_Lake* pInstance = new CMapElement_Lake(*this);
	pInstance->m_pOwner = pOwner;
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CMapElement_Lake");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

void CMapElement_Lake::Free()
{
	__super::Free();

	SAFE_RELEASE(m_pRefractionTextureCom);
	SAFE_RELEASE(m_pCubeMapTextureCom);
	SAFE_RELEASE(m_pNormalTextureCom);
	SAFE_RELEASE(m_pNormalLargeTextureCom);
	SAFE_RELEASE(m_pNormalSubTextureCom);
	SAFE_RELEASE(m_pCausticsTextureCom);
	SAFE_RELEASE(m_pNoiseTextureCom);
	SAFE_RELEASE(m_pShaderCom);

	for (auto& pModel : m_pModelComs)
		SAFE_RELEASE(pModel);
	for (auto& pModel : m_pShallowModels)
		SAFE_RELEASE(pModel);
}

#ifdef _DEBUG
void CMapElement_Lake::Describe_Entity()
{
}
#endif // _DEBUG
