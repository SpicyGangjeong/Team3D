#include "pch.h"
#include "State_LightAttack.h"
#include "Unit.h"


CState_LightAttack::CState_LightAttack()
	:CState()
{
}

void CState_LightAttack::Enter()
{
    if (m_pFSM->Get_PrevState() == FSMSTATE::LIGHT_ATTACK)
        return;

	auto anim = m_pOwner->Get_AnimInfo(STATEANIM::LIGHT_ATTACK);
	m_pModel->Set_AnimationIndex(anim.first, anim.second);
}

void CState_LightAttack::Update(_float fTimeDelta)
{
	if (CheckExitState())
		return;
}

void CState_LightAttack::Exit()
{
}

_bool CState_LightAttack::CheckExitState()
{
    if (m_pModel->IsFinishedAnim())
    {
        m_pOwner->Reset_LightCombo();
        m_pFSM->Change_State(FSMSTATE::IDLE);
        return true;
    }

    if (m_pOwner->Check(FSMSTATE::LIGHT_ATTACK))
    {
        _uint combo = m_pOwner->Next_LightCombo();
         m_pOwner->Set_LightCombo(combo);
        if (combo >= 4)
        {
            m_pOwner->Reset_LightCombo();
            m_pFSM->Change_State(FSMSTATE::IDLE);
            return true;
        }

        auto anim = m_pOwner->Get_AnimInfo(STATEANIM::LIGHT_ATTACK);
        m_pModel->Set_AnimationIndex(anim.first + combo, anim.second);

        m_pFSM->Change_State(FSMSTATE::LIGHT_ATTACK);

        return true;
    }


    if (m_pOwner->Check(FSMSTATE::SKILL2))
    {
        m_pOwner->Reset_LightCombo();
        m_pFSM->Change_State(FSMSTATE::SKILL2);
        return true;
    }

    return false;
}


void CState_LightAttack::Free()
{
	__super::Free();
}

void CState_LightAttack::Describe_Entity()
{
}
