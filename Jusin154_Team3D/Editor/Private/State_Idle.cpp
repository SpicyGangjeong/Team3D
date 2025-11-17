#include "pch.h"
#include "State_Idle.h"
#include "Unit.h"

CState_Idle::CState_Idle()
	:CState()
{
}

void CState_Idle::Enter()
{
	auto anim = m_pOwner->Get_AnimInfo(FSMSTATE::IDLE);
	m_pModel->Set_AnimationIndex(anim.first, anim.second);
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
	if (m_pOwner->Check(FSMSTATE::WALK))
		m_pFSM->Change_State(FSMSTATE::WALK);

	if (m_pOwner->Check(FSMSTATE::DODGE))
		m_pFSM->Change_State(FSMSTATE::DODGE);

	if (m_pOwner->Check(FSMSTATE::SPRINT))
		m_pFSM->Change_State(FSMSTATE::SPRINT);

	if (m_pOwner->Check(FSMSTATE::JUMP))
		m_pFSM->Change_State(FSMSTATE::JUMP);

	if (m_pOwner->Check(FSMSTATE::SKILL))
		m_pFSM->Change_State(FSMSTATE::SKILL);

	if (m_pOwner->Check(FSMSTATE::LIGHT_ATTACK))
		m_pFSM->Change_State(FSMSTATE::LIGHT_ATTACK);

	if (m_pOwner->Check(FSMSTATE::CAST))
		m_pFSM->Change_State(FSMSTATE::CAST);

	return false;
}

void CState_Idle::Free()
{
	__super::Free();
}

void CState_Idle::Describe_Entity()
{
}
