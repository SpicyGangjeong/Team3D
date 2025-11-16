#include "pch.h"
#include "Dummy_PhysXDoor.h"

#include "GameInstance.h"


CDummy_PhysXDoor::CDummy_PhysXDoor(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject{ pDevice, pContext }
{
}

CDummy_PhysXDoor::CDummy_PhysXDoor(const CDummy_PhysXDoor& rhs)
	: CGameObject(rhs)
{
}

HRESULT CDummy_PhysXDoor::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CDummy_PhysXDoor::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg))) {
		return E_FAIL;
	}

	if (FAILED(Ready_Components(pArg))) {
		return E_FAIL;
	}


	return S_OK;
}

void CDummy_PhysXDoor::Priority_Update(_float fTimeDelta)
{

}

void CDummy_PhysXDoor::Update(_float fTimeDelta)
{
}

void CDummy_PhysXDoor::Late_Update(_float fTimeDelta)
{
	if (m_pGameInstance->isIn_WorldFrustum(Get_WorldPostion(), m_pTransformCom->Get_Radius())) {
		m_pGameInstance->Add_RenderGroup(RENDER::BLEND, this);
	}
}

HRESULT CDummy_PhysXDoor::Render()
{
	_vector vRPY = m_pTransformCom->Get_RollPitchYawVector();
	//_float fCurrentRadian = XMVectorGetY(vRPY);

	//m_pTransformCom->Rotation(XMVectorSetY(vRPY, ClampRadian(fCurrentRadian)));

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
	GUI::Begin("Door");
	m_pTransformCom->Describe_Entity();
	GUI::Text("Current Roll : %f", XMConvertToDegrees(XMVectorGetX(vRPY)));
	GUI::Text("Current Pitch : %f", XMConvertToDegrees(XMVectorGetY(vRPY)));
	GUI::Text("Current Yaw : %f", XMConvertToDegrees(XMVectorGetZ(vRPY)));
	GUI::Text("Current Limit Left : %f", XMConvertToDegrees(m_vRadianYAngle.x));
	GUI::Text("Current Limit Right : %f", XMConvertToDegrees(m_vRadianYAngle.z));
	GUI::End();
#ifdef _DEBUG
	if (FAILED(m_pRigidBody->Render())) {
		return E_FAIL;
	}
#endif // _DEBUG


	return S_OK;
}

HRESULT CDummy_PhysXDoor::Ready_Components(void* pArg)
{
	if (FAILED(__super::Ready_Components(pArg))) {
		return E_FAIL;
	}
	PHYSXDUMMY_DESC* pPhysXDummyDesc = static_cast<PHYSXDUMMY_DESC*>(pArg);

	m_pTransformCom->Set_State(STATE::POSITION, XMVectorSetW(XMLoadFloat3(&pPhysXDummyDesc->vPos), 1.f));
	m_pTransformCom->Rotation(pPhysXDummyDesc->vRotRPY.x, pPhysXDummyDesc->vRotRPY.y, pPhysXDummyDesc->vRotRPY.z);

	m_vRadianYAngle.y = pPhysXDummyDesc->vRotRPY.z;
	m_vRadianYAngle.x = m_vRadianYAngle.y + XMConvertToRadians(-pPhysXDummyDesc->fAngleLimit);
	m_vRadianYAngle.z = m_vRadianYAngle.y + XMConvertToRadians(pPhysXDummyDesc->fAngleLimit);

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

HRESULT CDummy_PhysXDoor::Bind_ShaderResources()
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

_float CDummy_PhysXDoor::ClampRadian(_float fNewRadian)
{
	// 시작 라디안에서 부터 델타라디안을 구함
	_float fDeltaRadian = CMyTools::NormalizeRadian(fNewRadian - m_vRadianYAngle.y);
	if (fDeltaRadian < m_vRadianYAngle.x) {
		fDeltaRadian = m_vRadianYAngle.x;
	}
	else if (fDeltaRadian > m_vRadianYAngle.z) {
		fDeltaRadian = m_vRadianYAngle.z;
	}
	return CMyTools::NormalizeRadian(m_vRadianYAngle.y + fDeltaRadian);
}

CDummy_PhysXDoor* CDummy_PhysXDoor::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CDummy_PhysXDoor* pInstance = new CDummy_PhysXDoor(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CDummy_PhysXDoor");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

CGameObject* CDummy_PhysXDoor::Clone(void* pArg, CGameObject* pOwner)
{
	CDummy_PhysXDoor* pInstance = new CDummy_PhysXDoor(*this);
	pInstance->m_pOwner = pOwner;
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CDummy_PhysXDoor");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

void CDummy_PhysXDoor::Free()
{
	__super::Free();

	SAFE_RELEASE(m_pRigidBody);
	SAFE_RELEASE(m_pShaderCom);
	SAFE_RELEASE(m_pModelCom);
}

void CDummy_PhysXDoor::Describe_Entity()
{
}
