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

    m_fCamDepth = XMVectorGetX(XMVector3Length(XMLoadFloat4(m_pGameInstance->Get_CamPosition()) - XMLoadFloat3(&m_vExtentPosition)));

    for (auto& pCollisiton : m_ColiisonPartObjects)
        pCollisiton->Update(fTimeDelta);
}

void CStreetContainer::Late_Update(_float fTimeDelta)
{
    if (m_pOcclusionQueryCom->isDraw())
        __super::Late_Update(fTimeDelta);

    m_pGameInstance->Add_RenderGroup(RENDER::OCCLUSION, this);

    __super::Late_Update(fTimeDelta);

    for (auto& pCollisiton : m_ColiisonPartObjects)
        pCollisiton->Late_Update(fTimeDelta);
}

HRESULT CStreetContainer::Render()
{
    return S_OK;
}

HRESULT CStreetContainer::Render_BoundingBox()
{
    m_pOcclusionQueryCom->Begin_Query();

    if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_BoundingBoxWorldMatirx))) {
        return E_FAIL;
    }

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::VIEW)))) {
        return E_FAIL;
    }
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::PROJ)))) {
        return E_FAIL;
    }
    if (m_pGameInstance->Key_Pressing(DIK_Q))
    {
        if (FAILED(m_pShaderCom->Begin(1)))
            return E_FAIL;
    }
    else
    {
        if (FAILED(m_pShaderCom->Begin(0)))
            return E_FAIL;
    }

    if (FAILED(m_pShaderCom->Bind_RawValue("g_vRadius", &m_vExtentRadius, sizeof(_float3))))
        return E_FAIL;

    if (FAILED(m_pVIBufferCom->Bind_Resources()))
        return E_FAIL;

    if (FAILED(m_pVIBufferCom->Render()))
        return E_FAIL;

    m_pOcclusionQueryCom->End_Query();

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

    ///* Com_Shader */
    //if (FAILED(__super::Add_Asset_Component(g_iStaticLevel, FX_VTXPOS,
    //    reinterpret_cast<CComponent**>(&m_pShaderCom))))
    //    return E_FAIL;

    /* Com_Shader */
    if (FAILED(__super::Add_Asset_Component(g_iStaticLevel, TEXT("Prototype_Component_VIBuffer_Box"),
        reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
        return E_FAIL;

    /* Com_OcclusionQuery */
    if (FAILED(__super::Add_Asset_Component(g_iStaticLevel, TEXT("Prototype_Component_OcclusionQuery"),
        reinterpret_cast<CComponent**>(&m_pOcclusionQueryCom))))
        return E_FAIL;

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

	SAFE_RELEASE(m_pShaderCom);
	SAFE_RELEASE(m_pVIBufferCom);
	SAFE_RELEASE(m_pOcclusionQueryCom);

    for (auto& pCollision : m_ColiisonPartObjects)
        SAFE_RELEASE(pCollision);
}

#ifdef _DEBUG
void CStreetContainer::Describe_Entity()
{

}
#endif
