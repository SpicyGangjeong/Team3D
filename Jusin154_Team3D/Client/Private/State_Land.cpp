#include "pch.h"
#include "State_Land.h"
#include "Unit.h"


CState_Land::CState_Land()
	:CState()
{
}

void CState_Land::Enter()
{
	auto anim = m_pOwner->Get_AnimInfo(STATEANIM::LAND);
	m_pModel->Set_AnimationIndex(anim.first, anim.second);
}

void CState_Land::Update(_float fTimeDelta)
{
	if (CheckExitState())
		return;
}

void CState_Land::Exit()
{
}

_bool CState_Land::CheckExitState()
{
	if (m_pModel->IsFinishedAnim())
		m_pFSM->Change_State(FSMSTATE::IDLE);


	if (m_pOwner->Check(FSMSTATE::JOG))
		m_pFSM->Change_State(FSMSTATE::JOG);

	if (m_pOwner->Check(FSMSTATE::SPRINT))
		m_pFSM->Change_State(FSMSTATE::SPRINT);

	return false;
}

void CState_Land::Free()
{
	__super::Free();
}

void CState_Land::Describe_Entity()
{
}
