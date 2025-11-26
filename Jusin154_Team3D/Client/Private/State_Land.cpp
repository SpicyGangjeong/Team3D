#include "pch.h"
#include "State_Land.h"
#include "Unit.h"


CState_Land::CState_Land()
    :CState_Root()
{
}

void CState_Land::Enter()
{
    __super::Enter();
}

HRESULT CState_Land::Update(_float fTimeDelta)
{
    if (E_FAIL == (__super::Update(fTimeDelta))) {
        return E_FAIL;
    }
    return S_OK;
}

void CState_Land::Exit()
{
    __super::Exit();
}

HRESULT CState_Land::Initialize(STATE_LAND_DESC* pDesc)
{
    if (FAILED(__super::Initialize(pDesc))) {
        return E_FAIL;
    }
    m_pModel = m_pOwner->Get_Component<CModel>();
    m_pFSM = m_pOwner->Get_Component<CFSM>();

    return S_OK;
}

CState_Land* CState_Land::Create(STATE_LAND_DESC* pDesc)
{
    CState_Land* pInstance = new CState_Land;
    if (FAILED(pInstance->Initialize(pDesc))) {
        SAFE_RELEASE(pInstance);
    }
    return pInstance;
}

void CState_Land::Free()
{
    __super::Free();
}

void CState_Land::Describe_Entity()
{
}
