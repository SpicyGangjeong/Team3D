#include "pch.h"
#include "State_FireSweep.h"
#include "Unit.h"


CState_FireSweep::CState_FireSweep()
    :CState_Root()
{
}

void CState_FireSweep::Enter()
{
    __super::Enter();
}

HRESULT CState_FireSweep::Update(_float fTimeDelta)
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

void CState_FireSweep::Exit()
{
    __super::Exit();
}

HRESULT CState_FireSweep::Initialize(STATE_FIRESWEEP_DESC* pDesc)
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

CState_FireSweep* CState_FireSweep::Create(STATE_FIRESWEEP_DESC* pDesc)
{
    CState_FireSweep* pInstance = new CState_FireSweep;
    if (FAILED(pInstance->Initialize(pDesc))) {
        SAFE_RELEASE(pInstance);
    }
    return pInstance;
}

void CState_FireSweep::Free()
{
    __super::Free();
}

void CState_FireSweep::Describe_Entity()
{
}
