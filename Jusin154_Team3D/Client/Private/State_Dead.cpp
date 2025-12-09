#include "pch.h"
#include "State_Dead.h"
#include "Unit.h"


CState_Dead::CState_Dead()
    :CState_Root()
{
}

void CState_Dead::Enter()
{
    __super::Enter();
}

HRESULT CState_Dead::Update(_float fTimeDelta)
{
    HRESULT hr = (__super::Update(fTimeDelta));
    switch (hr)
    {
    case E_PENDING:
        m_vDeadTimer.x += fTimeDelta;
        break;
    default:
        break;
    }
    m_funcLateUpdate(m_vDeadTimer.x / m_vDeadTimer.y);
    if (m_vDeadTimer.x >= m_vDeadTimer.y) {
        return E_FAIL;
    }
    return S_OK;
}

void CState_Dead::Exit()
{
    __super::Exit();
}

HRESULT CState_Dead::Initialize(STATE_DEAD_DESC* pDesc)
{
    if (FAILED(__super::Initialize(pDesc))) {
        return E_FAIL;
    }
    m_pModel = m_pOwner->Get_Component<CModel>();
    m_pFSM = m_pOwner->Get_Component<CFSM>();
    m_vDeadTimer = pDesc->vDeadTimer;
    m_funcLateUpdate = pDesc->funcLateUpdate;
    return S_OK;
}

CState_Dead* CState_Dead::Create(STATE_DEAD_DESC* pDesc)
{
    CState_Dead* pInstance = new CState_Dead;
    if (FAILED(pInstance->Initialize(pDesc))) {
        SAFE_RELEASE(pInstance);
    }
    return pInstance;
}

void CState_Dead::Free()
{
    __super::Free();
}

void CState_Dead::Describe_Entity()
{
}
