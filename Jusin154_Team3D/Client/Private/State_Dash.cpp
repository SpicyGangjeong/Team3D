#include "pch.h"
#include "State_Dash.h"
#include "Unit.h"


CState_Dash::CState_Dash()
    :CState_Root()
{
}

void CState_Dash::Enter()
{
    __super::Enter();
}

HRESULT CState_Dash::Update(_float fTimeDelta)
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

void CState_Dash::Exit()
{
    __super::Exit();
}

HRESULT CState_Dash::Initialize(STATE_DASH_DESC* pDesc)
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

CState_Dash* CState_Dash::Create(STATE_DASH_DESC* pDesc)
{
    CState_Dash* pInstance = new CState_Dash;
    if (FAILED(pInstance->Initialize(pDesc))) {
        SAFE_RELEASE(pInstance);
    }
    return pInstance;
}

void CState_Dash::Free()
{
    __super::Free();
}

void CState_Dash::Describe_Entity()
{
}
