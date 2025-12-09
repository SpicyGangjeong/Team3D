#include "pch.h"
#include "Unified.h"

#include "GameInstance.h"

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
	//Describe_Entity();
}

void CUnified::Late_Update(_float fTimeDelta)
{
	if (m_isVisible)
		m_pGameInstance->Add_RenderGroup(RENDER::NONBLEND, this);
}

HRESULT CUnified::Render()
{
	if (FAILED(Bind_ShaderResources())) {
		return E_FAIL;
	}

	for (_uint i = 0; i < m_iNumMesh; ++i)
	{
		if (FAILED(m_pModelCom->Bind_Material(i, m_pShaderCom))) {
			return E_FAIL;
		}

		if (FAILED(m_pShaderCom->Bind_RawValue("g_fUsingSurfaceParams", &m_fUsingSurfaceParams, sizeof(_float)))) {
			return E_FAIL;
		}

		if (FAILED(m_pShaderCom->Bind_RawValue("g_vSurfaceColor", &m_vSurfaceColor, sizeof(_float4))))
			return E_FAIL;

		if (FAILED(m_pShaderCom->Begin(ENUM_CLASS(SHADER_PASS_MESH::NONMRO)))) {
			return E_FAIL;
		}

		if (FAILED(m_pModelCom->Render(i))) {
			return E_FAIL;
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

	m_iNumMesh = m_pModelCom->Get_NumMeshes();

	m_pTransformCom->Set_State(STATE::POSITION, XMVectorSet(-291.2f, 17.2f, -474.7f, 1.f));
	m_fUsingSurfaceParams = 2.f / 27.f;
	m_vSurfaceColor = _float4(1.f, 1.f, 1.f, 1.f);

	return S_OK;
}

HRESULT CUnified::Ready_Components(void* pArg)
{
	if (FAILED(__super::Ready_Components(pArg))) {
		return E_FAIL;
	}

	/* Com_Model */
	if (FAILED(__super::Add_Asset_Component(g_iStaticLevel, TEXT("Prototype_GameObject_SM_HW_HogwartsShell_B"),
		reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

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
	SAFE_RELEASE(m_pModelCom);
}
#ifdef _DEBUG
void CUnified::Describe_Entity()
{
	GUI::Begin("Unified");
	GUI::Checkbox("Visible ON / OFF", &m_isVisible);

	GUI::End();
}
#endif // DEBUG


