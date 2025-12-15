#include "pch.h"
#include "State_Swing.h"
#include "Unit.h"


CState_Swing::CState_Swing()
    :CState_Root()
{
}

void CState_Swing::Enter()
{
    m_bPlayerHit = false;
    __super::Enter();
}

HRESULT CState_Swing::Update(_float fTimeDelta)
{
    if (nullptr != m_funcPriorityUpdate) {
        m_funcPriorityUpdate(fTimeDelta);
    }
    if (E_FAIL == (__super::Update(fTimeDelta))) {
        return E_FAIL;
    }
    if (nullptr != m_funcLateUpdate) {
        m_funcLateUpdate(fTimeDelta, m_bPlayerHit);
    }
    return S_OK;
}

void CState_Swing::Exit()
{
    m_bPlayerHit = false;
    __super::Exit();
}

HRESULT CState_Swing::Initialize(STATE_SWING_DESC* pDesc)
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

CState_Swing* CState_Swing::Create(STATE_SWING_DESC* pDesc)
{
    CState_Swing* pInstance = new CState_Swing;
    if (FAILED(pInstance->Initialize(pDesc))) {
        SAFE_RELEASE(pInstance);
    }
    return pInstance;
}

void CState_Swing::Free()
{
    __super::Free();
}

void CState_Swing::Describe_Entity()
{
}
