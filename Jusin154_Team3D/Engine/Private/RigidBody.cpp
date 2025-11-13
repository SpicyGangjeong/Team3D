#include "pch.h"
#include "RigidBody.h"
#include "GameInstance.h"
#include "GameObject.h"

CRigidBody::CRigidBody(ID3D11Device* pDevice, ID3D11DeviceContext* pContext):
	CComponent(pDevice, pContext)
{
}

CRigidBody::CRigidBody(const CRigidBody& rhs) :
	CComponent(rhs),
	m_eActorType(rhs.m_eActorType),
	m_vhalfGeometryInfo(rhs.m_vhalfGeometryInfo),
	m_wstrMeshKey(rhs.m_wstrMeshKey),
	m_vMatInfo(rhs.m_vMatInfo),
	m_ePxRigidBodyFlags(rhs.m_ePxRigidBodyFlags),
	m_ePxShapeFlags(rhs.m_ePxShapeFlags),
	m_eMatType(rhs.m_eMatType),
	m_fContactOffset(rhs.m_fContactOffset)
{
}
#ifdef _DEBUG
HRESULT CRigidBody::Render()
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

HRESULT CRigidBody::Initialize_Prototype(RIGIDBODY_PROTOTYPEDESC& Desc)
{
	m_eActorType		= Desc.eType;
	m_ePxRigidBodyFlags = Desc.ePxRigidBodyFlags;
	m_ePxShapeFlags		= Desc.ePxShapeFlags;
	m_eMatType			= Desc.ePxMaterialTypes;
	m_vMatInfo			= Desc.vMatInfo;
	m_fContactOffset	= Desc.fContactOffset;
	m_vhalfGeometryInfo = Desc.vhalfGeometryInfo;
	m_fDensity			= Desc.fDensity;
#ifdef _DEBUG
	if (FAILED(Add_DebugShape())) {
		return E_FAIL;
	}
#endif // _DEBUG

	return S_OK;
}

HRESULT CRigidBody::Initialize(void* pArg)
{
	RIGIDBODY_DESC* pDesc = (RIGIDBODY_DESC*)pArg;

	if (FAILED(__super::Initialize(pArg))) {
		return E_FAIL;
	}
	
	m_pTransform = m_pOwner->Get_Component<CTransform>();
	SAFE_ADDREF(m_pTransform);

	switch (m_eActorType)
	{
	case Engine::ACTOR::BOX:
	case Engine::ACTOR::CAPSULE:
	case Engine::ACTOR::SPHERE:
		m_pRigidBody = m_pGameInstance->Add_DynamicActor(*this);
		break;
	case Engine::ACTOR::PLANE:
		break;
	case Engine::ACTOR::TRIANGLEMESH:
		m_pRigidBody = m_pGameInstance->Add_StaticActor(*this);
		break;
	case Engine::ACTOR::HEIGHTFIELD:
		break;
	case Engine::ACTOR::END:
		break;
	default:
		break;
	}
#ifdef _DEBUG
	Add_DebugShape();
#endif // _DEBUG

	return S_OK;
}

#ifdef _DEBUG
HRESULT CRigidBody::Add_DebugShape()
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
	case ACTOR::PLANE:
	case ACTOR::TRIANGLEMESH:
	case ACTOR::HEIGHTFIELD:
		break;
	default:
		break;
	}
	return S_OK;
}
#endif // _DEBUG

CRigidBody* CRigidBody::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, RIGIDBODY_PROTOTYPEDESC& Desc)
{
	CRigidBody* pInstance = new CRigidBody(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(Desc))) {
		MSG_BOX("CRigidBody Prototype Created Failed");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

CComponent* CRigidBody::Clone(void* pArg, CGameObject* pOwner)
{
	CRigidBody* pInstance = new CRigidBody(*this);
	pInstance->m_pOwner = pOwner;
	if (FAILED(pInstance->Initialize(pArg))) {
		MSG_BOX("CRigidBody Prototype Created Failed");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

void CRigidBody::Free()
{
	__super::Free();

	SAFE_RELEASE(m_pTransform);
}

void CRigidBody::Describe_Entity()
{
}
