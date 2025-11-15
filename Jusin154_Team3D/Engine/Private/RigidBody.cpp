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
	m_ePxRigidBodyFlags(rhs.m_ePxRigidBodyFlags),
	m_ePxShapeFlags(rhs.m_ePxShapeFlags),
	m_eMatType(rhs.m_eMatType),
	m_fContactOffset(rhs.m_fContactOffset)
{
}

HRESULT CRigidBody::Initialize(void* pArg)
{
	RIGIDBODY_DESC* pDesc = (RIGIDBODY_DESC*)pArg;

	if (FAILED(__super::Initialize(pArg))) {
		return E_FAIL;
	}
	
	m_pTransform = m_pOwner->Get_Component<CTransform>();
	SAFE_ADDREF(m_pTransform);

	m_tagData.iSubKind = pDesc->iSubKind;

	return S_OK;
}

void CRigidBody::Free()
{
	__super::Free();

	SAFE_RELEASE(m_pTransform);
}

void CRigidBody::Describe_Entity()
{
}
