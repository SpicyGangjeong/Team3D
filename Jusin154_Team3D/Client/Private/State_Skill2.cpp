#include "pch.h"
#include "State_Skill2.h"
#include "Unit.h"


CState_Skill2::CState_Skill2()
	:CState()
{
}

void CState_Skill2::Enter()
{
	auto anim = m_pOwner->Get_AnimInfo(STATEANIM::SKILL2);
	m_pModel->Set_AnimationIndex(anim.first, anim.second);
}

void CState_Skill2::Update(_float fTimeDelta)
{
	if (CheckExitState())
		return;
}

void CState_Skill2::Exit()
{
}

_bool CState_Skill2::CheckExitState()
{
	if (m_pModel->IsFinishedAnim())
		m_pFSM->Change_State(FSMSTATE::IDLE);

	return false;
}

void CState_Skill2::Free()
{
	__super::Free();
}

void CState_Skill2::Describe_Entity()
{
}
