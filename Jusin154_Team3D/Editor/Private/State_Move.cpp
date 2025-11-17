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
    if (m_pOwner->Check(FSMSTATE::COMBAT))
    {
        m_pFSM->Change_State(FSMSTATE::COMBAT);
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
