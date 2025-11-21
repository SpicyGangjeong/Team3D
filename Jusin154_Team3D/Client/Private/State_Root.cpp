#include "pch.h"
#include "State_Root.h"
#include "Unit.h"


CState_Root::CState_Root()
    :CState()
{
}

void CState_Root::Enter()
{
    if (m_pOwner->Check(FSMSTATE::COMBAT))
        m_pFSM->Change_State(FSMSTATE::COMBAT);

    else
        m_pFSM->Change_State(FSMSTATE::MOVE);
}

void CState_Root::Update(_float fTimeDelta)
{
    if (CheckExitState())
        return;
}

void CState_Root::Exit()
{
}

_bool CState_Root::CheckExitState()
{
    if (m_pOwner->Check(FSMSTATE::COMBAT))
    {
        if (m_pFSM->Get_Current()->Get_Parent()->Get_State() != FSMSTATE::COMBAT)
            m_pFSM->Change_State(FSMSTATE::COMBAT);

        return true;
    }

    if (m_pFSM->Get_Current()->Get_Parent()->Get_State() == FSMSTATE::COMBAT)
        return false;

    if (m_pOwner->Check(FSMSTATE::MOVE))
    {
        if (m_pFSM->Get_Current()->Get_Parent()->Get_State() != FSMSTATE::MOVE)
            m_pFSM->Change_State(FSMSTATE::MOVE);

        return true;
    }

    return false;
}


void CState_Root::Free()
{
    __super::Free();
}

void CState_Root::Describe_Entity()
{
}
