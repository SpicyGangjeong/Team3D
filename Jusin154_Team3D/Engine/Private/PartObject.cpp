#include "pch.h"

#include "PartObject.h"


CPartObject::CPartObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CGameObject{ pDevice, pContext }
{
}

CPartObject::CPartObject(const CPartObject& rhs)
    : CGameObject(rhs)
{
}

HRESULT CPartObject::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CPartObject::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg))){
        return E_FAIL;
    }
    PARTOBJECT_DESC* pDesc = static_cast<PARTOBJECT_DESC*>(pArg);

    m_pParentTransformCom = pDesc->pParentTransform;

    SAFE_ADDREF(m_pParentTransformCom);

    return S_OK;
}

HRESULT CPartObject::Ready_Components(void* pArg)
{
    if (FAILED(__super::Ready_Components(pArg))) {
        return E_FAIL;
    }
    return S_OK;
}

void CPartObject::Priority_Update(_float fTimeDelta)
{
}

void CPartObject::Update(_float fTimeDelta)
{
}

void CPartObject::Late_Update(_float fTimeDelta)
{
}

HRESULT CPartObject::Render()
{
    return S_OK;
}

_vector CPartObject::Get_WorldPostion()
{
    return m_pOwner->Get_WorldPostion();
}

void CPartObject::Free()
{
    __super::Free();

    SAFE_RELEASE(m_pParentTransformCom);
}
