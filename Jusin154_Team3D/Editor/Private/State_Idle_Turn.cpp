#include "pch.h"
#include "State_Idle_Turn.h"
#include "Unit.h"

CState_Idle_Turn::CState_Idle_Turn()
	:CState()
{
}

void CState_Idle_Turn::Enter()
{
	if (m_pGameInstance->Key_Down(DIK_LEFT))
	{
		auto anim = m_pOwner->Get_AnimInfo(STATEANIM::IDLE_TURN_L);
		m_pModel->Set_AnimationIndex(anim.first, anim.second);
	}
	if (m_pGameInstance->Key_Down(DIK_RIGHT))
	{
		auto anim = m_pOwner->Get_AnimInfo(STATEANIM::IDLE_TURN_R);
		m_pModel->Set_AnimationIndex(anim.first, anim.second);
	}
	if (m_pGameInstance->Key_Down(DIK_DOWN))
	{
		auto anim = m_pOwner->Get_AnimInfo(STATEANIM::IDLE_TURN_BWD);
		m_pModel->Set_AnimationIndex(anim.first, anim.second);
	}
}

void CState_Idle_Turn::Update(_float fTimeDelta)
{
	if (CheckExitState())
		return;
}

void CState_Idle_Turn::Exit()
{
}

_bool CState_Idle_Turn::CheckExitState()
{
	if (m_pOwner->Check(FSMSTATE::MOVE))
	{
		m_pFSM->Change_State(FSMSTATE::MOVE);

		if (m_pOwner->Check(FSMSTATE::WALK))
			m_pFSM->Change_State(FSMSTATE::WALK);                                     

		if (m_pOwner->Check(FSMSTATE::SPRINT))
			m_pFSM->Change_State(FSMSTATE::SPRINT);

		if (m_pOwner->Check(FSMSTATE::DODGE))
			m_pFSM->Change_State(FSMSTATE::DODGE);
	}

	if (m_pOwner->Check(FSMSTATE::COMBAT))
	{
		m_pFSM->Change_State(FSMSTATE::COMBAT);

		if (m_pOwner->Check(FSMSTATE::SKILL))
			m_pFSM->Change_State(FSMSTATE::SKILL);

		if (m_pOwner->Check(FSMSTATE::SKILL2))
			m_pFSM->Change_State(FSMSTATE::SKILL2);

		if (m_pOwner->Check(FSMSTATE::LIGHT_ATTACK))
			m_pFSM->Change_State(FSMSTATE::LIGHT_ATTACK);

		if (m_pOwner->Check(FSMSTATE::CAST))
			m_pFSM->Change_State(FSMSTATE::CAST);
	}

	if (m_pModel->IsFinishedAnim())
		m_pFSM->Change_State(FSMSTATE::IDLE);

	if (m_pOwner->Check(FSMSTATE::JUMP))
		m_pFSM->Change_State(FSMSTATE::JUMP);

	return false;
}

void CState_Idle_Turn::Free()
{
	__super::Free();
}

void CState_Idle_Turn::Describe_Entity()
{
}
