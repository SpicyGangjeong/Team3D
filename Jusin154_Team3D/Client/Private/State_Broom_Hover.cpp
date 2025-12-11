#include "pch.h"
#include "State_Broom_Hover.h"
#include "Unit.h"


CState_Broom_Hover::CState_Broom_Hover()
    :CState_Root()
{
}

void CState_Broom_Hover::Enter()
{
    __super::Enter();
}

HRESULT CState_Broom_Hover::Update(_float fTimeDelta)
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

void CState_Broom_Hover::Exit()
{
    __super::Exit();
}

HRESULT CState_Broom_Hover::Initialize(STATE_BROOM_HOVER_DESC* pDesc)
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

CState_Broom_Hover* CState_Broom_Hover::Create(STATE_BROOM_HOVER_DESC* pDesc)
{
    CState_Broom_Hover* pInstance = new CState_Broom_Hover;
    if (FAILED(pInstance->Initialize(pDesc))) {
        SAFE_RELEASE(pInstance);
    }
    return pInstance;
}

void CState_Broom_Hover::Free()
{
    __super::Free();
}

void CState_Broom_Hover::Describe_Entity()
{
}
