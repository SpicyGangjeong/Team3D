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
    if (m_pOwner->Check(FSMSTATE::MOVE))
    {
        m_pFSM->Change_State(FSMSTATE::MOVE);
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
