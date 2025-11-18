#include "pch.h"
#include "State_Jog.h"
#include "Unit.h"

CState_Jog::CState_Jog()
	:CState()
{
}

void CState_Jog::Enter()
{
	if (m_pOwner->Check(FSMSTATE::WALK_FWD))
	{
		auto anim = m_pOwner->Get_AnimInfo(STATEANIM::WALK_FWD);
		m_pModel->Set_AnimationIndex(anim.first, anim.second);
	}

	if (m_pOwner->Check(FSMSTATE::WALK_BWD))
	{
		auto anim = m_pOwner->Get_AnimInfo(STATEANIM::WALK_BWD);
		m_pModel->Set_AnimationIndex(anim.first, anim.second);
	}
}

void CState_Jog::Update(_float fTimeDelta)
{
	if (CheckExitState())
		return;
}

void CState_Jog::Exit()
{
}

_bool CState_Jog::CheckExitState()
{
	if (m_pOwner->Check(FSMSTATE::SPRINT))
		m_pFSM->Change_State(FSMSTATE::SPRINT);

	else if (m_pOwner->Check(FSMSTATE::DODGE))
		m_pFSM->Change_State(FSMSTATE::DODGE);

	else if (!m_pOwner->Check(FSMSTATE::JOG))
		m_pFSM->Change_State(FSMSTATE::IDLE);

	return false;
}

void CState_Jog::Free()
{
	__super::Free();
}

void CState_Jog::Describe_Entity()
{
}
