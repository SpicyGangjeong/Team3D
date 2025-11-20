#include "pch.h"
#include "Head.h"

#include "GameInstance.h"
#include "RootModelPart.h"

CHead::CHead(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CModelParts(pDevice, pContext)
{
}

CHead::CHead(const CHead& Prototype)
	: CModelParts(Prototype)
{
}

HRESULT CHead::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CHead::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;


	if (FAILED(Ready_Components()))
		return E_FAIL;

	return S_OK;
}

void CHead::Priority_Update(_float fTimeDelta)
{

}

void CHead::Update(_float fTimeDelta)
{
	__super::Update(fTimeDelta);
}

void CHead::Late_Update(_float fTimeDelta)
{
	if (m_pGameInstance->isIn_WorldFrustum(Get_WorldPostion(), m_pTransformCom->Get_Radius())) {
		m_pGameInstance->Add_RenderGroup(RENDER::BLEND, this);
	}
}

HRESULT CHead::Render()
{
	if(!m_pModelCom)
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

		if (FAILED(m_pModelCom->Bind_Material(i, m_pShaderCom))) {
			return E_FAIL;
		}

		if (i == m_HeadIndex)
		{

			if (FAILED(m_pShaderCom->Begin(5))) {
				return E_FAIL;
			}

			if (FAILED(m_pModelCom->Render(i))) {
				return E_FAIL;
			}
		}
		else
		{
			if (FAILED(m_pShaderCom->Begin(ENUM_CLASS(SHADER_PASS_ANIM::DEFAULT)))) {
				return E_FAIL;
			}

			if (FAILED(m_pModelCom->Render(i))) {
				return E_FAIL;
			}
		}

	}

	return S_OK;
}

HRESULT CHead::Bind_ShaderResources()
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


	GUI::DragFloat4("COLOR", (_float*)&m_vColor, 0.1f);

	if (FAILED(m_pShaderCom->Bind_RawValue("g_TestColor", &m_vColor, sizeof(_float4)))) {
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CHead::Ready_Components()
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

CHead* CHead::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CHead* pInstance = new CHead(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CHead");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

CGameObject* CHead::Clone(void* pArg, CGameObject* pOwner)
{
	CHead* pInstance = new CHead(*this);
	pInstance->m_pOwner = pOwner;
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CHead");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

void CHead::Free()
{
	__super::Free();

}

void CHead::Describe_Entity()
{
}
