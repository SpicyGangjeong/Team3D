#include "pch.h"
#include "ContainerObject.h"
#include "PartObject.h"

CContainerObject::CContainerObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CGameObject(pDevice, pContext)
{
}

CContainerObject::CContainerObject(const CContainerObject& rhs)
	:CGameObject(rhs)
{
}

HRESULT CContainerObject::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CContainerObject::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg))) {
        return E_FAIL;
    }

    return S_OK;
}

HRESULT CContainerObject::Ready_Components(void* pArg)
{
    if (FAILED(__super::Ready_Components(pArg))) {
        return E_FAIL;
    }
    return S_OK;
}

void CContainerObject::Priority_Update(_float fTimeDelta)
{
    for (auto& Pair : m_PartObjects){
        Pair.second->Priority_Update(fTimeDelta);
    }
}

void CContainerObject::Update(_float fTimeDelta)
{
    for (auto& Pair : m_PartObjects) {
        Pair.second->Update(fTimeDelta);
    }
}

void CContainerObject::Late_Update(_float fTimeDelta)
{
    for (auto& Pair : m_PartObjects) {
        Pair.second->Late_Update(fTimeDelta);
    }
}

HRESULT CContainerObject::Render()
{
    return S_OK;
}

void CContainerObject::Free()
{
	__super::Free();

    for (auto& Pair : m_PartObjects){
        SAFE_RELEASE(Pair.second);
    }

    m_PartObjects.clear();

}
