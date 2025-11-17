#include "pch.h"
#include "State_Walk.h"
#include "Unit.h"

CState_Walk::CState_Walk()
	:CState()
{
}

void CState_Walk::Enter()
{
	if(m_pOwner->Check(FSMSTATE::WALK_FWD))
	{
		auto anim = m_pOwner->Get_AnimInfo(FSMSTATE::WALK_FWD);
		m_pModel->Set_AnimationIndex(anim.first, anim.second);
	}

	if (m_pOwner->Check(FSMSTATE::WALK_BWD))
	{
		auto anim = m_pOwner->Get_AnimInfo(FSMSTATE::WALK_BWD);
		m_pModel->Set_AnimationIndex(anim.first, anim.second);
	}
}

void CState_Walk::Update(_float fTimeDelta)
{
	if (CheckExitState())
		return;
}

void CState_Walk::Exit()
{
}

_bool CState_Walk::CheckExitState()
{
	if (m_pOwner->Check(FSMSTATE::SPRINT))
		m_pFSM->Change_State(FSMSTATE::SPRINT);

	else if (m_pOwner->Check(FSMSTATE::DODGE))
		m_pFSM->Change_State(FSMSTATE::DODGE);

	else if (!m_pOwner->Check(FSMSTATE::WALK))
		m_pFSM->Change_State(FSMSTATE::IDLE);

	return false;
}

void CState_Walk::Free()
{
	__super::Free();
}

void CState_Walk::Describe_Entity()
{
}
