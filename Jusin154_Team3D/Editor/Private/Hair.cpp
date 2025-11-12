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
	_float4 vPos;
	XMStoreFloat4(&vPos, Get_WorldPostion());
	m_pGameInstance->Add_RenderGroup(RENDER::BLEND, this, vPos, 20.f);
}

HRESULT CHair::Render()
{
	if (!m_pModelCom ||!m_pTextureCom)
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

		/*if (FAILED(m_pModelCom->Bind_Material(i, m_pShaderCom, "g_DiffuseTexture", aiTextureType_DIFFUSE, 0))) {
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

	if (FAILED(__super::Add_Asset_Component(g_iStaticLevel, TEXT("MaskTexture"),
		reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

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

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_MaskingTexture", iTextureIndex))) {
		return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Bind_RawValue("g_TestColor",&m_vColor,sizeof(_float4)))) {
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
		Safe_Release(pInstance);
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
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CHair::Free()
{
	__super::Free();

	SAFE_RELEASE(m_pTextureCom);
}

void CHair::Describe_Entity()
{
	if (!m_pModelCom )
		return;

	for (_uint i = 0; i < dynamic_cast<CTexture*>(m_pTextureCom)->Get_Size(); i++)
	{
		_char label[128];

		sprintf_s(label, "%s%d","Mask",i);

		if (GUI::ImageButton(label, dynamic_cast<CTexture*>(m_pTextureCom)->Get_SRV(i), ImVec2(50, 50)))
		{
			iTextureIndex = i;
		}

		if (i % dynamic_cast<CTexture*>(m_pTextureCom)->Get_Size()   != dynamic_cast<CTexture*>(m_pTextureCom)->Get_Size()-1)
			GUI::SameLine();
	}

	GUI::DragFloat4("COLOR", (_float*)&m_vColor, 0.1f);

}
