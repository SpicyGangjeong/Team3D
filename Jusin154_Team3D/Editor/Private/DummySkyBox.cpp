#include "pch.h"
#include "DummySkyBox.h"

#include "GameInstance.h"
#include "DebugCamera.h"
#include "Layer.h"
#include "Camera_Gaze.h"

CDummySkyBox::CDummySkyBox(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject{ pDevice, pContext }
{
}

CDummySkyBox::CDummySkyBox(const CDummySkyBox& rhs)
	: CGameObject(rhs)
{
}

HRESULT CDummySkyBox::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CDummySkyBox::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components(pArg)))
		return E_FAIL;


	return S_OK;
}

void CDummySkyBox::Priority_Update(_float fTimeDelta)
{

}

void CDummySkyBox::Update(_float fTimeDelta)
{
	m_pTransformCom->Set_State(STATE::POSITION, m_pGameInstance->Get_CamXMPosition());
}

void CDummySkyBox::Late_Update(_float fTimeDelta)
{
	if (m_pGameInstance->isIn_WorldFrustum(Get_WorldPostion(), m_pTransformCom->Get_Radius())) {
		m_pGameInstance->Add_RenderGroup(RENDER::PRIORITY, this);
	}
}

HRESULT CDummySkyBox::Render()
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

HRESULT CDummySkyBox::Ready_Components(void* pArg)
{
	if (FAILED(__super::Ready_Components(pArg))) {
		return E_FAIL;
	}

	/* Com_Model */
	if (FAILED(__super::Add_Asset_Component(g_iStaticLevel, TEXT("Prototype_Component_SkyboxModel"),
		reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	/* Com_Shader */
	if (FAILED(__super::Add_Asset_Component(g_iStaticLevel, FX_MESH,
		reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CDummySkyBox::Bind_ShaderResources()
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

CDummySkyBox* CDummySkyBox::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CDummySkyBox* pInstance = new CDummySkyBox(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CDummySkyBox");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

CGameObject* CDummySkyBox::Clone(void* pArg, CGameObject* pOwner)
{
	CDummySkyBox* pInstance = new CDummySkyBox(*this);
	pInstance->m_pOwner = pOwner;
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CDummySkyBox");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

void CDummySkyBox::Free()
{
	__super::Free();

	SAFE_RELEASE(m_pModelCom);
	SAFE_RELEASE(m_pShaderCom);
}

void CDummySkyBox::Describe_Entity()
{
}
