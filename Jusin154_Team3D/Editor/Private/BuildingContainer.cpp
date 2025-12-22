#include "pch.h"
#include "BuildingContainer.h"
#include "PartObject.h"
#include "Terrain.h"
#include "Layer.h"
#include "VIBuffer_Terrain.h"
#include "MapObject_LOD.h"
#include "Bounding_OBB.h"

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
    m_pVIBufferCom->Update(m_vBoxCorners);

    __super::Update(fTimeDelta);

    for (auto& pCollisiton : m_ColiisonPartObjects)
        pCollisiton->Update(fTimeDelta);

    if (m_bOcclusionPassed)
    {
        _vector WorldPosition = XMVector3TransformCoord(
            XMVectorSetW(XMLoadFloat3(&m_vExtentPosition), 1.f),
            XMMatrixTranslation(m_vExtentPosition.x, m_vExtentPosition.y, m_vExtentPosition.z) * m_pTransformCom->Get_XMWorldMatrix());

        _vector		vCamPosition = XMLoadFloat4(m_pGameInstance->Get_CamPosition());

       // m_fCamDepth = XMVectorGetX(XMVector3LengthSq(vCamPosition - XMVectorSetW(XMLoadFloat3(&m_vExtentPosition), 1.f)));
        m_fCamDepth = XMVectorGetX(XMVector3LengthSq(vCamPosition - WorldPosition));
    }
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

    for (auto& pCollisiton : m_ColiisonPartObjects)
        pCollisiton->Late_Update(fTimeDelta);

    if (m_bOcclusionPassed)
    {
        if(m_fCamDepth > 3000.f)
        {
            if (m_pOcclusionQueryCom->isDraw())
                __super::Late_Update(fTimeDelta);
        }
        else
            __super::Late_Update(fTimeDelta);

        m_pGameInstance->Add_RenderGroup(RENDER::OCCLUSION, this);
    }
    else
    {
       __super::Late_Update(fTimeDelta);
    }
}

HRESULT CBuildingContainer::Render()
{
    return S_OK;
}

HRESULT CBuildingContainer::Render_BoundingBox()
{
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

    if (FAILED(m_pVIBufferCom->Bind_Resources()))
        return E_FAIL;

    if (FAILED(m_pVIBufferCom->Render()))
        return E_FAIL;

   m_pOcclusionQueryCom->End_Query();

    return S_OK;
}

void CBuildingContainer::Set_BoundingBox()
{
    _float3 vCurrentMin = {};
    _float3 vCurrentMax = {};
   

    for (auto& piar : m_PartObjects)
    {
        dynamic_cast<CMapObject_LOD*>(piar.second)->Get_BoundingBox(&vCurrentMin, &vCurrentMax);

        vContainerMin.x = min(vContainerMin.x, vCurrentMin.x);
        vContainerMin.y = min(vContainerMin.y, vCurrentMin.y);
        vContainerMin.z = min(vContainerMin.z, vCurrentMin.z);

        vContainerMax.x = max(vContainerMax.x, vCurrentMax.x);
        vContainerMax.y = max(vContainerMax.y, vCurrentMax.y);
        vContainerMax.z = max(vContainerMax.z, vCurrentMax.z);
    }

    m_vExtentRadius.x = (vContainerMax.x - vContainerMin.x) * 0.5f;
    m_vExtentRadius.y = (vContainerMax.y - vContainerMin.y) * 0.5f;
    m_vExtentRadius.z = (vContainerMax.z - vContainerMin.z) * 0.5f;

    m_vExtentPosition.x = vContainerMin.x + m_vExtentRadius.x;
    m_vExtentPosition.y = vContainerMin.y + m_vExtentRadius.y;
    m_vExtentPosition.z = vContainerMin.z + m_vExtentRadius.z;
    
    m_vBoxCorners[0] = _float3(vContainerMin.x, vContainerMax.y, vContainerMin.z);
    m_vBoxCorners[1] = _float3(vContainerMax.x, vContainerMax.y, vContainerMin.z);
    m_vBoxCorners[2] = _float3(vContainerMax.x, vContainerMin.y, vContainerMin.z);
    m_vBoxCorners[3] = _float3(vContainerMin.x, vContainerMin.y, vContainerMin.z);
    m_vBoxCorners[4] = _float3(vContainerMin.x, vContainerMax.y, vContainerMax.z);
    m_vBoxCorners[5] = _float3(vContainerMax.x, vContainerMax.y, vContainerMax.z);
    m_vBoxCorners[6] = _float3(vContainerMax.x, vContainerMin.y, vContainerMax.z);
    m_vBoxCorners[7] = _float3(vContainerMin.x, vContainerMin.y, vContainerMax.z);

    m_pVIBufferCom->Update(m_vBoxCorners);

	//XMStoreFloat4x4(&m_BoundingBoxWorldMatirx, m_pTransformCom->Get_XMWorldMatrix());

	//memcpy(&m_BoundingBoxWorldMatirx.m[3], &m_vExtentPosition, sizeof(_float3));
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
    if (string::npos == m_strName.find("Street"))
        m_bOcclusionPassed = true;
    else
		m_bOcclusionPassed = false;

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

    /* Com_Shader */
    if (FAILED(__super::Add_Asset_Component(g_iStaticLevel, FX_VTXPOS,
        reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;

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

    for (auto& pCollision : m_ColiisonPartObjects)
        SAFE_RELEASE(pCollision);
}

void CBuildingContainer::Describe_Entity()
{
    if (m_bDead)
        return;
    if (nullptr == m_pGameInstance)
        return;
    if (GUI::Button("BoundingBox"))
    {
        Set_BoundingBox();
    }
    _float3 vMove = {};
    GUI::InputFloat("Right", &vMove.x, 0.1f, 1.f);
    GUI::InputFloat("Up", &vMove.y, 0.1f, 1.f);
    GUI::InputFloat("Look", &vMove.z, 0.1f, 1.f);

    m_pTransformCom->Move_Right(vMove.x);
    m_pTransformCom->Move_Up(vMove.y);
    m_pTransformCom->Move_Look(vMove.z);

    XMStoreFloat3(&m_vPosition, m_pTransformCom->Get_State(STATE::POSITION));

    GUI::InputFloat("X##Position", &m_vPosition.x, 0.1f, 1.f);
    GUI::InputFloat("Y##Position", &m_vPosition.y, 0.1f, 1.f);
    GUI::InputFloat("Z##Position", &m_vPosition.z, 0.1f, 1.f);
    m_pTransformCom->Set_State(STATE::POSITION, XMVectorSetW(XMLoadFloat3(&m_vPosition), 1.f));

    if (m_pGameInstance->Mouse_Down(DIM_LBUTTON) && m_pGameInstance->Key_Pressing(DIK_LSHIFT))
    {
        _float3 vPosition = {};
        if (m_pGameInstance->isPicking(&vPosition))
        {
            memcpy(&m_vPosition, &vPosition, sizeof(_float3));
            m_pTransformCom->Set_State(STATE::POSITION, XMLoadFloat3(&m_vPosition));
        }
    }

    GUI::Text("----- Rotation ----");
    GUI::InputFloat("X##Rotation", &m_vRotation.x, 1.f, 15.f);
    GUI::InputFloat("Y##Rotation", &m_vRotation.y, 1.f, 15.f);
    GUI::InputFloat("Z##Rotation", &m_vRotation.z, 1.f, 15.f);

    GUI::Text("----- Scale ----");
    GUI::InputFloat("X##Scale", &m_vScale.x, 0.1f, 1.f);
    GUI::InputFloat("Y##Scale", &m_vScale.y, 0.1f, 1.f);
    GUI::InputFloat("Z##Scale", &m_vScale.z, 0.1f, 1.f);

    m_vScale.x = max(0.01f, m_vScale.x);
    m_vScale.y = max(0.01f, m_vScale.y);
    m_vScale.z = max(0.01f, m_vScale.z);


	GUI::InputFloat3("Extent Radius", &m_vExtentRadius.x);
	GUI::InputFloat3("Extent Position", &m_vExtentPosition.x);

    if (GUI::Button("Delete"))
        m_bDead = true;

    m_bSelected = true;
}
