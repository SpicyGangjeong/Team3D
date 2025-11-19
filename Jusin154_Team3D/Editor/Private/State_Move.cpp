#include "pch.h"
#include "State_Move.h"
#include "Unit.h"


CState_Move::CState_Move()
    :CState()
{
}

void CState_Move::Enter()
{

}

void CState_Move::Update(_float fTimeDelta)
{
    if (CheckExitState())
        return;
}

void CState_Move::Exit()
{
}

_bool CState_Move::CheckExitState()
{
    if (m_pOwner->Check(FSMSTATE::COMBAT) && 
        !m_pOwner->Check(FSMSTATE::MOVE))
    {
        m_pFSM->Change_State(FSMSTATE::COMBAT);

        if (m_pOwner->Check(FSMSTATE::SKILL))
            m_pFSM->Change_State(FSMSTATE::SKILL);

        if (m_pOwner->Check(FSMSTATE::SKILL2))
            m_pFSM->Change_State(FSMSTATE::SKILL2);

        if (m_pOwner->Check(FSMSTATE::LIGHT_ATTACK))
            m_pFSM->Change_State(FSMSTATE::LIGHT_ATTACK);

        if (m_pOwner->Check(FSMSTATE::CAST))
            m_pFSM->Change_State(FSMSTATE::CAST);

        return true;
    }

    return false;
}


void CState_Move::Free()
{
    __super::Free();
}

void CState_Move::Describe_Entity()
{
}
