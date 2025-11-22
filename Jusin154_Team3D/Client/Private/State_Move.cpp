#include "pch.h"
#include "State_Move.h"
#include "Unit.h"


CState_Move::CState_Move()
    :CState()
{
}

void CState_Move::Enter()
{

    if (m_pOwner->Check(FSMSTATE::WALK))
        m_pFSM->Change_State(FSMSTATE::WALK);

    if (m_pOwner->Check(FSMSTATE::JOG))
        m_pFSM->Change_State(FSMSTATE::JOG);

    if (m_pOwner->Check(FSMSTATE::SPRINT))
        m_pFSM->Change_State(FSMSTATE::SPRINT);

    if (m_pOwner->Check(FSMSTATE::DODGE))
        m_pFSM->Change_State(FSMSTATE::DODGE);

    if (m_pOwner->Check(FSMSTATE::IDLE_TURN))
        m_pFSM->Change_State(FSMSTATE::IDLE_TURN);

    m_pFSM->Change_State(FSMSTATE::IDLE);
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
    return false;
}


void CState_Move::Free()
{
    __super::Free();
}

void CState_Move::Describe_Entity()
{
}
