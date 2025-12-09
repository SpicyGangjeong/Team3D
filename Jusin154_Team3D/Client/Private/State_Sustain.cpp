#include "pch.h"
#include "State_Sustain.h"
#include "Unit.h"


CState_Sustain::CState_Sustain()
    :CState_Root()
{
}

void CState_Sustain::Enter()
{
    __super::Enter();
}

HRESULT CState_Sustain::Update(_float fTimeDelta)
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

void CState_Sustain::Exit()
{
    __super::Exit();
}

HRESULT CState_Sustain::Initialize(STATE_SUSTAIN_DESC* pDesc)
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

CState_Sustain* CState_Sustain::Create(STATE_SUSTAIN_DESC* pDesc)
{
    CState_Sustain* pInstance = new CState_Sustain;
    if (FAILED(pInstance->Initialize(pDesc))) {
        SAFE_RELEASE(pInstance);
    }
    return pInstance;
}

void CState_Sustain::Free()
{
    __super::Free();
}

void CState_Sustain::Describe_Entity()
{
}
