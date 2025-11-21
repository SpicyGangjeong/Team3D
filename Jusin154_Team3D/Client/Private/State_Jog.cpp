#include "pch.h"
#include "State_Jog.h"
#include "Unit.h"

CState_Jog::CState_Jog()
	:CState()
{
}

void CState_Jog::Enter()
{
	auto anim = m_pOwner->Get_AnimInfo(STATEANIM::JOG_FWD);
	m_pModel->Set_AnimationIndex(anim.first, anim.second);

	m_pOwner->Reset_Sprint();
	m_pOwner->Reset_Walk();
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

	if (m_pOwner->Check(FSMSTATE::WALK))
		m_pFSM->Change_State(FSMSTATE::WALK);

	else if (m_pOwner->Check(FSMSTATE::DODGE))
		m_pFSM->Change_State(FSMSTATE::DODGE);

	else if (!m_pOwner->Check(FSMSTATE::JOG))
		m_pFSM->Change_State(FSMSTATE::IDLE);

	if (m_pOwner->Check(FSMSTATE::JUMP))
		m_pFSM->Change_State(FSMSTATE::JUMP);


	return false;
}

void CState_Jog::Free()
{
	__super::Free();
}

void CState_Jog::Describe_Entity()
{
}
