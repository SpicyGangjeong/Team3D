#include "pch.h"
#include "RigidBody_Dynamic.h"
#include "GameInstance.h"

CRigidBody_Dynamic::CRigidBody_Dynamic(ID3D11Device* pDevice, ID3D11DeviceContext* pContext):
    CRigidBody(pDevice, pContext)
{
}

CRigidBody_Dynamic::CRigidBody_Dynamic(const CRigidBody_Dynamic& rhs) :
    CRigidBody(rhs),
    m_vMatInfo(rhs.m_vMatInfo),
    m_vhalfGeometryInfo(rhs.m_vhalfGeometryInfo)
{
}

#ifdef _DEBUG
HRESULT CRigidBody_Dynamic::Render()
{
	_matrix  WorldMatrix = m_pTransform->Get_XMWorldMatrix();
	_matrix ViewMatrix = m_pGameInstance->Get_Transform_Matrix(D3DTS::VIEW);
	_matrix ProjMatrix = m_pGameInstance->Get_Transform_Matrix(D3DTS::PROJ);
	_vector vColor = CMyTools::ColorRGB_A_HEXtoVECTOR(0x2fc48000, 1.f);

	switch (m_eActorType)
	{
	case ACTOR::BOX:
	{
		m_pMainShape->Draw(WorldMatrix, ViewMatrix, ProjMatrix, vColor, nullptr, true);
	} break;
	case ACTOR::CAPSULE:
	{
		m_pMainShape->Draw(WorldMatrix, ViewMatrix, ProjMatrix, vColor, nullptr, true);
		_matrix WorldUp = XMMatrixTranslationFromVector(m_pTransform->Get_State(STATE::UP) * m_vhalfGeometryInfo.y);
		_matrix WorldDown = XMMatrixTranslationFromVector(m_pTransform->Get_State(STATE::UP) * -m_vhalfGeometryInfo.y);
		m_pSubShape->Draw(WorldUp * WorldMatrix, ViewMatrix, ProjMatrix, vColor, nullptr, true);
		m_pSubShape->Draw(WorldDown * WorldMatrix, ViewMatrix, ProjMatrix, vColor, nullptr, true);
	} break;
	case ACTOR::SPHERE:
	{
		m_pMainShape->Draw(WorldMatrix, ViewMatrix, ProjMatrix, vColor, nullptr, true);
	} break;
	default:
		break;
	}
	return S_OK;
}
#endif // _DEBUG
void CRigidBody_Dynamic::Add_Force(_fvector vForce, PSX::PxForceMode::Enum eType)
{
	PSX::PxVec3 vPxForce = {};
	XMStoreFloat3((_float3*)&vPxForce, vForce);
	m_pRigidBody->addForce(vPxForce, eType);
}

void CRigidBody_Dynamic::Add_Torque(_fvector vDirection, PSX::PxForceMode::Enum eType)
{
	PSX::PxVec3 vPxTorque = {};
	XMStoreFloat3((_float3*)&vPxTorque, vDirection);
	m_pRigidBody->addForce(vPxTorque, eType);
}

void CRigidBody_Dynamic::Set_Kinematic(_bool bKinematic)
{
	m_pRigidBody->setRigidBodyFlag(PSX::PxRigidBodyFlag::eKINEMATIC, bKinematic);
}

HRESULT CRigidBody_Dynamic::Initialize_Prototype(RIGIDBODY_PROTOTYPE_DYNAMIC_DESC& Desc)
{
	m_eActorType		= Desc.eType;
	m_ePxRigidBodyFlags = Desc.ePxRigidBodyFlags;
	m_ePxShapeFlags		= Desc.ePxShapeFlags;
	m_eMatType			= Desc.ePxMaterialTypes;
	m_vMatInfo			= Desc.vMatInfo;
	m_fContactOffset	= Desc.fContactOffset;
	m_vhalfGeometryInfo = Desc.vhalfGeometryInfo;
	m_fDensity			= Desc.fDensity;
	m_vMassCenter		= Desc.vMassCenter;

	return S_OK;
}

HRESULT CRigidBody_Dynamic::Initialize(void* pArg)
{
	m_tagData.eKind = PHYSX_KIND::BODY_DYNAMIC;
	if (FAILED(__super::Initialize(pArg))) {
		return E_FAIL;
	}

#ifdef _DEBUG
	if (FAILED(Add_DebugShape())) {
		return E_FAIL;
	}
#endif // _DEBUG
	m_tagData.pOwner = m_pOwner;
	XMStoreFloat4x4(&m_tagData.BeforeMatrix, m_pTransform->Get_XMWorldMatrix());
	m_tagData.pBody = this;
	m_pRigidBody = m_pGameInstance->Add_DynamicActor(*this);
	m_pRigidBody->userData = &m_tagData;

	return S_OK;
}

#ifdef _DEBUG
HRESULT CRigidBody_Dynamic::Add_DebugShape()
{
	switch (m_eActorType)
	{
	case ACTOR::BOX:
		m_pMainShape = (GeometricPrimitive::CreateBox(m_pContext, m_vhalfGeometryInfo, false, false));
		break;
	case ACTOR::CAPSULE:
		m_pSubShape = (GeometricPrimitive::CreateSphere(m_pContext, m_vhalfGeometryInfo.x, 10, false, false));
		m_pMainShape = (GeometricPrimitive::CreateCylinder(m_pContext, m_vhalfGeometryInfo.y, m_vhalfGeometryInfo.x, 10, false));
		break;
	case ACTOR::SPHERE:
		m_pMainShape = (GeometricPrimitive::CreateSphere(m_pContext, m_vhalfGeometryInfo.x, 10, false, false));
		break;
	default:
		break;
	}
	return S_OK;
}
#endif // _DEBUG
CRigidBody_Dynamic* CRigidBody_Dynamic::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, RIGIDBODY_PROTOTYPE_DYNAMIC_DESC& Desc)
{
	CRigidBody_Dynamic* pInstance = new CRigidBody_Dynamic(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(Desc))) {
		MSG_BOX("CRigidBody_Dynamic Prototype Created Failed");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

CComponent* CRigidBody_Dynamic::Clone(void* pArg, CGameObject* pOwner)
{
	CRigidBody_Dynamic* pInstance = new CRigidBody_Dynamic(*this);
	pInstance->m_pOwner = pOwner;
	if (FAILED(pInstance->Initialize(pArg))) {
		MSG_BOX("CRigidBody_Dynamic Prototype Created Failed");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

void CRigidBody_Dynamic::Free()
{
	__super::Free();
	if (nullptr != m_pRigidBody) {
		m_pRigidBody = nullptr;
	}
}

void CRigidBody_Dynamic::Describe_Entity()
{
}
