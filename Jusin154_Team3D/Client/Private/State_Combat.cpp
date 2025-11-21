#include "pch.h"
#include "State_Combat.h"
#include "Unit.h"


CState_Combat::CState_Combat()
    :CState()
{
}

void CState_Combat::Enter()
{
    if (m_pOwner->Check(FSMSTATE::LIGHT_ATTACK))
        m_pFSM->Change_State(FSMSTATE::LIGHT_ATTACK);

    if (m_pOwner->Check(FSMSTATE::CAST))
        m_pFSM->Change_State(FSMSTATE::CAST);

    if (m_pOwner->Check(FSMSTATE::SKILL))
        m_pFSM->Change_State(FSMSTATE::SKILL);

    if (m_pOwner->Check(FSMSTATE::SKILL))
        m_pFSM->Change_State(FSMSTATE::SKILL);

    if (m_pOwner->Check(FSMSTATE::SKILL2))
        m_pFSM->Change_State(FSMSTATE::SKILL2);
    
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
    if (m_pOwner->IsCurrentKeyFrame("Jump"))
    {
        m_pOwner->Reset_LightCombo();
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
