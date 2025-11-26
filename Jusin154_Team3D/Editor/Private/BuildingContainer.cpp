#include "pch.h"
#include "BuildingContainer.h"
#include "PartObject.h"
#include "Terrain.h"
#include "Layer.h"
#include "VIBuffer_Terrain.h"
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

    m_bSelected = false;
}

void CBuildingContainer::Update(_float fTimeDelta)
{
    __super::Update(fTimeDelta);

    for (auto& pCollisiton : m_ColiisonPartObjects)
        pCollisiton->Update(fTimeDelta);
}

void CBuildingContainer::Late_Update(_float fTimeDelta)
{

#ifdef _DEBUG
    if (m_bSelected)
    {
        m_pTransformCom->Set_State(STATE::POSITION, XMVectorSetW(XMLoadFloat3(&m_vPosition), 1.f));
        m_pTransformCom->Set_Scale(m_vScale);
        m_pTransformCom->Rotation(XMConvertToRadians(m_vRotation.x), XMConvertToRadians(m_vRotation.y), XMConvertToRadians(m_vRotation.z));
    }
#endif // _DEBUG

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

    m_strName = pDesc->strName;

    m_vPosition = pDesc->vPosition;
    m_vScale = pDesc->vScale;
    m_vRotation = pDesc->vRotation;

    m_pTransformCom->Set_State(STATE::POSITION, XMVectorSetW(XMLoadFloat3(&m_vPosition), 1.f));
    m_pTransformCom->Set_Scale(m_vScale);
    m_pTransformCom->Rotation(XMConvertToRadians(m_vRotation.x), XMConvertToRadians(m_vRotation.y), XMConvertToRadians(m_vRotation.z));

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

void CBuildingContainer::Describe_Entity()
{
    if (m_bDead)
        return;
    if (nullptr == m_pGameInstance)
        return;

    GUI::Text(m_strName.c_str());
    GUI::Text("----- Transfrom ----");
    GUI::InputFloat("X##Position", &m_vPosition.x, 0.1f, 1.f);
    GUI::InputFloat("Y##Position", &m_vPosition.y, 0.1f, 1.f);
    GUI::InputFloat("Z##Position", &m_vPosition.z, 0.1f, 1.f);

    if (m_pGameInstance->Mouse_Down(DIM_LBUTTON) && m_pGameInstance->Key_Pressing(DIK_LSHIFT))
    {
        if (m_pGameInstance->isPicking(&m_vPosition))
        {
        }
    }

    GUI::Text("----- Rotation ----");
    GUI::InputFloat("X##Rotation", &m_vRotation.x, 10.f, 45.f);
    GUI::InputFloat("Y##Rotation", &m_vRotation.y, 10.f, 45.f);
    GUI::InputFloat("Z##Rotation", &m_vRotation.z, 10.f, 45.f);

    GUI::Text("----- Scale ----");
    GUI::InputFloat("X##Scale", &m_vScale.x, 0.1f, 1.f);
    GUI::InputFloat("Y##Scale", &m_vScale.y, 0.1f, 1.f);
    GUI::InputFloat("Z##Scale", &m_vScale.z, 0.1f, 1.f);

    m_vScale.x = max(0.01f, m_vScale.x);
    m_vScale.y = max(0.01f, m_vScale.y);
    m_vScale.z = max(0.01f, m_vScale.z);


    if (GUI::Button("Delete"))
        m_bDead = true;

    m_bSelected = true;
}
