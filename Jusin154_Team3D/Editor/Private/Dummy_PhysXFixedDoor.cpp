#include "pch.h"
#include "Dummy_PhysXFixedDoor.h"

#include "GameInstance.h"


CDummy_PhysXFixedDoor::CDummy_PhysXFixedDoor(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CPartObject{ pDevice, pContext }
{
}

CDummy_PhysXFixedDoor::CDummy_PhysXFixedDoor(const CDummy_PhysXFixedDoor& rhs)
	: CPartObject(rhs)
{
}

HRESULT CDummy_PhysXFixedDoor::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CDummy_PhysXFixedDoor::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg))) {
		return E_FAIL;
	}

	if (FAILED(Ready_Components(pArg))) {
		return E_FAIL;
	}

	return S_OK;
}

void CDummy_PhysXFixedDoor::Priority_Update(_float fTimeDelta)
{

}

void CDummy_PhysXFixedDoor::Update(_float fTimeDelta)
{
	XMStoreFloat4x4(&m_CombinedWorldMatrix,
		XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrixPtr()) * XMLoadFloat4x4(m_pParentTransformCom->Get_WorldMatrixPtr()));
}

void CDummy_PhysXFixedDoor::Late_Update(_float fTimeDelta)
{
	if (m_pGameInstance->isIn_WorldFrustum(Get_WorldPostion(), m_pTransformCom->Get_Radius())) {
		m_pGameInstance->Add_RenderGroup(RENDER::BLEND, this);
	}
}

HRESULT CDummy_PhysXFixedDoor::Render()
{
	_vector vRPY = m_pTransformCom->Get_RollPitchYawVector();

	//GUI::Begin("FixedDoor");
	//m_pTransformCom->Describe_Entity();
	//GUI::Text("Current Roll : %f", XMConvertToDegrees(XMVectorGetX(vRPY)));
	//GUI::Text("Current Pitch : %f", XMConvertToDegrees(XMVectorGetY(vRPY)));
	//GUI::Text("Current Yaw : %f", XMConvertToDegrees(XMVectorGetZ(vRPY)));
	//GUI::End();
#ifdef _DEBUG
	if (FAILED(m_pRigidBody->Render())) {
		return E_FAIL;
	}
#endif // _DEBUG


	return S_OK;
}

PSX::PxRigidDynamic* CDummy_PhysXFixedDoor::Get_Actor()
{
	return m_pRigidBody->Get_Actor();
}

HRESULT CDummy_PhysXFixedDoor::Ready_Components(void* pArg)
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
		if (FAILED(Add_Asset_Component(g_iStaticLevel, TEXT("PHYSX_DYNAMIC_DOOR"), (CComponent**)&m_pRigidBody, &Desc))) {
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

HRESULT CDummy_PhysXFixedDoor::Bind_ShaderResources()
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

CDummy_PhysXFixedDoor* CDummy_PhysXFixedDoor::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CDummy_PhysXFixedDoor* pInstance = new CDummy_PhysXFixedDoor(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CDummy_PhysXFixedDoor");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

CGameObject* CDummy_PhysXFixedDoor::Clone(void* pArg, CGameObject* pOwner)
{
	CDummy_PhysXFixedDoor* pInstance = new CDummy_PhysXFixedDoor(*this);
	pInstance->m_pOwner = pOwner;
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CDummy_PhysXFixedDoor");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

void CDummy_PhysXFixedDoor::Free()
{
	__super::Free();

	SAFE_RELEASE(m_pRigidBody);
	SAFE_RELEASE(m_pShaderCom);
	SAFE_RELEASE(m_pModelCom);
}

void CDummy_PhysXFixedDoor::Describe_Entity()
{
}
