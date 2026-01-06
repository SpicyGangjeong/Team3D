#include "pch.h"
#include "Dummy_Cube.h"

#include "GameInstance.h"


CDummy_Cube::CDummy_Cube(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject{ pDevice, pContext }
{
}

CDummy_Cube::CDummy_Cube(const CDummy_Cube& rhs)
	: CGameObject(rhs)
{
}

HRESULT CDummy_Cube::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CDummy_Cube::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components(pArg)))
		return E_FAIL;

	m_pTransformCom->Set_State(STATE::POSITION, XMVectorSet(0.f, 0.f, 195.f, 1.f));

	return S_OK;
}

void CDummy_Cube::Priority_Update(_float fTimeDelta)
{

}

void CDummy_Cube::Update(_float fTimeDelta)
{
}

void CDummy_Cube::Late_Update(_float fTimeDelta)
{
	if (m_pGameInstance->IsIn_WorldFrustum(Get_WorldPostion(), m_pTransformCom->Get_Radius())) {
		m_pGameInstance->Add_RenderGroup(RENDER::NONBLEND, this);
	}
}

HRESULT CDummy_Cube::Render()
{
	if (FAILED(Bind_ShaderResources())) {
		return E_FAIL;
	}


	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; i++)
	{
		if (FAILED(m_pModelCom->Bind_Material(i, m_pShaderCom))) {
			return E_FAIL;
		}

		if (FAILED(m_pShaderCom->Begin(ENUM_CLASS(SHADER_PASS_MESH::SKYBOX)))) {
			return E_FAIL;
		}

		if (FAILED(m_pModelCom->Render(i))) {
			return E_FAIL;
		}
	}


	return S_OK;
}

HRESULT CDummy_Cube::Ready_Components(void* pArg)
{
	if (FAILED(__super::Ready_Components(pArg))) {
		return E_FAIL;
	}

	///* Com_Model */
	//if (FAILED(__super::Add_Asset_Component(g_iStaticLevel, TEXT("Prototype_Component_GoblinBody_Model"),
	//	reinterpret_cast<CComponent**>(&m_pModelCom))))
	//	return E_FAIL;

	/* Com_Model */
	if (FAILED(__super::Add_Asset_Component(g_iStaticLevel, TEXT("Desc_Box"),
		reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	/* Com_Shader */
	if (FAILED(__super::Add_Asset_Component(g_iStaticLevel, FX_MESH,
		reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CDummy_Cube::Bind_ShaderResources()
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

CDummy_Cube* CDummy_Cube::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CDummy_Cube* pInstance = new CDummy_Cube(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CDummy_Cube");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

CGameObject* CDummy_Cube::Clone(void* pArg, CGameObject* pOwner)
{
	CDummy_Cube* pInstance = new CDummy_Cube(*this);
	pInstance->m_pOwner = pOwner;
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CDummy_Cube");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

void CDummy_Cube::Free()
{
	__super::Free();

	SAFE_RELEASE(m_pModelCom);
	SAFE_RELEASE(m_pShaderCom);
}

void CDummy_Cube::Describe_Entity()
{
}
