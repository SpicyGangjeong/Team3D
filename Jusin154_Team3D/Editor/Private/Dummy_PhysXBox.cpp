#include "pch.h"
#include "Dummy_PhysXBox.h"

#include "GameInstance.h"


CDummy_PhysXBox::CDummy_PhysXBox(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject{ pDevice, pContext }
{
}

CDummy_PhysXBox::CDummy_PhysXBox(const CDummy_PhysXBox& rhs)
	: CGameObject(rhs)
{
}

HRESULT CDummy_PhysXBox::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CDummy_PhysXBox::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg))) {
		return E_FAIL;
	}

	if (FAILED(Ready_Components(pArg))) {
		return E_FAIL;
	}

	return S_OK;
}

void CDummy_PhysXBox::Priority_Update(_float fTimeDelta)
{

}

void CDummy_PhysXBox::Update(_float fTimeDelta)
{
}

void CDummy_PhysXBox::Late_Update(_float fTimeDelta)
{
	if (m_pGameInstance->isIn_WorldFrustum(Get_WorldPostion(), m_pTransformCom->Get_Radius())) {
		m_pGameInstance->Add_RenderGroup(RENDER::BLEND, this);
	}
}

HRESULT CDummy_PhysXBox::Render()
{
	//if (FAILED(Bind_ShaderResources())) {
	//	return E_FAIL;
	//}

	//if (FAILED(m_pModelCom->Bind_Material(0, m_pShaderCom, "g_DiffuseTexture", aiTextureType_DIFFUSE, 0))) {
	//	return E_FAIL;
	//}
	//if (FAILED(m_pShaderCom->Begin(ENUM_CLASS(SHADER_PASS_MESH::DEFAULT)))) {
	//	return E_FAIL;
	//}

	//if (FAILED(m_pModelCom->Render(0))) {
	//	return E_FAIL;
	//}
#ifdef _DEBUG
	if (FAILED(m_pRigidBody->Render())) {
		return E_FAIL;
	}
#endif // _DEBUG


	return S_OK;
}

HRESULT CDummy_PhysXBox::Ready_Components(void* pArg)
{
	if (FAILED(__super::Ready_Components(pArg))) {
		return E_FAIL;
	}
	BOXSTARTPOS_DESC* pBox = static_cast<BOXSTARTPOS_DESC*>(pArg);

	m_pTransformCom->Set_State(STATE::POSITION, XMVectorSetW(XMLoadFloat3(&pBox->vPos), 1.f));
	m_pTransformCom->Rotation(pBox->vRotRPY.x, pBox->vRotRPY.y, pBox->vRotRPY.z);


	{ // RIGID_BODY
		CRigidBody_Dynamic::RIGIDBODY_DYNAMIC_DESC Desc{};

		if (FAILED(Add_Asset_Component(g_iStaticLevel, TEXT("PHYSX_DYNAMIC_BOX"), (CComponent**)&m_pRigidBody, &Desc))) {
			return E_FAIL;
		}
	}

	/* Com_Shader */
	if (FAILED(__super::Add_Asset_Component(g_iStaticLevel, FX_MESH,
		reinterpret_cast<CComponent**>(&m_pShaderCom)))){
		return E_FAIL;
	}

	if (FAILED(__super::Add_Asset_Component(g_iStaticLevel, TEXT("Prototype_Component_Box"), (CComponent**)&m_pModelCom))) {
		return E_FAIL;
	}


	return S_OK;
}

HRESULT CDummy_PhysXBox::Bind_ShaderResources()
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

CDummy_PhysXBox* CDummy_PhysXBox::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CDummy_PhysXBox* pInstance = new CDummy_PhysXBox(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CDummy_PhysXBox");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

CGameObject* CDummy_PhysXBox::Clone(void* pArg, CGameObject* pOwner)
{
	CDummy_PhysXBox* pInstance = new CDummy_PhysXBox(*this);
	pInstance->m_pOwner = pOwner;
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CDummy_PhysXBox");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

void CDummy_PhysXBox::Free()
{
	__super::Free();

	SAFE_RELEASE(m_pRigidBody);
	SAFE_RELEASE(m_pShaderCom);
	SAFE_RELEASE(m_pModelCom);
}

void CDummy_PhysXBox::Describe_Entity()
{
}
