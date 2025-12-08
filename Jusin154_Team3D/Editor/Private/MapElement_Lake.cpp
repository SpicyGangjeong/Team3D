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
	MAPOBJECT_LOD_DESC* pDesc = static_cast<MAPOBJECT_LOD_DESC*>(pArg);

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

#ifdef _DEBUG
	m_bSelected = false;
	m_vPosition = pDesc->vPosition;
	m_vScale = pDesc->vScale;
	m_vRotation = pDesc->vRotation;

	m_pTransformCom->Set_State(STATE::POSITION, XMVectorSetW(XMLoadFloat3(&m_vPosition), 1.f));
	m_pTransformCom->Set_Scale(m_vScale);
	m_pTransformCom->Rotation(XMConvertToRadians(m_vRotation.x), XMConvertToRadians(m_vRotation.y), XMConvertToRadians(m_vRotation.z));
#endif // _DEBUG

	m_vRefractionColor = _float4(0.29f, 0.375f, 0.4f, 1.f);
	m_vSurfaceColor = _float4(0.f, 1.f, 1.f, 1.f);

	return S_OK;
}

void CMapElement_Lake::Priority_Update(_float fTimeDelta)
{
#ifdef _DEBUG
	m_bSelected = false;
#endif 
}

void CMapElement_Lake::Update(_float fTimeDelta)
{
	m_fTimeAcc += fTimeDelta * m_fUVValue;
	//Describe_Entity();
}

void CMapElement_Lake::Late_Update(_float fTimeDelta)
{
#ifdef _DEBUG

	m_pTransformCom->Set_State(STATE::POSITION, XMVectorSetW(XMLoadFloat3(&m_vPosition), 1.f));
	m_pTransformCom->Set_Scale(m_vScale);
	m_pTransformCom->Rotation(XMConvertToRadians(m_vRotation.x), XMConvertToRadians(m_vRotation.y), XMConvertToRadians(m_vRotation.z));
	
#endif 
	m_pGameInstance->Add_RenderGroup(RENDER::NONBLEND, this);
}

HRESULT CMapElement_Lake::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Begin(ENUM_CLASS(SHADER_PASS_MESH::LAKE)))) {
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

	if (FAILED(m_pShaderCom->Bind_RawValue("g_fUVSpeed1", &m_fUVSpeed1, sizeof(_float)))) {
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fUVSpeed2", &m_fUVSpeed2, sizeof(_float)))) {
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fUVSpeed3", &m_fUVSpeed3, sizeof(_float)))) {
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
	if (FAILED(m_pShaderCom->Bind_SRV("g_NormalTexture", m_pNormalTextureCom->Get_SRV(0)))) {
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Bind_SRV("g_NormalLargeTexture", m_pNormalLargeTextureCom->Get_SRV(0)))) {
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Bind_SRV("g_NormalSubTexture", m_pNormalSubTextureCom->Get_SRV(0)))) {
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

	SAFE_RELEASE(m_pCubeMapTextureCom);
	SAFE_RELEASE(m_pNormalTextureCom);
	SAFE_RELEASE(m_pNormalLargeTextureCom);
	SAFE_RELEASE(m_pNormalSubTextureCom);
	SAFE_RELEASE(m_pCausticsTextureCom);
	SAFE_RELEASE(m_pNoiseTextureCom);
	SAFE_RELEASE(m_pShaderCom);
	for (auto& pModel : m_pModelComs)
		SAFE_RELEASE(pModel);
}

void CMapElement_Lake::Describe_Entity()
{
	if (m_bDead)
		return;
	if (nullptr == m_pGameInstance)
		return;
	GUI::Begin("Lake");
	GUI::Text(CMyTools::ToString(m_ModelPrototypeTags[m_iLodIndex]).c_str());
	GUI::InputInt("Lod Level", (_int*)(&m_iLodIndex));

	GUI::InputFloat("N_V1", (&m_fUVValue1));
	GUI::InputFloat("N_V2", (&m_fUVValue2));
	GUI::InputFloat("N_V3", (&m_fUVValue3));

	GUI::InputFloat("UV_Speed1", (&m_fUVSpeed1));
	GUI::InputFloat("UV_Speed2", (&m_fUVSpeed2));
	GUI::InputFloat("UV_Speed3", (&m_fUVSpeed3));

	GUI::InputFloat("m_fRefractionStrength", (&m_fRefractionStrength));
	GUI::InputFloat("m_fRefractionPow", (&m_fRefractionPow));

	m_iLodIndex = max(0, m_iLodIndex);
	m_iLodIndex = min(m_iMaxLodLevel, m_iLodIndex);

	GUI::ColorEdit4("Refaction Color", (_float*)&m_vRefractionColor);
	GUI::ColorEdit4("Surface Color", (_float*)&m_vSurfaceColor);
	GUI::InputFloat("UV", (_float*)&m_fUVValue);
	GUI::Text("----- Transfrom ----");
	GUI::InputFloat("X##Position", &m_vPosition.x, 0.1f, 1.f);
	GUI::InputFloat("Y##Position", &m_vPosition.y, 0.1f, 1.f);
	GUI::InputFloat("Z##Position", &m_vPosition.z, 0.1f, 1.f);

	if (m_pGameInstance->Mouse_Down(DIM_LBUTTON) && m_pGameInstance->Key_Pressing(DIK_LSHIFT))
	{
		if (m_pGameInstance->isPicking(&m_vPosition))
		{
		}
	}

	GUI::Text("----- Rotation ----");
	GUI::InputFloat("X##Rotation", &m_vRotation.x, 10.f, 45.f);
	GUI::InputFloat("Y##Rotation", &m_vRotation.y, 10.f, 45.f);
	GUI::InputFloat("Z##Rotation", &m_vRotation.z, 10.f, 45.f);

	GUI::Text("----- Scale ----");
	GUI::InputFloat("X##Scale", &m_vScale.x, 0.1f, 1.f);
	GUI::InputFloat("Y##Scale", &m_vScale.y, 0.1f, 1.f);
	GUI::InputFloat("Z##Scale", &m_vScale.z, 0.1f, 1.f);

	m_vScale.x = max(0.01f, m_vScale.x);
	m_vScale.y = max(0.01f, m_vScale.y);
	m_vScale.z = max(0.01f, m_vScale.z);

	if (GUI::Button("Delete"))
		m_bDead = true;
	GUI::End();
	m_bSelected = true;
}
