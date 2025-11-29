#include "pch.h"
#include "StreetContainer.h"
#include "PartObject.h"

CStreetContainer::CStreetContainer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    :CMapContainer(pDevice, pContext)
{
}

CStreetContainer::CStreetContainer(const CStreetContainer& rhs)
    :CMapContainer(rhs)
{
}

void CStreetContainer::Priority_Update(_float fTimeDelta)
{
    __super::Priority_Update(fTimeDelta);

    for (auto& pCollisiton : m_ColiisonPartObjects)
        pCollisiton->Priority_Update(fTimeDelta);
}

void CStreetContainer::Update(_float fTimeDelta)
{
    __super::Update(fTimeDelta);

    for (auto& pCollisiton : m_ColiisonPartObjects)
        pCollisiton->Update(fTimeDelta);
}

void CStreetContainer::Late_Update(_float fTimeDelta)
{
    __super::Late_Update(fTimeDelta);

    for (auto& pCollisiton : m_ColiisonPartObjects)
        pCollisiton->Late_Update(fTimeDelta);
}

HRESULT CStreetContainer::Render()
{
    return S_OK;
}

HRESULT CStreetContainer::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CStreetContainer::Initialize(void* pArg)
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

HRESULT CStreetContainer::Ready_Components(void* pArg)
{
    if (FAILED(__super::Ready_Components(pArg))) {
        return E_FAIL;
    }

    return S_OK;
}

HRESULT CStreetContainer::Bind_ShaderResources()
{
    return S_OK;
}

CStreetContainer* CStreetContainer::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CStreetContainer* pInstance = new CStreetContainer(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created : CStreetContainer");
        SAFE_RELEASE(pInstance);
    }

    return pInstance;
}

CGameObject* CStreetContainer::Clone(void* pArg, CGameObject* pOwner)
{
    CStreetContainer* pInstance = new CStreetContainer(*this);
    pInstance->m_pOwner = pOwner;
    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Cloned : CStreetContainer");
        SAFE_RELEASE(pInstance);
    }

    return pInstance;
}

void CStreetContainer::Free()
{
    __super::Free();

    for (auto& pCollision : m_ColiisonPartObjects)
        SAFE_RELEASE(pCollision);
}

#ifdef _DEBUG
void CStreetContainer::Describe_Entity()
{

}
#endif
