#include "pch.h"
#include "State_Combat.h"
#include "Unit.h"


CState_Combat::CState_Combat()
    :CState()
{
}

void CState_Combat::Enter()
{

}

void CState_Combat::Update(_float fTimeDelta)
{
    if (CheckExitState())
        return;
}

void CState_Combat::Exit()
{
}

_bool CState_Combat::CheckExitState()
{
    if (m_pOwner->Check(FSMSTATE::MOVE) && 
        !m_pOwner->Check(FSMSTATE::COMBAT))
    {
        m_pFSM->Change_State(FSMSTATE::MOVE);

        if (m_pOwner->Check(FSMSTATE::WALK))
            m_pFSM->Change_State(FSMSTATE::WALK);

        if (m_pOwner->Check(FSMSTATE::SPRINT))
            m_pFSM->Change_State(FSMSTATE::SPRINT);

        if (m_pOwner->Check(FSMSTATE::DODGE))
            m_pFSM->Change_State(FSMSTATE::DODGE);

        return true;
    }

    return false;
}


void CState_Combat::Free()
{
    __super::Free();
}

void CState_Combat::Describe_Entity()
{
}
