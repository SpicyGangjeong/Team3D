#include "pch.h"
#include "Dummy_PhysXHitBox.h"

#include "GameInstance.h"


CDummy_PhysXHitBox::CDummy_PhysXHitBox(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject{ pDevice, pContext }
{
}

CDummy_PhysXHitBox::CDummy_PhysXHitBox(const CDummy_PhysXHitBox& rhs)
	: CGameObject(rhs)
{
}

HRESULT CDummy_PhysXHitBox::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CDummy_PhysXHitBox::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg))) {
		return E_FAIL;
	}

	if (FAILED(Ready_Components(pArg))) {
		return E_FAIL;
	}

	return S_OK;
}

void CDummy_PhysXHitBox::Priority_Update(_float fTimeDelta)
{

}

void CDummy_PhysXHitBox::Update(_float fTimeDelta)
{

}

void CDummy_PhysXHitBox::Late_Update(_float fTimeDelta)
{
	if (m_pGameInstance->IsIn_WorldFrustum(Get_WorldPostion(), m_pTransformCom->Get_Radius())) {
		m_pGameInstance->Add_RenderGroup(RENDER::BLEND, this);
	}
}

HRESULT CDummy_PhysXHitBox::Render()
{
	//if (FAILED(Bind_ShaderResources())) {
	//	return E_FAIL;
	//}

	//if (FAILED(m_pModelCom->Bind_Material(0, m_pShaderCom))) {
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

HRESULT CDummy_PhysXHitBox::Ready_Components(void* pArg)
{
	if (FAILED(__super::Ready_Components(pArg))) {
		return E_FAIL;
	}
	PHYSXDUMMY_DESC* pPhysXDummyDesc = static_cast<PHYSXDUMMY_DESC*>(pArg);

	m_pTransformCom->Set_State(STATE::POSITION, XMVectorSetW(XMLoadFloat3(&pPhysXDummyDesc->vPos), 1.f));
	m_pTransformCom->Rotation(pPhysXDummyDesc->vRotRPY.x, pPhysXDummyDesc->vRotRPY.y, pPhysXDummyDesc->vRotRPY.z);


	{ // RIGID_BODY
		CRigidBody_Dynamic::RIGIDBODY_DYNAMIC_DESC Desc{};
		Desc.iSubKind = pPhysXDummyDesc->iSubKind;
		if (FAILED(Add_Asset_Component(g_iStaticLevel, TEXT("PHYSX_DYNAMIC_BOX"), (CComponent**)&m_pRigidBody, &Desc))) {
			return E_FAIL;
		}
	}

	/* Com_Shader */
	if (FAILED(__super::Add_Asset_Component(g_iStaticLevel, FX_MESH,
		reinterpret_cast<CComponent**>(&m_pShaderCom)))) {
		return E_FAIL;
	}

	if (FAILED(__super::Add_Asset_Component(g_iStaticLevel, TEXT("Prototype_Component_Box"), (CComponent**)&m_pModelCom))) {
		return E_FAIL;
	}


	return S_OK;
}

HRESULT CDummy_PhysXHitBox::Bind_ShaderResources()
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

CDummy_PhysXHitBox* CDummy_PhysXHitBox::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CDummy_PhysXHitBox* pInstance = new CDummy_PhysXHitBox(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CDummy_PhysXHitBox");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

CGameObject* CDummy_PhysXHitBox::Clone(void* pArg, CGameObject* pOwner)
{
	CDummy_PhysXHitBox* pInstance = new CDummy_PhysXHitBox(*this);
	pInstance->m_pOwner = pOwner;
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CDummy_PhysXHitBox");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

void CDummy_PhysXHitBox::Free()
{
	if (nullptr != m_pRigidBody) {
		m_pGameInstance->Release_Actor(*m_pRigidBody->Get_Actor());
	}

	__super::Free();

	SAFE_RELEASE(m_pRigidBody);
	SAFE_RELEASE(m_pShaderCom);
	SAFE_RELEASE(m_pModelCom);
}

void CDummy_PhysXHitBox::Describe_Entity()
{
}
