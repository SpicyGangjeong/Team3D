#include "pch.h"
#include "State_Cast.h"
#include "Unit.h"


CState_Cast::CState_Cast()
    :CState()
{
}

void CState_Cast::Enter()
{
    if (m_pFSM->Get_PrevState() == FSMSTATE::CAST)
        return;

    auto anim = m_pOwner->Get_AnimInfo(FSMSTATE::CAST);
    m_pModel->Set_AnimationIndex(anim.first, anim.second);
}

void CState_Cast::Update(_float fTimeDelta)
{
    if (CheckExitState())
        return;
}

void CState_Cast::Exit()
{
}

_bool CState_Cast::CheckExitState()
{
    if (m_pOwner->Check(FSMSTATE::WALK))
    {
        m_pOwner->Reset_LightCombo();
        m_pFSM->Change_State(FSMSTATE::WALK);
        return true;
    }

    if (m_pModel->IsFinishedAnim())
    {
        m_pOwner->Reset_LightCombo();
        m_pFSM->Change_State(FSMSTATE::IDLE);
        return true;
    }

    if (m_pOwner->Check(FSMSTATE::CAST))
    {
        _uint combo = m_pOwner->Next_LightCombo();
        m_pOwner->Set_LightCombo(combo);
        if (combo >= 3)
        {
            m_pOwner->Reset_LightCombo();
            m_pFSM->Change_State(FSMSTATE::IDLE);
            return true;
        }

        auto anim = m_pOwner->Get_AnimInfo(FSMSTATE::CAST);
        m_pModel->Set_AnimationIndex(anim.first + combo, anim.second);

        m_pFSM->Change_State(FSMSTATE::CAST);

        return true;
    }

    return false;
}

void CState_Cast::Free()
{
    __super::Free();
}

void CState_Cast::Describe_Entity()
{
}
