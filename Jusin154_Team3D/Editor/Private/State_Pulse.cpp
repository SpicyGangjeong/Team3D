#include "pch.h"
#include "State_Pulse.h"
#include "Unit.h"


CState_Pulse::CState_Pulse()
    :CState_Root()
{
}

void CState_Pulse::Enter()
{
    __super::Enter();
}

HRESULT CState_Pulse::Update(_float fTimeDelta)
{
    if (nullptr != m_funcPriorityUpdate) {
        m_funcPriorityUpdate(fTimeDelta);
    }
    if (E_FAIL == (__super::Update(fTimeDelta))) {
        return E_FAIL;
    }
    if (nullptr != m_funcLateUpdate) {
        m_funcLateUpdate(fTimeDelta);
    }
    return S_OK;
}

void CState_Pulse::Exit()
{
    __super::Exit();
}

HRESULT CState_Pulse::Initialize(STATE_PULSE_DESC* pDesc)
{
    if (FAILED(__super::Initialize(pDesc))) {
        return E_FAIL;
    }
    m_funcPriorityUpdate = pDesc->funcPriorityUpdate;
    m_funcLateUpdate = pDesc->funcLateUpdate;
    m_pModel = m_pOwner->Get_Component<CModel>();
    m_pFSM = m_pOwner->Get_Component<CFSM>();

    return S_OK;
}

CState_Pulse* CState_Pulse::Create(STATE_PULSE_DESC* pDesc)
{
    CState_Pulse* pInstance = new CState_Pulse;
    if (FAILED(pInstance->Initialize(pDesc))) {
        SAFE_RELEASE(pInstance);
    }
    return pInstance;
}

void CState_Pulse::Free()
{
    __super::Free();
}

void CState_Pulse::Describe_Entity()
{
}
