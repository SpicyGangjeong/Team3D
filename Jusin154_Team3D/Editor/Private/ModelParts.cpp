#include "pch.h"
#include "Body.h"

#include "GameInstance.h"

CModelParts::CModelParts(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
}

CModelParts::CModelParts(const CModelParts& Prototype)
	: CGameObject(Prototype)
{
}

HRESULT CModelParts::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CModelParts::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;


	if (FAILED(Ready_Components()))
		return E_FAIL;

	return S_OK;
}

void CModelParts::Priority_Update(_float fTimeDelta)
{
}

void CModelParts::Update(_float fTimeDelta)
{
}

void CModelParts::Late_Update(_float fTimeDelta)
{
}

HRESULT CModelParts::Render()
{
	return S_OK;
}

HRESULT CModelParts::Ready_Components()
{
	__super::Ready_Components(nullptr);

	/* Com_Shader */
	if (FAILED(__super::Add_Asset_Component(g_iStaticLevel, FX_ANIMMESH,
		reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CModelParts::Bind_ShaderResources()
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
	return S_OK;
}

void CModelParts::Free()
{
	__super::Free();

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);
}

void CModelParts::Describe_Entity()
{
}
