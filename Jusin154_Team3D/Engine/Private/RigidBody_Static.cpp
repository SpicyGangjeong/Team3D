#include "pch.h"
#include "RigidBody_Static.h"
#include "GameInstance.h"

CRigidBody_Static::CRigidBody_Static(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) :
    CRigidBody(pDevice, pContext)
{
}

CRigidBody_Static::CRigidBody_Static(const CRigidBody_Static& rhs) :
    CRigidBody(rhs),
    m_pMeshName(rhs.m_pMeshName)
{
}

HRESULT CRigidBody_Static::Initialize_Prototype(RIGIDBODY_STATIC_PROTOTYPEDESC& Desc)
{
    m_eActorType = Desc.eType;
    m_ePxRigidBodyFlags = Desc.ePxRigidBodyFlags;
    m_ePxShapeFlags = Desc.ePxShapeFlags;
    m_eMatType = Desc.ePxMaterialTypes;
    m_fContactOffset = Desc.fContactOffset;

    return S_OK;
}

HRESULT CRigidBody_Static::Initialize(void* pArg)
{
    m_tagData.eKind = PHYSX_KIND::BODY_STATIC;
    if (FAILED(__super::Initialize(pArg))) {
        return E_FAIL;
    }
    m_tagData.pOwner = m_pOwner;
    XMStoreFloat4x4(&m_tagData.BeforeMatrix, m_pTransform->Get_XMWorldMatrix());
    m_tagData.pBody = this;
    m_pRigidBody = m_pGameInstance->Add_StaticActor(*this);
    m_pRigidBody->userData = &m_tagData;

    return S_OK;
}

CRigidBody_Static* CRigidBody_Static::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, RIGIDBODY_STATIC_PROTOTYPEDESC& Desc)
{
    CRigidBody_Static* pInstance = new CRigidBody_Static(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype(Desc))) {
        MSG_BOX("CRigidBody_Static Prototype Created Failed");
        SAFE_RELEASE(pInstance);
    }

    return pInstance;
}

CComponent* CRigidBody_Static::Clone(void* pArg, CGameObject* pOwner)
{
    CRigidBody_Static* pInstance = new CRigidBody_Static(*this);
    pInstance->m_pOwner = pOwner;
    if (FAILED(pInstance->Initialize(pArg))) {
        MSG_BOX("CRigidBody_Static Prototype Created Failed");
        SAFE_RELEASE(pInstance);
    }

    return pInstance;
}

void CRigidBody_Static::Free()
{
    __super::Free();
    if (nullptr != m_pRigidBody) {
        m_pRigidBody = nullptr;
    }
}

void CRigidBody_Static::Describe_Entity()
{
}
