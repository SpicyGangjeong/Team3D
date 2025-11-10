#include "pch.h"
#include "RigidBody.h"
#include "GameInstance.h"
#include "GameObject.h"

CRigidBody::CRigidBody(ID3D11Device* pDevice, ID3D11DeviceContext* pContext):
	CComponent(pDevice, pContext)
{
}

CRigidBody::CRigidBody(const CRigidBody& rhs):
	CComponent(rhs),
	m_pMaterial(rhs.m_pMaterial),
	m_eActorType(rhs.m_eActorType),
	m_vhalfGeometryInfo(rhs.m_vhalfGeometryInfo),
	m_pShape(rhs.m_pShape),
	m_pMeshKey(rhs.m_pMeshKey),
	m_bExclusive(rhs.m_bExclusive),
	m_bKinematic(rhs.m_bKinematic)
{

}

HRESULT CRigidBody::Initialize_Prototype(RIGIDBODY_PROTOTYPEDESC& Desc)
{
	m_eActorType = Desc.eType;
	switch (m_eActorType)
	{
	case ACTOR::BOX:
	case ACTOR::CAPSULE:
	case ACTOR::SPHERE:
		m_pMaterial = m_pGameInstance->Get_Material(Desc.tRigidDynamicDesc.vMatInfo);
		m_vhalfGeometryInfo = Desc.tRigidDynamicDesc.vhalfGeometryInfo;
		m_bExclusive = Desc.tRigidDynamicDesc.bExclusive;
		m_pShape = m_pGameInstance->Create_Shape(m_eActorType, m_vhalfGeometryInfo, *m_pMaterial, m_bExclusive, Desc.tRigidDynamicDesc.ePxShapeFlag);
		if (nullptr == m_pShape) {
			assert(false);
			m_pMaterial->release();
			m_pMaterial = nullptr;
			return E_FAIL;
		}
		break;
	case ACTOR::PLANE:
		break;
	case ACTOR::TRIANGLEMESH:
		{
			m_pGameInstance->Create_TriangleMesh(Desc.tRigidStaticDesc.pMeshKey, Desc.tRigidStaticDesc.pMesh);
			m_pMaterial = m_pGameInstance->Get_Material(Desc.tRigidDynamicDesc.vMatInfo);
			m_pMeshKey = Desc.tRigidStaticDesc.pMeshKey;
		}
		break;
	case ACTOR::HEIGHTFIELD:
		break;
	default:
		break;
	}
	return S_OK;
}

HRESULT CRigidBody::Initialize(void* pArg)
{
	RIGIDBODY_DESC* pDesc = (RIGIDBODY_DESC*)pArg;

	if (FAILED(__super::Initialize(pArg))) {
		return E_FAIL;
	}
	if (true == m_bExclusive) {
		return E_FAIL;
	}
	
	m_pTransform = m_pOwner->Get_Component<CTransform>();
	SAFE_ADDREF(m_pTransform);

	switch (m_eActorType)
	{
	case Engine::ACTOR::BOX:
	case Engine::ACTOR::CAPSULE:
	case Engine::ACTOR::SPHERE:
		m_fDensity = pDesc->tRigidDynamicDesc.fDensity;
		m_bKinematic = pDesc->tRigidDynamicDesc.bIsKinematic;
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



	return S_OK;
}

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
