#include "pch.h"
#include "State_Skill.h"
#include "Unit.h"


CState_Skill::CState_Skill()
	:CState()
{
}

void CState_Skill::Enter()
{
	auto anim = m_pOwner->Get_AnimInfo(STATEANIM::SKILL);
	m_pModel->Set_AnimationIndex(anim.first, anim.second);
}

void CState_Skill::Update(_float fTimeDelta)
{
	if (CheckExitState())
		return;
}

void CState_Skill::Exit()
{
}

_bool CState_Skill::CheckExitState()
{

	return false;
}

void CState_Skill::Free()
{
	__super::Free();
}

void CState_Skill::Describe_Entity()
{
}
