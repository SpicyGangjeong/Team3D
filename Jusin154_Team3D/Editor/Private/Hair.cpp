#include "pch.h"
#include "Hair.h"

#include "GameInstance.h"
#include "RootModelPart.h"

CHair::CHair(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CModelParts(pDevice, pContext)
{
}

CHair::CHair(const CHair& Prototype)
	: CModelParts(Prototype)
{
}

HRESULT CHair::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CHair::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	return S_OK;
}

void CHair::Priority_Update(_float fTimeDelta)
{

}

void CHair::Update(_float fTimeDelta)
{
	__super::Update(fTimeDelta);
}

void CHair::Late_Update(_float fTimeDelta)
{
	if (m_pGameInstance->isIn_WorldFrustum(Get_WorldPostion(), m_pTransformCom->Get_Radius())) {
		m_pGameInstance->Add_RenderGroup(RENDER::BLEND, this);
	}
}

HRESULT CHair::Render()
{
	if (!m_pModelCom || !m_pTHV_TextureCom || !m_pDAO_TextureCom)
		return S_OK;

	if (FAILED(Bind_ShaderResources())) {
		return E_FAIL;
	}

	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; i++)
	{
		if (FAILED(m_pModelCom->Bind_BoneMatrices(i, m_pShaderCom, "g_BoneMatrices"))) {
			return E_FAIL;
		}

	/*	if (FAILED(m_pModelCom->Bind_Material(i, m_pShaderCom, "g_DiffuseTexture", aiTextureType_DIFFUSE, 0))) {
			return E_FAIL;
		}
		if (FAILED(m_pModelCom->Bind_Material(i, m_pShaderCom, "g_NormalTexture", aiTextureType_NORMALS, 0))) {
			return E_FAIL;
		}*/

		if (FAILED(m_pShaderCom->Begin(4))) {
			return E_FAIL;
		}

		if (FAILED(m_pModelCom->Render(i))) {
			return E_FAIL;
		}
	}

	return S_OK;
}

HRESULT CHair::Ready_Components()
{
	if (m_strModelPrototypeTag != L"")
	{
		/* Com_Model */
		if (FAILED(__super::Add_Asset_Component(g_iStaticLevel, m_strModelPrototypeTag,
			reinterpret_cast<CComponent**>(&m_pModelCom))))
			return E_FAIL;

	}

	return S_OK;
}

HRESULT CHair::Bind_ShaderResources()
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

	if (FAILED(m_pDAO_TextureCom->Bind_ShaderResource(m_pShaderCom, "g_DAOTexture", 0))) {
		return E_FAIL;
	}
	if (FAILED(m_pTHV_TextureCom->Bind_ShaderResource(m_pShaderCom, "g_THVTexture", 0))) {
		return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Bind_RawValue("g_RootColor", &m_vRootColor, sizeof(_float3)))){
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Bind_RawValue("g_TipColor", &m_vTipColor, sizeof(_float3)))) {
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Bind_RawValue("g_DyeColor", &m_vDyeColor, sizeof(_float3)))) {
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Bind_RawValue("g_DyeOpacity", &m_fDyeOpacity, sizeof(_float)))) {
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Bind_RawValue("g_HairRoughness", &m_fHairRoughness, sizeof(_float)))) {
		return E_FAIL;
	}

	
	return S_OK;
}
CHair* CHair::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CHair* pInstance = new CHair(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CHair");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

CGameObject* CHair::Clone(void* pArg, CGameObject* pOwner)
{
	CHair* pInstance = new CHair(*this);
	pInstance->m_pOwner = pOwner;
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CHair");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

void CHair::Free()
{
	__super::Free();
}

void CHair::Describe_Entity()
{
	if (!m_pModelCom )
		return;

	/*for (_uint i = 0; i < dynamic_cast<CTexture*>(m_pTextureCom)->Get_TextureDesc(); i++)
	{
		_char label[128];

		sprintf_s(label, "%s%d","Mask",i);

		if (GUI::ImageButton(label, dynamic_cast<CTexture*>(m_pTextureCom)->Get_SRV(i), ImVec2(50, 50)))
		{
			iTextureIndex = i;
		}

		if (i % dynamic_cast<CTexture*>(m_pTextureCom)->Get_TextureDesc()   != dynamic_cast<CTexture*>(m_pTextureCom)->Get_TextureDesc()-1)
			GUI::SameLine();
	}*/

	GUI::DragFloat3("RootColor", (_float*)&m_vRootColor, 0.1f);
	GUI::DragFloat3("TipColor", (_float*)&m_vTipColor, 0.1f);
	GUI::DragFloat3("DyeColor", (_float*)&m_vDyeColor, 0.1f);
	GUI::DragFloat("DyeOpacity", &m_fDyeOpacity, 0.1f);
	GUI::DragFloat("HairRoughness", &m_fHairRoughness, 0.1f);

}
