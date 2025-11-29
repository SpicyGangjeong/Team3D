#include "pch.h"
#include "BuildingContainer.h"
#include "PartObject.h"

CBuildingContainer::CBuildingContainer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    :CMapContainer(pDevice, pContext)
{
}

CBuildingContainer::CBuildingContainer(const CBuildingContainer& rhs)
    :CMapContainer(rhs)
{
}

void CBuildingContainer::Priority_Update(_float fTimeDelta)
{
    __super::Priority_Update(fTimeDelta);

    for (auto& pCollisiton : m_ColiisonPartObjects)
        pCollisiton->Priority_Update(fTimeDelta);
}

void CBuildingContainer::Update(_float fTimeDelta)
{
    __super::Update(fTimeDelta);

    for (auto& pCollisiton : m_ColiisonPartObjects)
        pCollisiton->Update(fTimeDelta);
}

void CBuildingContainer::Late_Update(_float fTimeDelta)
{
    __super::Late_Update(fTimeDelta);

    for (auto& pCollisiton : m_ColiisonPartObjects)
        pCollisiton->Late_Update(fTimeDelta);
}

HRESULT CBuildingContainer::Render()
{
    return S_OK;
}

HRESULT CBuildingContainer::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CBuildingContainer::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg))) {
        return E_FAIL;
    }

    if (FAILED(Ready_Components(pArg))) {
        return E_FAIL;
    }

    MAP_CONTAINER_DESC* pDesc = static_cast<MAP_CONTAINER_DESC*>(pArg);

    m_pTransformCom->Set_State(STATE::POSITION, XMVectorSetW(XMLoadFloat3(&pDesc->vPosition), 1.f));
    m_pTransformCom->Set_Scale(pDesc->vScale);
    m_pTransformCom->Rotation(XMConvertToRadians(pDesc->vRotation.x), XMConvertToRadians(pDesc->vRotation.y), XMConvertToRadians(pDesc->vRotation.z));

    return S_OK;
}

HRESULT CBuildingContainer::Ready_Components(void* pArg)
{
    if (FAILED(__super::Ready_Components(pArg))) {
        return E_FAIL;
    }

    return S_OK;
}

HRESULT CBuildingContainer::Bind_ShaderResources()
{
    return S_OK;
}

CBuildingContainer* CBuildingContainer::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CBuildingContainer* pInstance = new CBuildingContainer(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created : CBuildingContainer");
        SAFE_RELEASE(pInstance);
    }

    return pInstance;
}

CGameObject* CBuildingContainer::Clone(void* pArg, CGameObject* pOwner)
{
    CBuildingContainer* pInstance = new CBuildingContainer(*this);
    pInstance->m_pOwner = pOwner;
    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Cloned : CBuildingContainer");
        SAFE_RELEASE(pInstance);
    }

    return pInstance;
}

void CBuildingContainer::Free()
{
    __super::Free();

    for (auto& pCollision : m_ColiisonPartObjects)
        SAFE_RELEASE(pCollision);
}

#ifdef _DEBUG
void CBuildingContainer::Describe_Entity()
{
    
}
#endif
