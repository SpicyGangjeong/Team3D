#include "pch.h"
#include "Land.h"

#include "GameInstance.h"

CLand::CLand(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject{ pDevice, pContext }
{
}

CLand::CLand(const CLand& rhs)
	: CGameObject(rhs)
{
}

void CLand::Priority_Update(_float fTimeDelta)
{
}

void CLand::Update(_float fTimeDelta)
{
}

void CLand::Late_Update(_float fTimeDelta)
{
	if (m_pGameInstance->IsIn_WorldFrustum(m_pTransformCom->Get_State(STATE::POSITION), m_fRaduis))
		m_pGameInstance->Add_RenderGroup(RENDER::NONBLEND, this);
}

HRESULT CLand::Render()
{
	if (FAILED(Bind_ShaderResources())) {
		return E_FAIL;
	}

	if (FAILED(m_pModelCom->Bind_Material(0, m_pShaderCom)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Begin(ENUM_CLASS(SHADER_PASS_MESH::LAND)))) {
		return E_FAIL;
	}

	if (FAILED(m_pModelCom->Render(0))) {
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CLand::Render_Shadow(SHADOW eType)
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix"))) {
		return E_FAIL;
	}
	if (FAILED(m_pGameInstance->Bind_Shadow_Resource(m_pShaderCom, "g_ViewMatrix", D3DTS::VIEW, eType))) {
		return E_FAIL;
	}
	if (FAILED(m_pGameInstance->Bind_Shadow_Resource(m_pShaderCom, "g_ProjMatrix", D3DTS::PROJ, eType))) {
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Begin(ENUM_CLASS(SHADER_PASS_MESH::SHADOW)))) {
		return E_FAIL;
	}

	if (FAILED(m_pModelCom->Render(0))) {
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CLand::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CLand::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components(pArg)))
		return E_FAIL;

	m_fRaduis = 350.f;

	return S_OK;
}

HRESULT CLand::Ready_Components(void* pArg)
{
	if (FAILED(__super::Ready_Components(pArg))) {
		return E_FAIL;
	}

	LAND_DESC* pDesc = static_cast<LAND_DESC*>(pArg);

	m_pTransformCom->Set_State(STATE::POSITION, XMVectorSetW(XMLoadFloat3(&pDesc->vPosition), 1.f));
	m_pTransformCom->Set_Scale(pDesc->vScale);

	/* Com_Model */
	if (FAILED(__super::Add_Asset_Component(g_iStaticLevel, pDesc->strModelComTag,
		reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	/* Com_Shader */
	if (FAILED(__super::Add_Asset_Component(g_iStaticLevel, FX_MESH,
		reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CLand::Bind_ShaderResources()
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

CLand* CLand::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CLand* pInstance = new CLand(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CLand");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

CGameObject* CLand::Clone(void* pArg, CGameObject* pOwner)
{
	CLand* pInstance = new CLand(*this);
	pInstance->m_pOwner = pOwner;
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CLand");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}
void CLand::Free()
{
	__super::Free();

	SAFE_RELEASE(m_pShaderCom);
	SAFE_RELEASE(m_pModelCom);
}
#ifdef _DEBUG
void CLand::Describe_Entity()
{

}
#endif // _DEBUG
