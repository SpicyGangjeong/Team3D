#include "pch.h"
#include "State_Hover.h"
#include "Unit.h"


CState_Hover::CState_Hover()
    :CState_Root()
{
}

void CState_Hover::Enter()
{
    __super::Enter();
}

HRESULT CState_Hover::Update(_float fTimeDelta)
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

void CState_Hover::Exit()
{
    __super::Exit();
}

HRESULT CState_Hover::Initialize(STATE_HOVER_DESC* pDesc)
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

CState_Hover* CState_Hover::Create(STATE_HOVER_DESC* pDesc)
{
    CState_Hover* pInstance = new CState_Hover;
    if (FAILED(pInstance->Initialize(pDesc))) {
        SAFE_RELEASE(pInstance);
    }
    return pInstance;
}

void CState_Hover::Free()
{
    __super::Free();
}

void CState_Hover::Describe_Entity()
{
}
