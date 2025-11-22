#include "pch.h"
#include "State_Idle.h"
#include "Unit.h"

CState_Idle::CState_Idle()
	:CState()
{
}

void CState_Idle::Enter()
{
	if (m_pFSM->Get_PrevState() == FSMSTATE::JOG)
	{
		auto anim = m_pOwner->Get_AnimInfo(STATEANIM::JOG_STOP);
		m_pModel->Set_AnimationIndex(anim.first, anim.second);
		return;
	}
	if (m_pFSM->Get_PrevState() == FSMSTATE::WALK)
	{
		auto anim = m_pOwner->Get_AnimInfo(STATEANIM::WALK_STOP);
		m_pModel->Set_AnimationIndex(anim.first, anim.second);
		return;
	}
	else
	{
		auto anim = m_pOwner->Get_AnimInfo(STATEANIM::IDLE);
		m_pModel->Set_AnimationIndex(anim.first, anim.second);
	}
}

void CState_Idle::Update(_float fTimeDelta)
{
	if (CheckExitState())
		return;
}

void CState_Idle::Exit()
{
}

_bool CState_Idle::CheckExitState()
{
	if (m_pOwner->Check(FSMSTATE::JOG))
		m_pFSM->Change_State(FSMSTATE::JOG);

	if (m_pOwner->Check(FSMSTATE::WALK))
		m_pFSM->Change_State(FSMSTATE::WALK);

	if (m_pOwner->Check(FSMSTATE::SPRINT))
		m_pFSM->Change_State(FSMSTATE::SPRINT);

	if (m_pOwner->Check(FSMSTATE::DODGE))
		m_pFSM->Change_State(FSMSTATE::DODGE);

	if (m_pOwner->Check(FSMSTATE::IDLE_TURN))
		m_pFSM->Change_State(FSMSTATE::IDLE_TURN);

	if (m_pOwner->Check(FSMSTATE::JUMP))
		m_pFSM->Change_State(FSMSTATE::JUMP);

	return false;
}

void CState_Idle::Free()
{
	__super::Free();
}

void CState_Idle::Describe_Entity()
{
}
