#include "pch.h"
#include "Dummy_PhysXDoorFrame.h"

#include "GameInstance.h"


CDummy_PhysXDoorFrame::CDummy_PhysXDoorFrame(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CPartObject{ pDevice, pContext }
{
}

CDummy_PhysXDoorFrame::CDummy_PhysXDoorFrame(const CDummy_PhysXDoorFrame& rhs)
	: CPartObject(rhs)
{
}

HRESULT CDummy_PhysXDoorFrame::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CDummy_PhysXDoorFrame::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg))) {
		return E_FAIL;
	}

	if (FAILED(Ready_Components(pArg))) {
		return E_FAIL;
	}

	return S_OK;
}

void CDummy_PhysXDoorFrame::Priority_Update(_float fTimeDelta)
{

}

void CDummy_PhysXDoorFrame::Update(_float fTimeDelta)
{
	XMStoreFloat4x4(&m_CombinedWorldMatrix,
		XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrixPtr()) * XMLoadFloat4x4(m_pParentTransformCom->Get_WorldMatrixPtr()));
}

void CDummy_PhysXDoorFrame::Late_Update(_float fTimeDelta)
{
	if (m_pGameInstance->IsIn_WorldFrustum(Get_WorldPostion(), m_pTransformCom->Get_Radius())) {
		m_pGameInstance->Add_RenderGroup(RENDER::BLEND, this);
	}
}

HRESULT CDummy_PhysXDoorFrame::Render()
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

PSX::PxRigidDynamic* CDummy_PhysXDoorFrame::Get_Actor()
{
	return m_pRigidBody->Get_Actor();
}

HRESULT CDummy_PhysXDoorFrame::Ready_Components(void* pArg)
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
		if (FAILED(Add_Asset_Component(g_iStaticLevel, TEXT("PHYSX_DYNAMIC_HEAVY_WALL"), (CComponent**)&m_pRigidBody, &Desc))) {
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

HRESULT CDummy_PhysXDoorFrame::Bind_ShaderResources()
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

CDummy_PhysXDoorFrame* CDummy_PhysXDoorFrame::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CDummy_PhysXDoorFrame* pInstance = new CDummy_PhysXDoorFrame(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CDummy_PhysXDoorFrame");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

CGameObject* CDummy_PhysXDoorFrame::Clone(void* pArg, CGameObject* pOwner)
{
	CDummy_PhysXDoorFrame* pInstance = new CDummy_PhysXDoorFrame(*this);
	pInstance->m_pOwner = pOwner;
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CDummy_PhysXDoorFrame");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

void CDummy_PhysXDoorFrame::Free()
{
	if (nullptr != m_pRigidBody) {
		m_pGameInstance->Release_Actor(*m_pRigidBody->Get_Actor());
	}

	__super::Free();

	SAFE_RELEASE(m_pRigidBody);
	SAFE_RELEASE(m_pShaderCom);
	SAFE_RELEASE(m_pModelCom);
}

void CDummy_PhysXDoorFrame::Describe_Entity()
{
}
