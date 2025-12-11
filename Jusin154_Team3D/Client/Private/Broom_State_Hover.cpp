#include "pch.h"
#include "Broom_State_Hover.h"
#include "Unit.h"


CBroom_State_Hover::CBroom_State_Hover()
    :CState_Root()
{
}

void CBroom_State_Hover::Enter()
{
    __super::Enter();
}

HRESULT CBroom_State_Hover::Update(_float fTimeDelta)
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

void CBroom_State_Hover::Exit()
{
    __super::Exit();
}

HRESULT CBroom_State_Hover::Initialize(STATE_HOVER_DESC* pDesc)
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

CBroom_State_Hover* CBroom_State_Hover::Create(STATE_HOVER_DESC* pDesc)
{
    CBroom_State_Hover* pInstance = new CBroom_State_Hover;
    if (FAILED(pInstance->Initialize(pDesc))) {
        SAFE_RELEASE(pInstance);
    }
    return pInstance;
}

void CBroom_State_Hover::Free()
{
    __super::Free();
}

void CBroom_State_Hover::Describe_Entity()
{
}
