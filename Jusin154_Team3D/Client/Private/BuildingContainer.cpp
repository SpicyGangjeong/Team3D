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

    for (auto& pCollisiton : m_ColiisonPartObjects){
        pCollisiton->Priority_Update(fTimeDelta);
    }
}

void CBuildingContainer::Update(_float fTimeDelta)
{
    __super::Update(fTimeDelta);

    m_fCamDepth = XMVectorGetX(XMVector3Length(XMLoadFloat4(m_pGameInstance->Get_CamPosition()) - XMVectorSetW(XMLoadFloat3(&m_vExtentWorldPosition), 1.f))) - m_fRadius;

    for (auto& pCollisiton : m_ColiisonPartObjects){
        pCollisiton->Update(fTimeDelta);
    }
}

void CBuildingContainer::Late_Update(_float fTimeDelta)
{
    if (m_pGameInstance->IsIn_WorldFrustum(XMVectorSetW(XMLoadFloat3(&m_vExtentWorldPosition), 1.f), m_fRadius)) {
        if (m_fCamDepth > 100.f)
        {
            if (m_pOcclusionQueryCom->isDraw())
                __super::Late_Update(fTimeDelta);
        }
        else
            __super::Late_Update(fTimeDelta);

        m_pGameInstance->Add_RenderGroup(RENDER::OCCLUSION, this);

        for (auto& pCollisiton : m_ColiisonPartObjects) {
            pCollisiton->Late_Update(fTimeDelta);
        }
    }
    else
        m_pOcclusionQueryCom->Set_PreFrameDraw();

}

HRESULT CBuildingContainer::Render()
{
    return S_OK;
}

HRESULT CBuildingContainer::Render_Shadow(SHADOW eType)
{
    unordered_map<_string, CPartObject*>::iterator iter = m_PartObjects.begin();
    for (; iter != m_PartObjects.end(); ++iter) {
        CPartObject* pObject = (*iter).second;
        if (FAILED(pObject->Render_Shadow(eType))) {
            return E_FAIL;
        }
    }
    return S_OK;
}

HRESULT CBuildingContainer::Render_BoundingBox()
{
    /* Begin_Query, End_Query 사이에 Render */

    m_pOcclusionQueryCom->Begin_Query();

    if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", m_pTransformCom->Get_WorldMatrixPtr()))) {
        return E_FAIL;
    }

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::VIEW)))) {
        return E_FAIL;
    }

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::PROJ)))) {
        return E_FAIL;
    }
   
    if (FAILED(m_pShaderCom->Begin(0))) {
        return E_FAIL;
    }

    if (FAILED(m_pVIBufferCom->Bind_Resources())) {
        return E_FAIL;
    }

    if (FAILED(m_pVIBufferCom->Render())) {
        return E_FAIL;
    }

    m_pOcclusionQueryCom->End_Query();

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

    XMStoreFloat3(&m_vExtentWorldPosition, XMVector3TransformCoord(XMLoadFloat3(&m_vExtentWorldPosition), m_pTransformCom->Get_XMWorldMatrix()));

    return S_OK;
}

HRESULT CBuildingContainer::Ready_Components(void* pArg)
{
    if (FAILED(__super::Ready_Components(pArg))) {
        return E_FAIL;
    }

    /* Com_Shader */
    if (FAILED(__super::Add_Asset_Component(g_iStaticLevel, FX_VTXPOS,
         reinterpret_cast<CComponent**>(&m_pShaderCom)))) {
        return E_FAIL;
    }

    MAP_CONTAINER_DESC* pDesc = static_cast<MAP_CONTAINER_DESC*>(pArg);

	_float3 vBoxCorners[8] = {};

    vBoxCorners[0] = _float3(pDesc->vContainerMin.x, pDesc->vContainerMax.y, pDesc->vContainerMin.z);
    vBoxCorners[1] = _float3(pDesc->vContainerMax.x, pDesc->vContainerMax.y, pDesc->vContainerMin.z);
    vBoxCorners[2] = _float3(pDesc->vContainerMax.x, pDesc->vContainerMin.y, pDesc->vContainerMin.z);
    vBoxCorners[3] = _float3(pDesc->vContainerMin.x, pDesc->vContainerMin.y, pDesc->vContainerMin.z);
    vBoxCorners[4] = _float3(pDesc->vContainerMin.x, pDesc->vContainerMax.y, pDesc->vContainerMax.z);
    vBoxCorners[5] = _float3(pDesc->vContainerMax.x, pDesc->vContainerMax.y, pDesc->vContainerMax.z);
    vBoxCorners[6] = _float3(pDesc->vContainerMax.x, pDesc->vContainerMin.y, pDesc->vContainerMax.z);
    vBoxCorners[7] = _float3(pDesc->vContainerMin.x, pDesc->vContainerMin.y, pDesc->vContainerMax.z);

	_float3 vRadius = {};

    vRadius.x = (pDesc->vContainerMax.x - pDesc->vContainerMin.x) * 0.5f;
    vRadius.y = (pDesc->vContainerMax.y - pDesc->vContainerMin.y) * 0.5f;
    vRadius.z = (pDesc->vContainerMax.z - pDesc->vContainerMin.z) * 0.5f;

    m_fRadius = max(vRadius.x, vRadius.y);
    m_fRadius = max(m_fRadius, vRadius.z);

    m_vExtentWorldPosition.x = pDesc->vContainerMin.x + vRadius.x;
    m_vExtentWorldPosition.y = pDesc->vContainerMin.y + vRadius.y;
    m_vExtentWorldPosition.z = pDesc->vContainerMin.z + vRadius.z;

    /* Com_VIBuffer */
    if (FAILED(__super::Add_Asset_Component(g_iStaticLevel, TEXT("Prototype_Component_VIBuffer_Box"),
        reinterpret_cast<CComponent**>(&m_pVIBufferCom), &vBoxCorners))) {
        return E_FAIL;
    }

    /* Com_OcclusionQuery */
    if (FAILED(__super::Add_Asset_Component(g_iStaticLevel, TEXT("Prototype_Component_OcclusionQuery"),
        reinterpret_cast<CComponent**>(&m_pOcclusionQueryCom)))) {
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

    SAFE_RELEASE(m_pShaderCom);
    SAFE_RELEASE(m_pVIBufferCom);
    SAFE_RELEASE(m_pOcclusionQueryCom);

    for (auto& pCollision : m_ColiisonPartObjects){
        SAFE_RELEASE(pCollision);
    }
}

#ifdef _DEBUG
void CBuildingContainer::Describe_Entity()
{
    
}
#endif
////////////////////////////////
