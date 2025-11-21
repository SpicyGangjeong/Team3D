#include "pch.h"
#include "State_Jump.h"
#include "Unit.h"


CState_Jump::CState_Jump()
	:CState()
{
}

void CState_Jump::Enter()
{
	if (m_pOwner->Check(FSMSTATE::SPRINT))
	{
		auto anim = m_pOwner->Get_AnimInfo(STATEANIM::JUMP_SPRINT);
		m_pModel->Set_AnimationIndex(anim.first, anim.second);
		return;
	}
	if (m_pOwner->Check(FSMSTATE::JOG))
	{
		auto anim = m_pOwner->Get_AnimInfo(STATEANIM::JUMP_JOG);
		m_pModel->Set_AnimationIndex(anim.first, anim.second);
		return;
	}

	auto anim = m_pOwner->Get_AnimInfo(STATEANIM::JUMP);
	m_pModel->Set_AnimationIndex(anim.first, anim.second);
}

void CState_Jump::Update(_float fTimeDelta)
{
	if (CheckExitState())
		return;
}

void CState_Jump::Exit()
{
}

_bool CState_Jump::CheckExitState()
{
	if (m_pOwner->IsCurrentKeyFrame("Jump"))
		m_pFSM->Change_State(FSMSTATE::LAND);

	return false;
}

void CState_Jump::Free()
{
	__super::Free();
}

void CState_Jump::Describe_Entity()
{
}
